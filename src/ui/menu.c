#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ogc/gu.h>
#include <ogc/gx.h>
#include <ogc/video.h>
#include <ogc/console.h>
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

// GX drawing functions
static void draw_rect(int x, int y, int w, int h, u32 color) {
    GX_SetTevOp(GX_TEVSTAGE0, GX_REPLACE);
    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
    GX_Position2s16(x, y);
    GX_Color4u8((color >> 24) & 0xFF, (color >> 16) & 0xFF, 
                 (color >> 8) & 0xFF, color & 0xFF);
    GX_Position2s16(x + w, y);
    GX_Color4u8((color >> 24) & 0xFF, (color >> 16) & 0xFF, 
                 (color >> 8) & 0xFF, color & 0xFF);
    GX_Position2s16(x + w, y + h);
    GX_Color4u8((color >> 24) & 0xFF, (color >> 16) & 0xFF, 
                 (color >> 8) & 0xFF, color & 0xFF);
    GX_Position2s16(x, y + h);
    GX_Color4u8((color >> 24) & 0xFF, (color >> 16) & 0xFF, 
                 (color >> 8) & 0xFF, color & 0xFF);
    GX_End();
}

static void draw_text(const char *text, int x, int y, u32 color) {
    // TODO: Implement text drawing using GX
    // For now, use console for text output
    printf("\x1b[%d;%dH%s", y, x, text);
}

void menu_init(void) {
    memset(&current_menu, 0, sizeof(menu_t));
    current_menu.is_active = true;
    current_menu.selected_item = 0;
    current_menu.visible_start = 0;
    current_menu.visible_count = 8; // Show 8 items at a time
}

void menu_add_item(const char *text, menu_callback_t callback) {
    if (current_menu.item_count >= MAX_MENU_ITEMS) return;
    
    menu_item_t *item = &current_menu.items[current_menu.item_count];
    item->text = strdup(text);
    item->callback = callback;
    item->is_selected = (current_menu.item_count == 0);
    item->is_enabled = true;
    
    current_menu.item_count++;
}

void menu_set_title(const char *title) {
    if (current_menu.title) {
        free(current_menu.title);
    }
    current_menu.title = strdup(title);
}

void menu_draw(void) {
    if (!current_menu.is_active) return;
    
    // Get screen dimensions
    u16 width = VIDEO_GetFramebufferWidth();
    u16 height = VIDEO_GetFramebufferHeight();
    
    // Calculate menu dimensions
    int menu_width = 400;
    int menu_height = (current_menu.item_count * MENU_ITEM_HEIGHT) + (MENU_PADDING * 2);
    int menu_x = (width - menu_width) / 2;
    int menu_y = (height - menu_height) / 2;
    
    // Draw menu background
    draw_rect(menu_x, menu_y, menu_width, menu_height, menu_colors.background);
    
    // Draw title
    if (current_menu.title) {
        int title_x = menu_x + (menu_width - strlen(current_menu.title) * MENU_FONT_SIZE) / 2;
        draw_text(current_menu.title, title_x, menu_y + MENU_PADDING, menu_colors.title);
    }
    
    // Draw menu items
    for (int i = 0; i < current_menu.item_count; i++) {
        menu_item_t *item = &current_menu.items[i];
        int item_y = menu_y + MENU_PADDING + (i * MENU_ITEM_HEIGHT);
        
        // Draw selection highlight
        if (item->is_selected) {
            draw_rect(menu_x + 10, item_y, menu_width - 20, MENU_ITEM_HEIGHT - 5, menu_colors.selected);
        }
        
        // Draw item text
        u32 text_color = item->is_enabled ? menu_colors.text : menu_colors.disabled;
        int text_x = menu_x + (menu_width - strlen(item->text) * MENU_FONT_SIZE) / 2;
        draw_text(item->text, text_x, item_y + (MENU_ITEM_HEIGHT - MENU_FONT_SIZE) / 2, text_color);
    }
}

void menu_handle_input(void) {
    if (!current_menu.is_active) return;
    
    // Handle Wii Remote input
    u32 pressed = PAD_ButtonsDown(0);
    
    if (pressed & PAD_BUTTON_UP) {
        // Move selection up
        current_menu.items[current_menu.selected_item].is_selected = false;
        current_menu.selected_item--;
        if (current_menu.selected_item < 0) {
            current_menu.selected_item = current_menu.item_count - 1;
        }
        current_menu.items[current_menu.selected_item].is_selected = true;
    }
    else if (pressed & PAD_BUTTON_DOWN) {
        // Move selection down
        current_menu.items[current_menu.selected_item].is_selected = false;
        current_menu.selected_item++;
        if (current_menu.selected_item >= current_menu.item_count) {
            current_menu.selected_item = 0;
        }
        current_menu.items[current_menu.selected_item].is_selected = true;
    }
    else if (pressed & PAD_BUTTON_A) {
        // Select item
        menu_item_t *item = &current_menu.items[current_menu.selected_item];
        if (item->is_enabled && item->callback) {
            item->callback();
        }
    }
}

void menu_cleanup(void) {
    // Free title
    if (current_menu.title) {
        free(current_menu.title);
    }
    
    // Free menu items
    for (int i = 0; i < current_menu.item_count; i++) {
        free(current_menu.items[i].text);
    }
    
    memset(&current_menu, 0, sizeof(menu_t));
}

// Helper functions
void menu_set_colors(const menu_colors_t *colors) {
    memcpy(&menu_colors, colors, sizeof(menu_colors_t));
}

void menu_set_active(bool active) {
    current_menu.is_active = active;
}

bool menu_is_active(void) {
    return current_menu.is_active;
}

int menu_get_selected_item(void) {
    return current_menu.selected_item;
}

void menu_set_selected_item(int index) {
    if (index < 0 || index >= current_menu.item_count) return;
    
    current_menu.items[current_menu.selected_item].is_selected = false;
    current_menu.selected_item = index;
    current_menu.items[current_menu.selected_item].is_selected = true;
}

void menu_enable_item(int index, bool enable) {
    if (index < 0 || index >= current_menu.item_count) return;
    current_menu.items[index].is_enabled = enable;
} 