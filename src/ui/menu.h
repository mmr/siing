#ifndef _MENU_H_
#define _MENU_H_

#include <gccore.h>
#include <wiiuse/wpad.h>

#define MAX_MENU_ITEMS 10
#define MAX_MENU_TEXT_LENGTH 64
#define MAX_MENU_TITLE_LENGTH 128
#define MENU_ITEM_HEIGHT 30
#define MENU_PADDING 20
#define MENU_FONT_SIZE 24

// Menu callback function type
typedef void (*menu_callback_t)(void);

// Menu item structure
typedef struct {
    char text[MAX_MENU_TEXT_LENGTH];
    menu_callback_t callback;
    bool enabled;
} menu_item_t;

// Menu structure
typedef struct {
    char title[MAX_MENU_TITLE_LENGTH];
    menu_item_t items[MAX_MENU_ITEMS];
    int item_count;
    int selected_item;
    bool active;
} menu_t;

// Menu colors
typedef struct {
    u32 background;
    u32 text;
    u32 selected;
    u32 disabled;
    u32 title;
} menu_colors_t;

// Function declarations
void menu_init(void);
void menu_add_item(const char *text, menu_callback_t callback);
void menu_set_title(const char *title);
void menu_draw(void);
void menu_handle_input(void);
void menu_cleanup(void);

// Helper functions
void menu_set_colors(const menu_colors_t *colors);
void menu_set_active(bool active);
bool menu_is_active(void);
int menu_get_selected_item(void);
void menu_set_selected_item(int index);
void menu_enable_item(int index, bool enable);

#endif /* _MENU_H_ */ 