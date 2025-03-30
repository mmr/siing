#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ogcsys.h>
#include <ogc/usb.h>
#include <ogc/audio.h>
#include <ogc/console.h>
#include <ogc/video.h>
#include <ogc/video_types.h>

// Include our custom headers
#include "usb/device.h"
#include "audio/input.h"
#include "ui/menu.h"
#include "game/modes.h"
#include "game/logic.h"

// Function declarations
static void init_system(void);
static void init_audio(void);
static void init_usb(void);
static void init_game(void);
static void main_loop(void);
static void cleanup(void);

// Menu callbacks
static void start_free_play(void) {
    game_mode_set(GAME_MODE_FREE_PLAY);
}

static void start_practice(void) {
    game_mode_set(GAME_MODE_PRACTICE);
}

static void exit_game(void) {
    // TODO: Implement proper exit
    exit(0);
}

int main(int argc, char *argv[]) {
    // Initialize system
    init_system();
    
    // Initialize subsystems
    init_audio();
    init_usb();
    init_game();
    
    // Initialize menu
    menu_init();
    menu_add_item("Free Play", start_free_play);
    menu_add_item("Practice Mode", start_practice);
    menu_add_item("Exit", exit_game);
    
    // Enter main game loop
    main_loop();
    
    // Cleanup
    cleanup();
    
    return 0;
}

static void init_system(void) {
    // Initialize video
    VIDEO_Init();
    
    // Initialize console
    console_init(NULL, 80, 25, 80, 25, CONSOLE_PRIORITY_USER);
    
    // Initialize system
    SYS_Init();
    
    // Initialize USB subsystem
    USB_Init();
    
    // Set video mode
    VIDEO_SetMode(MODE_0_NTSC);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    
    // Initialize framebuffer
    void *framebuffer = MEM_K0_TO_K1(SYS_AllocateFramebuffer(VI_DISPLAY_PIXEL_FORMAT_RGBA8888));
    VIDEO_Configure(VIDEO_BUF0);
    VIDEO_SetNextFramebuffer(framebuffer);
    VIDEO_Flush();
    VIDEO_WaitVSync();
}

static void init_audio(void) {
    // Initialize audio system
    ASND_Init();
}

static void init_usb(void) {
    // Initialize USB devices
    usb_detect_devices();
}

static void init_game(void) {
    // Initialize game systems
    game_mode_init();
    game_logic_init();
}

static void main_loop(void) {
    while(1) {
        // Process USB inputs
        usb_handle_input(NULL);
        
        // Process audio
        audio_input_process(NULL);
        
        // Handle menu input if in menu
        menu_handle_input();
        
        // Update game state
        game_logic_update();
        game_mode_update();
        
        // Draw current state
        menu_draw();
        game_logic_draw();
        game_mode_draw();
        
        // Wait for next frame
        VIDEO_WaitVSync();
    }
}

static void cleanup(void) {
    // Clean up subsystems
    menu_cleanup();
    game_logic_cleanup();
    game_mode_cleanup();
    audio_input_cleanup(NULL);
    usb_cleanup_device(NULL);
    
    // Clean up system
    VIDEO_Flush();
    VIDEO_WaitVSync();
    VIDEO_Shutdown();
    USB_Deinitialize();
    ASND_Pause(1);
    ASND_End();
} 