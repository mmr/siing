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
#include <ogc/usbstorage.h>

#include "audio/audio.h"
#include "game/modes.h"
#include "game/logic.h"
#include "ui/menu.h"
#include "usb/device.h"

#define DEFAULT_FIFO_SIZE (256*1024)

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
    if (!usb_device_init()) {
        printf("Failed to initialize USB subsystem\n");
    }
    
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

    // Initialize GX
    void *gp_fifo = memalign(32, DEFAULT_FIFO_SIZE);
    GX_Init(gp_fifo, DEFAULT_FIFO_SIZE);
    
    // Set up viewport
    GX_SetViewport(0, 0, rmode->fbWidth, rmode->efbHeight, 0, 1);
    GX_SetScissor(0, 0, rmode->fbWidth, rmode->efbHeight);
    
    // Set up projection matrix
    Mtx44 perspective;
    guPerspective(perspective, 45, (f32)rmode->fbWidth/(f32)rmode->efbHeight, 0.1F, 300.0F);
    GX_LoadProjectionMtx(perspective, GX_PERSPECTIVE);
    
    // Set up model view matrix
    Mtx modelview;
    guMtxIdentity(modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    // Set up GX state
    GX_SetCullMode(GX_CULL_NONE);
    GX_ClearVtxDesc();
    GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    GX_SetNumChans(1);
    GX_SetNumTexGens(1);
    GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
    GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_SET);
    GX_SetColorUpdate(GX_TRUE);
    GX_SetAlphaUpdate(GX_TRUE);
    GX_SetDstAlpha(GX_TRUE, 0xFF);
}

static void init_audio(void) {
    // Initialize audio system
    AUDIO_Init(NULL);
    AUDIO_SetDSPSampleRate(AI_SAMPLERATE_48KHZ);
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
        usb_device_scan();
        
        // Update game state
        game_mode_update(&game_state);
        
        // Begin frame
        GX_ClearVtxDesc();
        GX_InvVtxCache();
        
        // Draw frame
        VIDEO_WaitVSync();
        game_mode_draw(&game_state);
        
        // End frame
        GX_DrawDone();
        VIDEO_Flush();
        
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
    AUDIO_StopDMA();
    
    // Cleanup GX
    GX_AbortFrame();
    GX_Flush();
    
    // Cleanup video
    VIDEO_SetBlack(TRUE);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    
    // Cleanup USB
    usb_device_close();
} 