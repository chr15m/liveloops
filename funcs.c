#include "loops.h"

/********************************************************************************
		This function lists all the files in the samples directory onto the screen
********************************************************************************/

char ** listFiles(int *number_of_files)
{
DIR *dp;
struct dirent *dirp;
int size = 0; /* we have zero files registered so far! */
char **filenames=NULL;

if ((dp=opendir("samples"))==NULL)
	{
	textprintf(screen, font, 10, 30, makecol(255, 255, 255), "Samples directory does not exist");
	exit(1);
	}

while ((dirp=readdir(dp))!=NULL)
	{
	if (strcmp(".", dirp->d_name) & strcmp("..", dirp->d_name))
		{
		size++;	/* we have another file ready! */
		filenames = (char **)realloc(filenames, size * sizeof(char*));
		filenames[size - 1] = malloc(sizeof(char) * 60);	/* ok ok it's yucky to assume they're always going */
		filenames[size - 1][0] = 0;
		strcat(filenames[size - 1], "samples/");		/* to be 60 long but really....who gives a fuck? */
		strcat(filenames[size - 1], dirp->d_name);	/* Put this file into our array of files */
		}
	}
closedir(dp);
*number_of_files = size;
return filenames;
}

/****************************************************************************
									UPDATES THE DISPLAY
****************************************************************************/

#define BLOCKHEIGHT 90

void screenupdate(int playing, int beat, int BPM, int selected, int number_of_tracks, struct track *tracks, int number_of_files, char **filenames, int file_selected)
{
int i, b; // loop counters
int tmpVol; // temp variable to hold the volume
BITMAP *buffer;
buffer = create_bitmap(640, 480);

/* clear the buffer and aquire */
clear_to_color(buffer, makecol(0, 0, 0));

if (playing) textprintf(buffer, font, 10, 10, makecol(255, 255, 255), "Status: Playing");
else textprintf(buffer, font, 10, 10, makecol(255, 255, 255), "Status: Stopped");
textprintf(buffer, font, 10, 20, makecol(255, 255, 255), "Beat marker: %d", beat);
textprintf(buffer, font, 10, 30, makecol(255, 255, 255), "Speed: %d bpm (W & E to change)", BPM);

for (i=0; i<number_of_tracks; i++)
	{
	/* if this is the selected track, then let the user know */
	if (i==selected) textprintf(buffer, font, 10, i*BLOCKHEIGHT + 40, makecol(255, 255, 255), "[---]");

	/* print the name of this track */
	textprintf(buffer, font, 10, i*BLOCKHEIGHT + 50, makecol(255, 255, 255), "[ %s ]", tracks[i].filename);

	/* display the pitch at which it's played */
	textprintf(buffer, font, 10, i*BLOCKHEIGHT + 60, makecol(255, 255, 255), "Pitch (UP & DN): %d", tracks[i].pitch);
	
	/* display the number of beats per loop of this track by putting a string of equals signs */
	textprintf(buffer, font, 10, i*BLOCKHEIGHT + 70, makecol(255, 255, 255), "Beats per loop (L & R): %d", tracks[i].bpl);

	/* display whether time stretching is turned on or not */
	textprintf(buffer, font, 10, i*BLOCKHEIGHT + 80, makecol(255, 255, 255), "Time Stretching (T): %d", tracks[i].timestretch);

	/* display the pitch at which it's played */
	textprintf(buffer, font, 10 , i*BLOCKHEIGHT + 90, makecol(255, 255, 255), "Pan (Q): ");
	if (tracks[i].pan) textprintf(buffer, font, 10 + 8 * 9, i*BLOCKHEIGHT + 90, makecol(255, 255, 255), "Right");
	else textprintf(buffer, font, 10 + 8 * 9, i*BLOCKHEIGHT + 90, makecol(255, 255, 255), "Left");

	/* display the volume the same way as bpl */
	textprintf(buffer, font, 10, i*BLOCKHEIGHT + 100, makecol(255, 255, 255), "Volume (A & Z): |");
	tmpVol = (tracks[i].vol/10);
	for (b=0; b<tmpVol; b++)
		textprintf (buffer, font, 144 + b * 8, i*BLOCKHEIGHT + 100, makecol(255, 255, 255), "=");

	/* if this is the selected track, then let the user know */
	if (i==selected) textprintf(buffer, font, 10, i*BLOCKHEIGHT + 110, makecol(255, 255, 255), "[---]");
	}

textprintf(buffer, font, 10, (number_of_tracks-1) * BLOCKHEIGHT + 120, makecol(255, 255, 255), "File list (X & C to navigate, Enter to load into current track)");

for (i=(file_selected - 2); i<=(file_selected + 2); i++)
	{
	if (i == file_selected)
		textprintf(buffer, font, 10, 150 + (number_of_tracks-1) * BLOCKHEIGHT , makecol(255, 255, 255), "[ %s \t]", filenames[i]);	
	else if ((i >= 0) & (i < number_of_files))
		textprintf(buffer, font, 10, 150 + (number_of_tracks-1) * BLOCKHEIGHT + (i - file_selected) * 10, makecol(255, 255, 255), "  %s", filenames[i]);
	}

/* update the screen */
acquire_screen();
blit(buffer, screen, 0, 0, 0, 0, 640, 480);
release_screen();
destroy_bitmap(buffer);
}

