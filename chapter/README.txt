Tools for chapterizing books
----------------------------

The tools in the directory are a work in progress. They're
posted here on the off chance that they'll be useful to
somebody who uses `audiobook_tool`.

To build silence, you'll need ffmpeg or libav development packages. Typically
ffmpeg-devel will get everything needed on Fedora, and on Debian or Ubuntu,
these should do it: libavutil-dev libavcodec-dev libavformat-dev

Once you have all the needed libraries and a working C++ compiler, just
run "make" to build.

The other two tools are a quick perl script to translate the output from
silence into a list of chapter candidates for any chapters that fall
within a given length range and a bash script to iteratively use
`audiobook_tool` to listen to candidate chapters to select all the
actual chapters.

Workflow
--------

1. Make a list of all the places in the audio file where silence is present

     silence book.m4b > silence.txt

2. Generate a list of likely chapters based upon silence length

     silence2chaps 3 < silence.txt > chaps.txt; wc -l chaps.txt

   Adjust the silence length until you get approximately the right number of
   chapters.

3. Listen to each chapter candidate and select the ones that are actually
   chapters

     abtpickch chaps.txt book.m4b chaps2.txt

   Save chapters with 'q', skip them with CTRL-c.

   Then if there are some missing, you can hunt for them by repeating steps 2 and
   3 looking at smaller silence lenghts. Don't forget to sort your chapters and
   name them when you've found them all.

   Here's an example showing how to look next at silences between 2.5 and 3 seconds
   during the second pass.

     silence2chaps 2.5 3 < silence.txt > chaps.txt; wc -l chaps.txt
