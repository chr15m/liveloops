#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include "allegro.h"

struct track 
	{
	SAMPLE *mysample; 		/* array of tracks */
	char filename[60]; 		/* filename of this track */
	int handle;				/* handle for each time the sample is triggered */
	int bpl;				/* BEATS PER LOOP */
	int pan;				/* pan of this track */
	int vol;				/* volume */
	int mutevol;			/* muted return volume */
	int pitch;				/* pitch in allegro format of this track */
	char timestretch;		/* whether it's timestretching or not */
	};

char ** listFiles(int *number_of_files);
void instructions(void);
void screenupdate(int playing, int beat, int BPM, int selected, int number_of_tracks, struct track *tracks, int number_of_files, char **filenames, int file_selected);
void TimerCallback(void);