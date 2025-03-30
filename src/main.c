#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogc/system.h>
#include <ogc/video.h>
#include <ogc/consol.h>
#include <ogc/audio.h>
#include <ogc/usb.h>
#include "audio/audio.h"
#include "game/modes.h"
#include "game/logic.h"
#include "ui/menu.h"

// Global state
static game_state_t game_state;
static bool running = true;

// Function declarations
static void init_system(void);
static void init_audio(void);
static void init_game(void);
static void main_loop(void);
static void cleanup(void);

// Menu callbacks
static void start_free_play(void) {
    game_mode_set(&game_state, MODE_FREE_PLAY);
}

static void start_practice(void) {
    game_mode_set(&game_state, MODE_PRACTICE);
}

static void quit_game(void) {
    running = false;
}

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    // Initialize everything
    init_system();
    init_audio();
    init_game();

    // Main game loop
    main_loop();

    // Cleanup
    cleanup();

    return 0;
}

static void init_system(void) {
    // Initialize video system
    VIDEO_Init();
    
    // Initialize console on screen
    CON_Init(0, 20, 20, 640, 480, 640*VI_DISPLAY_PIX_SZ);

    // Initialize USB for instruments
    USB_Initialize();
    
    // Set up video
    GXRModeObj *rmode = VIDEO_GetPreferredMode(NULL);
    VIDEO_Configure(rmode);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    if(rmode->viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();

    // Allocate framebuffer
    void *framebuffer = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    VIDEO_ClearFrameBuffer(rmode, framebuffer, COLOR_BLACK);
    VIDEO_SetNextFramebuffer(framebuffer);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    if(rmode->viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();
}

static void init_audio(void) {
    // Initialize audio system
    ASND_Init();
    ASND_Pause(0);
}

static void init_game(void) {
    // Initialize game state
    memset(&game_state, 0, sizeof(game_state));
    game_mode_init(&game_state);
    
    // Set up menu
    menu_init();
    menu_add_item("Free Play", start_free_play);
    menu_add_item("Practice", start_practice);
    menu_add_item("Quit", quit_game);
}

static void main_loop(void) {
    while (running) {
        // Handle USB events
        USB_ScanInputDevices();
        
        // Update game state
        game_mode_update(&game_state);
        
        // Draw frame
        VIDEO_WaitVSync();
        game_mode_draw(&game_state);
        
        // Check for exit condition
        if (!game_mode_is_running(&game_state)) {
            running = false;
        }
    }
}

static void cleanup(void) {
    // Cleanup game
    game_mode_cleanup(&game_state);
    menu_cleanup();
    
    // Cleanup audio
    ASND_Pause(1);
    ASND_End();
    
    // Cleanup video
    VIDEO_SetBlack(TRUE);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    VIDEO_Shutdown();
    
    // Cleanup USB
    USB_Deinitialize();
} 