#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <gccore.h>
#include "modes.h"

// Game logic structure
typedef struct {
    bool initialized;
    void *data;
} game_logic_t;

// Function declarations
int game_logic_init(game_state_t *state);
int game_logic_update(game_state_t *state);
int game_logic_handle_input(game_state_t *state);
int game_logic_draw(game_state_t *state);
int game_logic_cleanup(game_state_t *state);

// Helper functions
bool game_logic_is_initialized(game_logic_t *logic);

#endif // GAME_LOGIC_H 