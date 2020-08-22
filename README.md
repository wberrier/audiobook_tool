# audiobook_tool README

## History

I took a job where I was driving 100 miles every day.  The audiobook became my
new best friend.

A quick google search showed that there weren't any nice linux tools to create
audiobooks, so I started hacking some scripts together.  They were hacky, but
I only had to use them every few weeks when I wanted a new audiobook.

After listening to dozens of books, I started collecting a list of features
that I wanted my authored books to have.

An audiobook-interested coworker knew how to create chapters for mp4 books,
and I started adding scripts to get chapter goodness.

There were a mess of scripts and really they were only useful to me.  I had
different scripts for making books from cds, mp3s, etc, and I eventually
combined them into an encompassing tool to make them easier to use.  This way
I didn't have to re-learn my hacky scripts every time I used them.

Through more and more usage as well as input from the previously mentioned
coworker, the tool evolved to something useful for at least one other person.

This other person (we may as well identify him: Joel) started adding cool
features so I eventually posted the code to ease collaboration.

## Containers / Codecs

For Apple devices, m4b probably works the best.  Most Android players
will also support m4b.

For internet friendly (ie: no patents/royalties) codecs and
containers, mka, ogg, and opus are preferred.  I think mka (Matroska)
is the best overall generic container, although it's not terribly
popular for audiobooks (nor audio only for that matter).  Opus works
really well for low bitrate speech.

Now that the ogg container has a [formal extension for
chapters](https://wiki.xiph.org/Chapter_Extension), opus and ogg work
pretty well also.  But, the tooling for vorbis and opus don't seem to
be interchangeable, even though they both use the ogg container (hence
the preference for Matroska).  Opus is a more versatile codec than
vorbis in that it works well for low and high bitrates.

mp3 support could probably be enhanced to support chapters and
metadata via ID3v2 tags, but this has not been a priority.

## Players

### Android

#### Listen

* m4b: all metadata supported

* mka: does not show up in list

* webm:
  * [-] title
  * [?] author
  * [-] cover art

* opus:
  * [x] title works
  * [?] author
  * [-] cover art

* ogg:
  * [x] title works
  * [?] author
  * [x] cover art

#### Voice

* m4b: all metadata supported

* mka: all metadata supported (my preferred container/codec)

* webm: all metadata supported

* opus: all metadata supported

* ogg: all metadata supported

### Data CD

Many cd players, particularly in automobiles, support playing mp3
files.  The recommended layout seems to be to have a "Title" folder
containing files.  The files can each have metadata tags, which can be
displayed.

Easytag is a nice way to set the metadata.  For each file:

* Artist: Author
* Album: Title
* Title: Chapter Name

The 2013 Ford Fusion supports mp3 and wma.  What other formats are
common for automobiles?

## Splitting File

Sometimes it's helpful to split an audio file into multiple files.
This can be done with Audacity:

https://manual.audacityteam.org/man/splitting_a_recording_into_separate_tracks.html

If the source file is large, exporting to flac works well (ie: wav
files seem to have a 6GB filesize max)

# Other projects

* http://audiotools.sourceforge.net/ (doesn't seem to handle chapters)

