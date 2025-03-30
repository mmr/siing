#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "modes.h"

// Game mode structure
static game_mode_t current_mode;
static game_state_t game_state;

int game_mode_init(game_state_t *state) {
    if (!state) return -1;
    
    memset(state, 0, sizeof(game_state_t));
    state->current_mode = MODE_NONE;
    state->is_running = true;
    state->is_paused = false;
    state->score = 0;
    state->level = 1;
    state->next_level_score = 1000;
    state->points_per_frame = 1.0f;
    
    return 0;
}

int game_mode_set(game_state_t *state, game_mode_t mode) {
    if (!state) return -1;
    
    // Clean up previous mode if needed
    if (state->mode_data) {
        free(state->mode_data);
        state->mode_data = NULL;
    }
    
    state->current_mode = mode;
    
    switch (mode) {
        case MODE_FREE_PLAY:
            // Initialize free play mode
            break;
        case MODE_PRACTICE:
            // Initialize practice mode
            break;
        case MODE_MENU:
            // Menu mode doesn't need additional data
            break;
        default:
            return -1;
    }
    
    return 0;
}

int game_mode_update(game_state_t *state) {
    if (!state || !state->is_running) return -1;
    
    switch (state->current_mode) {
        case MODE_FREE_PLAY:
            // Update free play mode
            break;
        case MODE_PRACTICE:
            // Update practice mode
            break;
        case MODE_MENU:
            // Menu updates are handled by the menu system
            break;
        default:
            return -1;
    }
    
    return 0;
}

int game_mode_draw(game_state_t *state) {
    if (!state) return -1;
    
    switch (state->current_mode) {
        case MODE_FREE_PLAY:
            // Draw free play mode
            break;
        case MODE_PRACTICE:
            // Draw practice mode
            break;
        case MODE_MENU:
            // Menu drawing is handled by the menu system
            break;
        default:
            return -1;
    }
    
    return 0;
}

int game_mode_cleanup(game_state_t *state) {
    if (!state) return -1;
    
    // Clean up mode-specific data
    if (state->mode_data) {
        free(state->mode_data);
        state->mode_data = NULL;
    }
    
    // Reset state
    memset(state, 0, sizeof(game_state_t));
    
    return 0;
}

bool game_mode_is_running(game_state_t *state) {
    return state && state->is_running;
}

bool game_mode_is_paused(game_state_t *state) {
    return state && state->is_paused;
}

game_mode_t game_mode_get_current(game_state_t *state) {
    return state ? state->current_mode : MODE_NONE;
} 