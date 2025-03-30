#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "effects.h"

#define MAX_BUFFER_SIZE (44100 * 2) // 2 seconds at 44.1kHz
#define PI 3.14159265358979323846f

// Effect state structures
typedef struct {
    float last_sample;
    float envelope;
} compressor_state_t;

typedef struct {
    s16 *delay_buffer;
    u32 delay_pos;
    u32 delay_size;
} delay_state_t;

typedef struct {
    float cutoff;
    float resonance;
    float b0, b1, b2;
    float a0, a1, a2;
} eq_state_t;

int effect_init(effect_t *effect, effect_type_t type, effect_params_t *params) {
    if (!effect) return -1;
    
    // Set effect type and parameters
    effect->type = type;
    if (params) {
        memcpy(&effect->params, params, sizeof(effect_params_t));
    } else {
        memset(&effect->params, 0, sizeof(effect_params_t));
    }
    
    effect->enabled = true;
    effect->buffer = NULL;
    effect->buffer_size = 0;
    effect->state = NULL;
    
    // Allocate buffer if needed
    if (type == EFFECT_DELAY) {
        effect->buffer_size = MAX_BUFFER_SIZE;
        effect->buffer = malloc(effect->buffer_size);
        if (!effect->buffer) return -1;
        
        // Initialize delay state
        delay_state_t *state = malloc(sizeof(delay_state_t));
        if (!state) {
            free(effect->buffer);
            return -1;
        }
        
        state->delay_buffer = effect->buffer;
        state->delay_pos = 0;
        state->delay_size = (u32)(effect->params.delay * 44100);
        effect->state = state;
    }
    
    // Initialize other effect states
    switch (type) {
        case EFFECT_COMPRESSOR:
            effect->state = malloc(sizeof(compressor_state_t));
            if (!effect->state) return -1;
            ((compressor_state_t *)effect->state)->last_sample = 0.0f;
            ((compressor_state_t *)effect->state)->envelope = 0.0f;
            break;
            
        case EFFECT_EQ:
            effect->state = malloc(sizeof(eq_state_t));
            if (!effect->state) return -1;
            // Initialize EQ coefficients
            // TODO: Calculate proper coefficients based on params
            break;
            
        default:
            break;
    }
    
    return 0;
}

int effect_process(effect_t *effect, s16 *input, s16 *output, u32 size) {
    if (!effect || !effect->enabled || !input || !output) return -1;
    
    switch (effect->type) {
        case EFFECT_DISTORTION:
            // Simple soft clipping
            for (u32 i = 0; i < size; i++) {
                float sample = input[i] / 32768.0f;
                sample = tanh(sample * effect->params.gain);
                output[i] = (s16)(sample * 32768.0f);
            }
            break;
            
        case EFFECT_DELAY: {
            delay_state_t *state = (delay_state_t *)effect->state;
            for (u32 i = 0; i < size; i++) {
                s16 delayed = state->delay_buffer[state->delay_pos];
                output[i] = (s16)(input[i] * (1.0f - effect->params.mix) + 
                                 delayed * effect->params.mix);
                state->delay_buffer[state->delay_pos] = (s16)(input[i] + 
                    delayed * effect->params.feedback);
                state->delay_pos = (state->delay_pos + 1) % state->delay_size;
            }
            break;
        }
        
        case EFFECT_COMPRESSOR: {
            compressor_state_t *state = (compressor_state_t *)effect->state;
            for (u32 i = 0; i < size; i++) {
                float sample = input[i] / 32768.0f;
                float envelope = fabs(sample);
                
                // Update envelope
                if (envelope > state->envelope) {
                    state->envelope = envelope + (envelope - state->envelope) * 
                        effect->params.attack;
                } else {
                    state->envelope = envelope + (state->envelope - envelope) * 
                        effect->params.release;
                }
                
                // Apply compression
                float gain = 1.0f;
                if (state->envelope > effect->params.threshold) {
                    gain = effect->params.threshold + 
                        (state->envelope - effect->params.threshold) / 
                        effect->params.ratio;
                }
                
                output[i] = (s16)(sample * gain * 32768.0f);
            }
            break;
        }
        
        case EFFECT_EQ: {
            eq_state_t *state = (eq_state_t *)effect->state;
            // TODO: Implement EQ processing
            memcpy(output, input, size * sizeof(s16));
            break;
        }
        
        default:
            memcpy(output, input, size * sizeof(s16));
            break;
    }
    
    return 0;
}

int effect_cleanup(effect_t *effect) {
    if (!effect) return -1;
    
    // Free buffer
    if (effect->buffer) {
        free(effect->buffer);
    }
    
    // Free state
    if (effect->state) {
        free(effect->state);
    }
    
    // Reset structure
    memset(effect, 0, sizeof(effect_t));
    
    return 0;
}

// Helper functions
void effect_set_enabled(effect_t *effect, bool enabled) {
    if (!effect) return;
    effect->enabled = enabled;
}

void effect_set_params(effect_t *effect, effect_params_t *params) {
    if (!effect || !params) return;
    memcpy(&effect->params, params, sizeof(effect_params_t));
}

bool effect_is_enabled(effect_t *effect) {
    if (!effect) return false;
    return effect->enabled;
}

effect_type_t effect_get_type(effect_t *effect) {
    if (!effect) return EFFECT_NONE;
    return effect->type;
} 