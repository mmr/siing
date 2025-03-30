#ifndef AUDIO_H
#define AUDIO_H

#include <gccore.h>
#include "input.h"
#include "output.h"
#include "effects.h"

// Audio system structure
typedef struct {
    audio_input_t input;
    audio_output_t output;
    effect_t *effects;
    u32 num_effects;
    bool initialized;
} audio_system_t;

// Function declarations
int audio_init(audio_system_t *audio);
int audio_start(audio_system_t *audio);
int audio_stop(audio_system_t *audio);
int audio_process(audio_system_t *audio);
int audio_cleanup(audio_system_t *audio);

// Effect management
int audio_add_effect(audio_system_t *audio, effect_type_t type, effect_params_t *params);
int audio_remove_effect(audio_system_t *audio, u32 index);
int audio_set_effect_enabled(audio_system_t *audio, u32 index, bool enabled);
int audio_set_effect_params(audio_system_t *audio, u32 index, effect_params_t *params);

// Helper functions
bool audio_is_initialized(audio_system_t *audio);
bool audio_is_running(audio_system_t *audio);
u32 audio_get_num_effects(audio_system_t *audio);
effect_t *audio_get_effect(audio_system_t *audio, u32 index);

#endif // AUDIO_H 