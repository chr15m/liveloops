Firstly, this code is copyright 2001 Chris McCormick.
It is licensed under the GPL (see COPYING in the directory above)
-----------------------------------------------------

You need to have allegro library installed to compile/run this:
Download the latest Work in Progress version from here: http://sunsite.dk/allegro/wip.html

NOTICE: This program won't run unless you create a sub-directory where you are executing it called 'samples'
with all of your samples in it (wav format).

This program will attempt to run in a window. If that fails it will attempt to use framebuffer (under linux).

Essentially i wrote this for my own purposes and i wanted a very simple way of putting loops together, and outputting
them through existing soundcard hardware into an external mixer.

TODO:

1) put in some code to send a pulse to the parallel port to control the timing of my Commodore64 computer (running 
custom music software found on this server in /dev/aSid/current/) This functionality was present in v0.1 before I ported 
to linux.

2) Put in some audio-input functionality so that a user could record a bar of say, guitar, and that
bar would be looped over whilst they played other stuff over the top. Nice for making very live
sounding looped music. Sort of like a delay pedal.

If you want to add to this or have any comments email me here:
chris@mccormick.cx

Enjoy!
