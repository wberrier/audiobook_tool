
// Why don't these headers have an #ifdef __cplusplus \ extern "C" { \ #endif?
// It's bad form to make developers wrap them
#ifdef __cplusplus
#define __STDC_CONSTANT_MACROS
extern "C"
{
#endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>

#ifdef __cplusplus
}
#endif

#include <vector>
#include <cmath>
#include <string>
#include <iostream>
 
void die(const char* message)
{
    fprintf(stderr, "%s\n", message);
    exit(1);
}

inline uint32_t get_sample(const uint8_t * buf, uint8_t sample_size, bool is_fp)
{
    int16_t s;
    int32_t l;
    float f;
    double d;
    // uses memcpy() instead of a cast to avoid alignment problems
    switch (sample_size)
    {
        case 1:
            return abs(int16_t(*buf) - 128) << 24;
            break;
        case 2:
            memcpy(&s, buf, sizeof(s));
            return abs(s) << 16;
        case 4:
            if (is_fp)
            {
                memcpy(&f, buf, sizeof(f));
                return abs(f * (1 << 31));
            }
            memcpy(&l, buf, sizeof(l));
            return abs(l);
        case 8:
            memcpy(&d, buf, sizeof(d));
            return abs(d * (1 << 31));
        default:
            std::cerr << "Unsupported sample size: " << uint16_t(sample_size) << std::endl;
            exit(1);
    }
}
 
