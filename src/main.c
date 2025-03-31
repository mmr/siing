#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <unistd.h>
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
static WPADData *wpad_data = NULL;

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
	console_init(xfb, 20, 20, rmode->fbWidth, rmode->xfbHeight, rmode->fbWidth * VI_DISPLAY_PIX_SZ);

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
	if (rmode->viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();

	// The console understands VT terminal escape codes
	// This positions the cursor on row 2, column 0
	// we can use variables for this with format codes too
	// e.g. printf ("\x1b[%d;%dH", row, column );
	printf("\x1b[2;0H");

	while (1) {
		// Call WPAD_ScanPads each loop, this reads the latest controller states
		WPAD_ScanPads();

		// Get the controller data
		wpad_data = WPAD_Data(0);

		if (wpad_data) {
			printf("Controller Type: %d\n", wpad_data->exp.type);
		}

        if (wpad_data) {
            // Print newly pressed buttons
            if (wpad_data->btns_d) {
                printf("Newly pressed buttons: ");
                if (wpad_data->btns_d & WPAD_BUTTON_1) printf("1 ");
                if (wpad_data->btns_d & WPAD_BUTTON_2) printf("2 ");
                if (wpad_data->btns_d & WPAD_BUTTON_A) printf("A ");
                if (wpad_data->btns_d & WPAD_BUTTON_B) printf("B ");
                if (wpad_data->btns_d & WPAD_BUTTON_PLUS) printf("+ ");
                if (wpad_data->btns_d & WPAD_BUTTON_MINUS) printf("- ");
                if (wpad_data->btns_d & WPAD_BUTTON_HOME) printf("HOME ");
                printf("\n");
            }
        }


		// if (wpad_data) {
		// 	// Get the drum pad states
		// 	u32 drum_pads = wpad_data->btns_d;

        //     if (drum_pads & WPAD_BUTTON_1) {  // Red pad
        //         MP3Player_PlayBuffer(drums_red_mp3, drums_red_mp3_size, NULL);
        //     }
        //     if (drum_pads & WPAD_BUTTON_2) {  // Yellow pad
        //         MP3Player_PlayBuffer(drums_yellow_mp3, drums_yellow_mp3_size, NULL);
        //     }
        //     if (drum_pads & WPAD_BUTTON_A) {  // Blue pad
        //         MP3Player_PlayBuffer(drums_blue_mp3, drums_blue_mp3_size, NULL);
        //     }
        //     if (drum_pads & WPAD_BUTTON_B) {  // Green pad
        //         MP3Player_PlayBuffer(drums_green_mp3, drums_green_mp3_size, NULL);
        //     }
        //     if (drum_pads & WPAD_BUTTON_PLUS) {  // Orange pad
        //         MP3Player_PlayBuffer(drums_orange_mp3, drums_orange_mp3_size, NULL);
        //     }
        //     if (drum_pads & WPAD_BUTTON_MINUS) {  // Kick pedal
        //         MP3Player_PlayBuffer(drums_kick_mp3, drums_kick_mp3_size, NULL);
        //     }
		// }

		// We return to the launcher application via exit
		if (wpad_data && (wpad_data->btns_d & WPAD_BUTTON_HOME)) {
			exit(0);
		}

		// Wait for the next frame
		VIDEO_WaitVSync();
        usleep(1000000);
	}

	return 0;
}
