#ifndef _LOGIC_H_
#define _LOGIC_H_

#include <gccore.h>

// Game state structure
typedef struct {
    int score;
    int level;
    float points_per_frame;
    int next_level_score;
    // Add more state variables as needed
} game_state_t;

// Game logic structure
typedef struct {
    game_state_t state;
    // Add more logic variables as needed
} game_logic_t;

// Function declarations
void game_logic_init(void);
void game_logic_update(void);
void game_logic_handle_input(void);
void game_logic_draw(void);
void game_logic_cleanup(void);

#endif /* _LOGIC_H_ */ 