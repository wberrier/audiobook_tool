LDLIBS=-lavformat -lavutil -lavcodec -lstdc++ -lm
CXXFLAGS=-O3 -I/usr/include/ffmpeg -std=c++11 -Wno-deprecated-declarations

silence: silence.o

silence.o: silence.cpp

clean:
	rm -f silence.o silence
