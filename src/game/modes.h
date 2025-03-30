#ifndef GAME_MODES_H
#define GAME_MODES_H

#include <gccore.h>

// Game mode types
typedef enum {
    MODE_NONE = 0,
    MODE_MENU,
    MODE_GAMEPLAY,
    MODE_FREE_PLAY,
    MODE_PRACTICE,
    MODE_PERFORMANCE
} game_mode_t;

// Game state structure
typedef struct {
    // Mode state
    game_mode_t current_mode;
    bool is_paused;
    bool is_running;
    
    // Scoring and progression
    int score;
    int level;
    int next_level_score;
    float points_per_frame;
    
    // Mode-specific data
    void *mode_data;
} game_state_t;

// Function declarations
int game_mode_init(game_state_t *state);
int game_mode_set(game_state_t *state, game_mode_t mode);
int game_mode_update(game_state_t *state);
int game_mode_draw(game_state_t *state);
int game_mode_cleanup(game_state_t *state);

// Helper functions
bool game_mode_is_running(game_state_t *state);
bool game_mode_is_paused(game_state_t *state);
game_mode_t game_mode_get_current(game_state_t *state);

#endif // GAME_MODES_H 