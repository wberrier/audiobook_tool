* Add some test suites
* Print ffmpeg output on error
* Add option to rip_cds to skip cddb lookup
* check to make sure that first chapter mark is at 0 (maybe when
  setting the chapters)  Otherwise, chapter marks are off.
* add back play duration support (I think only mplayer supports this)
  Idea being you can play each track for 1 second to easily scan them
* handle case where ffmpeg fails to encode (probably with shell magic)
  Otherwise, when creating an audiobook tool fails, it can be
  difficult to track down
* normalize audio?
* document typical use cases
   1. Combining a whole bunch of mp3 (or any format that ffmpeg supports) to a single
      .m4b file in one swoop.
   2. Doing each step manually so you can adjust the chapter list. (Note, you can also
      do use case #1, and then extract chapter list, modify it, and then set it again)
   3. Ripping an audiobook from cds only differs in that you create the input files
      (wav files) first, and then continue with the other use cases
* Add a verbose option, (ie: the user probably doesn't want to see the ffmpeg
  input output)

* implement "gen_chapter_list_from_cddbid".
  it would fetch track info from the cddb and spit out the chapters

  Not really worth doing since cddb reports track lengths in seconds
  instead of milliseconds.

* be able to submit tracks from chapter list to cddb?

* Convert abtpickch to python

 One potential problem of just using disc ids to generate chapter
 lists is that the total cd length as reported by cddb is in seconds
 (instead of something more granular).  It would probably be a little
 bit off, and require manual adjusting.

 This doesn't matter for having the actual cds, because we get lengths
 from the real ripped audio.

* convert filenames and arguments to use hyphens instead of
  underscores?  Would we rename the repo?
