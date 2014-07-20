#include <string.h>

#include <bitbox.h>
#include <fatfs/ff.h>
#include <fatfs/diskio.h>

#include "mod32.h"
FATFS fso;        // The FATFS structure (file system object) holds dynamic work area of individual logical drives
DIR dir;          // The DIR structure is used for the work area to read a directory by f_oepndir, f_readdir function
FILINFO fileInfo; // The FILINFO structure holds a file information returned by f_stat and f_readdir function
FIL file;         // The FIL structure (file object) holds state of an open file


#define MOD_PATH "MODS"

int sample_in_tick;

void loadNextFile() {
	int res;
	char *dotPointer;

	do {
		res=f_readdir(&dir, &fileInfo);
		if (res) {
			message ("wtf cannot read dir\n");
			die(4,res);
		}
		if(fileInfo.fname[0] == 0) break;
		dotPointer = strrchr(fileInfo.fname, '.');
	} while(dotPointer == NULL || strcmp(dotPointer, ".MOD"));

	if(fileInfo.fname[0] != 0) {
		f_open(&file, fileInfo.fname, FA_READ);
		message("Opened File: %s ",fileInfo.fname);
		loadMod();
		message("Song name: [%s]\n",Mod.name);
	}
}


void game_init()
{
	// XXX  better check those errors !
	int res;

	f_mount(&fso,"",1); //mount now
	
	res = f_opendir(&dir, MOD_PATH);
	if (res) {
		message ("wtf no dir\n");
		die(3,res);
	}
	
	res = f_chdir(MOD_PATH);
	if (res) {
		message ("wtf no dir\n");
		die(2,res);
	}

	loadNextFile();
	audio_on=1;
}


void game_frame()
{
}

void game_line()
{
}

int prev_but;
void game_snd_buffer(uint16_t *stream, int size)
{
	for (int i=0;i<size; i++) {

		if (sample_in_tick++==Player.samplesPerTick) {

			// check button for next song ...
			if (!button_state() && prev_but){
				set_led(vga_frame & 64);
				loadNextFile();
			} else {
				player();
			}
			prev_but=button_state();
			sample_in_tick=0;
		}
		stream[i] = mixer(stream); 
	}
} 