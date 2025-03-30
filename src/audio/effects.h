#ifndef AUDIO_EFFECTS_H
#define AUDIO_EFFECTS_H

#include <gccore.h>

// Effect types
typedef enum {
    EFFECT_NONE = 0,
    EFFECT_DISTORTION,
    EFFECT_DELAY,
    EFFECT_REVERB,
    EFFECT_COMPRESSOR,
    EFFECT_EQ
} effect_type_t;

// Effect parameters
typedef struct {
    float gain;
    float threshold;
    float ratio;
    float attack;
    float release;
    float mix;
    float feedback;
    float delay;
    float cutoff;
    float resonance;
} effect_params_t;

// Effect structure
typedef struct {
    effect_type_t type;
    effect_params_t params;
    bool enabled;
    s16 *buffer;
    u32 buffer_size;
    void *state;
} effect_t;

// Function declarations
int effect_init(effect_t *effect, effect_type_t type, effect_params_t *params);
int effect_process(effect_t *effect, s16 *input, s16 *output, u32 size);
int effect_cleanup(effect_t *effect);

// Helper functions
void effect_set_enabled(effect_t *effect, bool enabled);
void effect_set_params(effect_t *effect, effect_params_t *params);
bool effect_is_enabled(effect_t *effect);
effect_type_t effect_get_type(effect_t *effect);

#endif // AUDIO_EFFECTS_H 