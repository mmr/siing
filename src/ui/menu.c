#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ogc/gu.h>
#include <ogc/gx.h>
#include <ogc/video.h>
#include <ogc/consol.h>
#include <wiiuse/wpad.h>
#include "menu.h"

// Menu state
static menu_t current_menu;
static menu_colors_t menu_colors = {
    .background = 0x000000FF,  // Black
    .text = 0xFFFFFFFF,        // White
    .selected = 0x00FF00FF,    // Green
    .disabled = 0x808080FF,    // Gray
    .title = 0xFFFF00FF        // Yellow
};

// Font texture
static GXTexObj font_texture;
static u8 font_data[256 * 16];  // Simple 8x16 font data

// Helper functions
static void draw_rect(int x, int y, int w, int h, u32 color) {
    GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
    GX_Position2s16(x, y);
    GX_Color4u8((color >> 24) & 0xFF, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
    GX_Position2s16(x + w, y);
    GX_Color4u8((color >> 24) & 0xFF, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
    GX_Position2s16(x + w, y + h);
    GX_Color4u8((color >> 24) & 0xFF, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
    GX_Position2s16(x, y + h);
    GX_Color4u8((color >> 24) & 0xFF, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
    GX_End();
}

static void draw_text(const char *text, int x, int y, u32 color) {
    // Convert color to RGBA components
    u8 r = (color >> 24) & 0xFF;
    u8 g = (color >> 16) & 0xFF;
    u8 b = (color >> 8) & 0xFF;
    u8 a = color & 0xFF;
    
    // Set up GX for text rendering
    GX_LoadTexObj(&font_texture, GX_TEXMAP0);
    GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    
    // Draw each character
    for (int i = 0; text[i]; i++) {
        char c = text[i];
        int char_x = x + (i * 8);
        
        // Draw character quad
        GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position2s16(char_x, y);
        GX_Color4u8(r, g, b, a);
        GX_TexCoord2f32((c * 8.0f) / 256.0f, 0.0f);
        
        GX_Position2s16(char_x + 8, y);
        GX_Color4u8(r, g, b, a);
        GX_TexCoord2f32(((c + 1) * 8.0f) / 256.0f, 0.0f);
        
        GX_Position2s16(char_x + 8, y + 16);
        GX_Color4u8(r, g, b, a);
        GX_TexCoord2f32(((c + 1) * 8.0f) / 256.0f, 1.0f);
        
        GX_Position2s16(char_x, y + 16);
        GX_Color4u8(r, g, b, a);
        GX_TexCoord2f32((c * 8.0f) / 256.0f, 1.0f);
        GX_End();
    }
}

void menu_init(void) {
    memset(&current_menu, 0, sizeof(current_menu));
    current_menu.active = true;
    current_menu.selected_item = 0;
    
    // Initialize GX for text rendering
    GX_ClearVtxDesc();
    GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XY, GX_S16, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    
    GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    GX_SetAlphaUpdate(GX_TRUE);
    GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    
    // Initialize font texture
    memset(font_data, 0, sizeof(font_data));  // Clear font data
    GX_InitTexObj(&font_texture, font_data, 256, 16, GX_TF_I4, GX_CLAMP, GX_CLAMP, GX_FALSE);
    GX_LoadTexObj(&font_texture, GX_TEXMAP0);
}

void menu_add_item(const char *text, menu_callback_t callback) {
    if (current_menu.item_count >= MAX_MENU_ITEMS) return;
    
    menu_item_t *item = &current_menu.items[current_menu.item_count];
    strncpy(item->text, text, MAX_MENU_TEXT_LENGTH - 1);
    item->callback = callback;
    item->enabled = true;
    current_menu.item_count++;
}

void menu_set_title(const char *title) {
    strncpy(current_menu.title, title, MAX_MENU_TITLE_LENGTH - 1);
}

void menu_draw(void) {
    if (!current_menu.active) return;
    
    // Draw background
    draw_rect(0, 0, 640, 480, menu_colors.background);
    
    // Draw title
    if (current_menu.title[0]) {
        int title_x = (640 - strlen(current_menu.title) * 8) / 2;
        draw_text(current_menu.title, title_x, 50, menu_colors.title);
    }
    
    // Draw items
    for (int i = 0; i < current_menu.item_count; i++) {
        menu_item_t *item = &current_menu.items[i];
        int y = 150 + i * MENU_ITEM_HEIGHT;
        u32 color = item->enabled ? 
            (i == current_menu.selected_item ? menu_colors.selected : menu_colors.text) :
            menu_colors.disabled;
        
        int item_x = (640 - strlen(item->text) * 8) / 2;
        draw_text(item->text, item_x, y, color);
    }
}

void menu_handle_input(void) {
    if (!current_menu.active) return;
    
    // Handle Wii Remote input
    u32 buttons = WPAD_ButtonsDown(0);
    
    if (buttons & WPAD_BUTTON_UP) {
        current_menu.selected_item--;
        if (current_menu.selected_item < 0) {
            current_menu.selected_item = current_menu.item_count - 1;
        }
    }
    else if (buttons & WPAD_BUTTON_DOWN) {
        current_menu.selected_item++;
        if (current_menu.selected_item >= current_menu.item_count) {
            current_menu.selected_item = 0;
        }
    }
    else if (buttons & WPAD_BUTTON_A) {
        menu_item_t *item = &current_menu.items[current_menu.selected_item];
        if (item->enabled && item->callback) {
            item->callback();
        }
    }
}

void menu_cleanup(void) {
    memset(&current_menu, 0, sizeof(current_menu));
}

void menu_set_colors(const menu_colors_t *colors) {
    if (colors) {
        menu_colors = *colors;
    }
}

void menu_set_active(bool active) {
    current_menu.active = active;
}

bool menu_is_active(void) {
    return current_menu.active;
}

int menu_get_selected_item(void) {
    return current_menu.selected_item;
}

void menu_set_selected_item(int index) {
    if (index >= 0 && index < current_menu.item_count) {
        current_menu.selected_item = index;
    }
}

void menu_enable_item(int index, bool enable) {
    if (index >= 0 && index < current_menu.item_count) {
        current_menu.items[index].enabled = enable;
    }
} 