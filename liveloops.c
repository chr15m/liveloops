/* 
 *    Live Loops!!!
 *
 *		Copyright 2001 Chris McCormick.
 *
 */

#include "loops.h"

int playing=TRUE;
int tickCount=0;

#define NUMSAMPLES 4					// Global define of the number of tracks we're going to use (yuck!)

/****************************************************************************
							increment the tick counter
****************************************************************************/

void TimerCallback(void)
{
if (playing) tickCount++;
}
END_OF_FUNCTION(TimerCallback);

int main(int argc, char *argv[])
{
	// track stuff
	struct track tracks[NUMSAMPLES];
	int selected=0;			// Which track is currently selected

	// song stuff
	int nextBit=0;			// the next bit to play
	int i = 0;          // what do you reckon?
	float T = 80.0, BPM = 180;		  // quater of a beat time in milliseconds ( 200 = 0.2 seconds)
	int quaterBeat=0, beat=63; // where abouts in the bar are we up to
	int whichKey=0;

	// file stuff
	int number_of_files = 0;	// tells us how many files in the samples directory
	char **filenames=NULL;		// an array of strings holding the filenames
	int file_selected=0;

	/* Set up the allegro stuff */
	printf("Setting up allegro\n");
	allegro_init();
	install_keyboard(); 
	install_timer();
	/* GFX_SAFE instead of windowed */
	printf("Setting gfx mode\n");
	if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0) != 0) 
		{
		if (set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, 640, 480, 0, 0) != 0)
			{
			set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
			printf("Unable to set any graphic mode\n%s\n", allegro_error);
			return 1;
			}
		}

	/* Calculate the initial BPM */
	T = 15000/BPM;

	/* lock our callback functions */
	printf("Locking timer variables\n");
	LOCK_VARIABLE(tickCount);
	LOCK_FUNCTION(TimerCallback);

	/* setup the graphics crap */
	printf("Setting up graphics\n");
	set_palette(desktop_palette);
	clear_to_color(screen, makecol(0, 0, 0));
	text_mode(-1);
	acquire_screen();

	/* install a digital sound driver */
	printf("Init sound device\n");
	if (install_sound(DIGI_AUTODETECT, MIDI_NONE, argv[0]) != 0) 
   		{
		textprintf(screen, font, 10, 10, makecol(255, 255, 255), "Error initialising sound system\n%s\n", allegro_error);
		return 1;
		}
	else
		{
		textprintf(screen, font, 10, 10, makecol(255, 255, 255), "Digital sound driver: %s", digi_driver->name);
		}

	/* Flag that we don't have anything playing. Reset variables. start samples rolling */
	printf("Initialize variables\n");
	for (i=0; i<NUMSAMPLES; i++)
		{
	  	tracks[i].pan = 0;
	  	tracks[i].pitch = 600;
	  	tracks[i].filename[0] = 0;
	  	tracks[i].bpl = 16;
	  	tracks[i].vol = 255;
	  	tracks[i].handle = -1;
		tracks[i].mutevol = 0;
		tracks[i].mysample = NULL;
		tracks[i].timestretch = 0;
	  	}
	selected = 0;

	/* start the callback every millisecond */
	printf("Installing timer\n");
  	install_int(TimerCallback, 1);

	set_volume(255, 255);

	textprintf(screen, font, 10, 20, makecol(255, 255, 255), "Loading directory listing");

	/* load in all our sample filenames */
	printf("Loading in filename array\n");
	filenames = listFiles(&number_of_files);

	/* release the screen and continue */
	printf("First screen update\n");
	release_screen();

	/* do a screen update */
	screenupdate(playing, beat, BPM, selected, NUMSAMPLES, tracks, number_of_files, filenames, file_selected);
	
	/***************************************************
	*				BEGIN THE MAIN LOOP HERE
	***************************************************/
	printf("Launch main\n");

   do 
   	{
		/* check each sample for timestretching */
		for (i=0; i<NUMSAMPLES; i++)
			{
			/* if this track is marked for timestretching */
			if (tracks[i].timestretch == 1)
				{
				/* figure out where in the sample we're meant to be playing */
				nextBit = tracks[i].mysample->len * ((beat*4*T + T*quaterBeat + tickCount) / (tracks[i].bpl*T*4));
				/* make sure we haven't exceeded the sample length (sounds nasty) */
				if (nextBit >= tracks[i].mysample->len) nextBit = nextBit % tracks[i].mysample->len;
				/* move the pointer to the right place */
				voice_set_position(tracks[i].handle, nextBit);
				}
			}
		
		/* at the end of the bar */
		if (beat == 64)
			{
			printf("End of bar\n");
			/* reset the beats */
			beat=0;
			
			/* stop all the samples, and start them again */
			for (i=0; i<NUMSAMPLES; i++)
				{
				if (tracks[i].filename[0] != 0) // (if the samples are loaded already)
					{
					stop_sample(tracks[i].mysample);
				   	tracks[i].handle = play_sample(tracks[i].mysample, tracks[i].vol, tracks[i].pan, tracks[i].pitch, TRUE);
				   	}
			   	}
			}

		/* when we hit one beat */
		if (quaterBeat == 4)
			{
			beat++;
			quaterBeat=0;

			/* update the position and the details of each sample */
			for (i=0; i<NUMSAMPLES; i++)
				{
				/* (if the samples are loaded already) */
				if (tracks[i].filename[0] != 0)
					{
					if (tracks[i].timestretch == 0)
						{
						/* make sure the sample is on the right beat */
						nextBit = tracks[i].mysample->len * beat / tracks[i].bpl;
						if (nextBit >= tracks[i].mysample->len) nextBit = nextBit % tracks[i].mysample->len;
						voice_set_position(tracks[i].handle, nextBit);
						}
					
					/* adjust all the crap */
					adjust_sample(tracks[i].mysample, tracks[i].vol, tracks[i].pan, tracks[i].pitch, TRUE);
					}
				}
			T = 15000/BPM;
			}

		/* increment the quater beat */
		if (tickCount >= T)
			{
			quaterBeat++;
			tickCount = tickCount - T;
			/* do a screen update */
			screenupdate(playing, beat, BPM, selected, NUMSAMPLES, tracks, number_of_files, filenames, file_selected);
			}

		/* check for a key in the keyboard buffer */
		if (keypressed()) whichKey = readkey() >> 8; 

		/* select a new track by number */
		if (whichKey == KEY_1)
			{
			selected = 0;
			}

		if (whichKey == KEY_2)
			{
			selected = 1;
			}

		if (whichKey == KEY_3)
			{
			selected = 2;
			}

		if (whichKey == KEY_4)
			{
			selected = 3;
			}
		
		/* set the volume to zero and back again */
		if (whichKey == KEY_SPACE)
			{
			if (tracks[selected].vol == 0) tracks[selected].vol = tracks[selected].mutevol;
			else
				{
				tracks[selected].mutevol = tracks[selected].vol;
				tracks[selected].vol = 0;
				}		
			}

      /* alter the pitch? */
      if ((whichKey == KEY_UP) && (tracks[selected].pitch < 16384))
			tracks[selected].pitch = ((tracks[selected].pitch * 513) / 512) + 1; 
      else if ((whichKey == KEY_DOWN) && (tracks[selected].pitch > 64))
	 		tracks[selected].pitch = ((tracks[selected].pitch * 511) / 512) - 1; 

		/* alter the number of beats per loop */
      if ((whichKey == KEY_RIGHT) && (tracks[selected].bpl < 64))
			tracks[selected].bpl++;
      else if ((whichKey == KEY_LEFT) && (tracks[selected].bpl > 1))
	 		tracks[selected].bpl--; 

		/* alter the volume */
      if ((whichKey == KEY_A) && (tracks[selected].vol < 255))
			tracks[selected].vol = tracks[selected].vol + 3;
      else if ((whichKey == KEY_Z) && (tracks[selected].vol > 0))
	 		tracks[selected].vol = tracks[selected].vol - 3; 

		/* toggle timestretching */
		if (whichKey == KEY_T)
			{
			if (tracks[selected].timestretch == 0)
				tracks[selected].timestretch = 1;
			else
				tracks[selected].timestretch = 0;
			}

		/* run through the selection of files until we find one we like */
      if ((whichKey == KEY_C) && (file_selected < number_of_files - 1))
			file_selected++;
      else if ((whichKey == KEY_X) && (file_selected > 0))
			file_selected--; 

		/* swap the panning */
      if ((whichKey == KEY_Q) & (tracks[selected].pan == 0)) tracks[selected].pan = 255;
      else if ((whichKey == KEY_Q) && (tracks[selected].pan == 255)) tracks[selected].pan = 0;

		/* change the beats per minute */
      if ((whichKey == KEY_W) & (BPM >= 3)) BPM--;
      else if ((whichKey == KEY_E) & (BPM < 400)) BPM++;

		// Match the pitch to the current speed
		if (whichKey == KEY_M) tracks[selected].pitch = (1000*(tracks[selected].mysample->len)/(tracks[selected].mysample->freq))/(tracks[selected].bpl*T*4/1000);
		
		/* Select the next track */
		if (whichKey == KEY_TAB) selected++;
		if (selected == NUMSAMPLES) selected = 0;

		/* load in a new sample on this track */
		if (whichKey == KEY_ENTER)
			{
			/* free the old sample */
			printf("stopping sample\n");			
			if (tracks[selected].filename[0] != 0)
				destroy_sample(tracks[selected].mysample);

			/* copy in our new filename */
			strcpy(tracks[selected].filename, filenames[file_selected]);
			
			/* try to load our new file */
			printf("loading sample %s\n", tracks[selected].filename);
			tracks[selected].mysample = load_wav(tracks[selected].filename);

			/* if the load is unsuccessful */
			if (tracks[selected].mysample == NULL)
				{
				/* make sure every part of the program knows it was not successful for this sample */
				printf("Sample %s not loaded\n", tracks[selected].filename);
				tracks[selected].filename[0] = 0;
				}
			else
				{
				/* otherwise set the pitch etc */
				printf("Sample length: %ld\n", tracks[selected].mysample->len);

				tracks[selected].pitch = (1000*(tracks[selected].mysample->len)/(tracks[selected].mysample->freq))/(tracks[selected].bpl*T*4/1000);
				printf("setting pitch: %d\n", tracks[selected].pitch);
	
				/* start it playing again */
				printf("playing sample\n");
			   	tracks[selected].handle = play_sample(tracks[selected].mysample, tracks[selected].vol, tracks[selected].pan, tracks[selected].pitch, TRUE); 
				}
			}
		
		whichKey = (int)NULL;
   	}
   while (!key[KEY_ESC]);

   /* destroy the sample */
	for (i=0; i<NUMSAMPLES; i++)
	  	{
		if (tracks[i].filename[0] != 0)
			destroy_sample(tracks[i].mysample);
		}
	
   return 0;
}
END_OF_MAIN()