int main(int argc, char* argv[])
{
    double decibels = -21.07209969647868366503; // -21 dB is around 1/128th, or less than 1% total power
    if (argc < 2)
    {
        die("Please provide the file path as the first argument");
    }
    if (argc > 2)
    {
        try
        {
            decibels = std::stof(argv[2]);
        }
        catch(...)
        {
            std::cerr << "Unable to convert argument to floating-point dB value: " << argv[2] << std::endl;
            exit(1);
        }
        if (decibels > 0)
        {
            std::cerr << "Silence value greather than 0 means that nothing is considered 'silence'" << std::endl;
            exit(1);
        }
        if (decibels > -12) // -12 dB is around 1/16th, or 6.25% total power
        {
            std::cerr << "Silence value of " << decibels << " dB is very high. Are you sure you want it so high?" << std::endl;
        }
    }
 
    const char* input_filename = argv[1];
 
    // This call is necessarily done once in your app to initialize
    // libavformat to register all the muxers, demuxers and protocols.
    av_register_all();
 
    // A media container
    AVFormatContext* container = 0;
 
    uint8_t samples_to_save = 0;
    uint8_t samples_to_save_mask = 0;
    uint8_t average_to_shift = 0;
    uint8_t sample_size = 0;
    bool planar_sample = false;
    bool fp_sample = false;
    uint64_t average = 0;
    uint64_t sample_index = 0;
    uint32_t silence_index = 0;
    std::vector<uint32_t> samples;
    uint64_t silence_start_sample = 0;
    const uint32_t silence_value = 0xffffffffl * std::exp(std::log(10.0) * decibels / 10.0);
 
    if (avformat_open_input(&container, input_filename, NULL, NULL) < 0)
    {
        die("Could not open file");
    }
 
    if (avformat_find_stream_info(container, NULL) < 0)
    {
        die("Could not find file info");
    }
 
    int stream_id = -1;
 
    // To find the first audio stream. This process may not be necessary
    // if you can gurarantee that the container contains only the desired
    // audio stream
    for (int i = 0; i < container->nb_streams; i++)
    {
        if (container->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            stream_id = i;
            break;
        }
    }
 
    if (stream_id == -1)
    {
        die("Could not find an audio stream");
    }
 
    // Find the apropriate codec and open it
    AVCodecContext* codec_context = container->streams[stream_id]->codec;
 
    AVCodec* codec = avcodec_find_decoder(codec_context->codec_id);
 
    if (!avcodec_open2(codec_context, codec, NULL) < 0)
    {
        die("Could not find open the needed codec");
    }

    AVPacket packet;
    AVFrame *decoded_frame = NULL;
 
    while (1)
    {
        int got_frame = 0;
        if (!decoded_frame)
        {
            if (!(decoded_frame = avcodec_alloc_frame()))
            {
                die("Could not allocate decoded frame buffer. Out of memory.");
            }
        }
        else
        {
            avcodec_get_frame_defaults(decoded_frame);
        }
 
        // Read one packet into 'packet'
        if (av_read_frame(container, &packet) < 0)
        {
            break;  // End of stream. Done decoding.
        }
 
        // Decodes from 'packet' into the buffer
        if (avcodec_decode_audio4(codec_context, decoded_frame, &got_frame, &packet) < 1)
        {
            break;  // Error in decoding
        }
        if (got_frame)
        {
            if (!samples_to_save)
            {
                switch (codec_context->sample_rate)
                {
                    case 8000:
                        samples_to_save = 8;
                        break;
                    case 22050:
                        samples_to_save = 16;
                        break;
                    case 44100:
                    case 48000:
                        samples_to_save = 32;
                        break;
                    default:
                        die("Unsupported sample rate");
                        break;
                }
                switch (codec_context->sample_fmt)
                {
                    case AV_SAMPLE_FMT_U8P:
                        planar_sample = true; // fall through
                    case AV_SAMPLE_FMT_U8:
                        sample_size = 1;
                        break;
                    case AV_SAMPLE_FMT_S16P:
                        planar_sample = true; // fall through
                    case AV_SAMPLE_FMT_S16:
                        sample_size = 2;
                        break;
                    case AV_SAMPLE_FMT_S32P:
                        planar_sample = true; // fall through
                    case AV_SAMPLE_FMT_S32:
                        sample_size = 4;
                        break;
                    case AV_SAMPLE_FMT_FLTP:
                        planar_sample = true;
                        sample_size = 4;
                        fp_sample = true;
                        break;
                    case AV_SAMPLE_FMT_DBLP:
                        planar_sample = true;
                        sample_size = 8;
                        fp_sample = true;
                        break;

                    default:
                        std::cerr << "Unsupported sample size: " << codec_context->sample_fmt << std::endl;
                        exit(1);
                        break;
                }
                samples_to_save_mask = samples_to_save - 1;
                for (int i = samples_to_save_mask; i; ++average_to_shift)
                {
                    i >>= 1;
                }
                samples.resize(samples_to_save);
            }
            // data now at decoded_frame->data[0] (len data_size)
            for (int i = 0; i < decoded_frame->nb_samples; ++i, ++sample_index)
            {
                if (sample_index >= samples_to_save)
                {
                    // remove old sample value from rolling average prior to adding new one
                    average -= samples[sample_index & samples_to_save_mask];
                }
                uint64_t sample_value = 0;
                for (int chan = 0; chan < codec_context->channels; ++chan)
                {
                    int index1 = planar_sample ? chan : 0;
                    int index2 = planar_sample ?
                        i * sample_size :
                        i * (sample_size * codec_context->channels) + (sample_size * chan);
                    sample_value += get_sample(&decoded_frame->data[index1][index2], sample_size, fp_sample);
                }
                switch (codec_context->channels)
                {
                    case 0:
                    case 1:
                        break;
                    case 2:
                        sample_value /= 2;
                    default:
                        sample_value /= codec_context->channels;
                }
                samples[sample_index & samples_to_save_mask] = sample_value;
                average += sample_value;
                if ((average >> average_to_shift) < silence_value && // 1/128th power is -21dB
                        (!silence_start_sample || sample_index == samples_to_save))
                {
                    silence_start_sample = sample_index;
                }
                else if ((average >> average_to_shift) >= silence_value && silence_start_sample)
                {
                    // silence is longer than one-half second
                    if (sample_index - silence_start_sample > codec_context->sample_rate / 2)
                    {
                        printf("%u\t%u\t%0.4f\n",
                                uint32_t((1000 * (sample_index - silence_start_sample))/codec_context->sample_rate),
                                ++silence_index,
                                (float)sample_index/codec_context->sample_rate - 0.23); // 0.23 is a comfortable gap prior to the end of the silence
                    }
                    silence_start_sample = 0;
                }
            }
        }
    }
    av_free(decoded_frame);
    avformat_close_input(&container);
    return 0;
}
