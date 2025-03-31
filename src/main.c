#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <asndlib.h>
#include <mp3player.h>

// include generated header
#include "drums_red_mp3.h"
#include "drums_blue_mp3.h"
#include "drums_green_mp3.h"
#include "drums_yellow_mp3.h"
#include "drums_orange_mp3.h"
#include "drums_kick_mp3.h"

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
//---------------------------------------------------------------------------------

	// Initialise the video system
	VIDEO_Init();

	// Initialise the attached controllers
	WPAD_Init();

	// Initialise the audio subsystem
	ASND_Init();
	MP3Player_Init();

	// Obtain the preferred video mode from the system
	// This will correspond to the settings in the Wii menu
	rmode = VIDEO_GetPreferredMode(NULL);

	// Allocate memory for the display in the uncached region
	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

	// Initialise the console, required for printf
	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);

	// Set up the video registers with the chosen mode
	VIDEO_Configure(rmode);

	// Tell the video hardware where our display memory is
	VIDEO_SetNextFramebuffer(xfb);

	// Make the display visible
	VIDEO_SetBlack(false);

	// Flush the video register changes to the hardware
	VIDEO_Flush();

	// Wait for Video setup to complete
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

	// The console understands VT terminal escape codes
	// This positions the cursor on row 2, column 0
	// we can use variables for this with format codes too
	// e.g. printf ("\x1b[%d;%dH", row, column );
	printf("\x1b[2;0H");

	printf("A = Drums Red\n");
	printf("B = Drums Blue\n");
	printf("1 = Drums Green\n");
	printf("2 = Drums Yellow\n");
	printf("+ = Drums Orange\n");
	printf("- = Drums Kick\n");

	// MP3Player_PlayBuffer(sample_mp3, sample_mp3_size, NULL);

	while(1) {

		// Call WPAD_ScanPads each loop, this reads the latest controller states
		WPAD_ScanPads();

		// WPAD_ButtonsDown tells us which buttons were pressed in this loop
		// this is a "one shot" state which will not fire again until the button has been released
		u32 pressed = WPAD_ButtonsDown(0);

		// Handle button presses for drum sounds
		if (pressed & WPAD_BUTTON_A) {
			MP3Player_PlayBuffer(drums_red_mp3, drums_red_mp3_size, NULL);
		}
		if (pressed & WPAD_BUTTON_B) {
			MP3Player_PlayBuffer(drums_blue_mp3, drums_blue_mp3_size, NULL);
		}
		if (pressed & WPAD_BUTTON_1) {
			MP3Player_PlayBuffer(drums_green_mp3, drums_green_mp3_size, NULL);
		}
		if (pressed & WPAD_BUTTON_2) {
			MP3Player_PlayBuffer(drums_yellow_mp3, drums_yellow_mp3_size, NULL);
		}
		if (pressed & WPAD_BUTTON_PLUS) {
			MP3Player_PlayBuffer(drums_orange_mp3, drums_orange_mp3_size, NULL);
		}
		if (pressed & WPAD_BUTTON_MINUS) {
			MP3Player_PlayBuffer(drums_kick_mp3, drums_kick_mp3_size, NULL);
		}

		// We return to the launcher application via exit
		if ( pressed & WPAD_BUTTON_HOME ) {
            exit(0);
        }
		// Wait for the next frame
		VIDEO_WaitVSync();
	}

	return 0;
}
