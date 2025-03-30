#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "audio.h"

#define MAX_EFFECTS 16

int audio_init(audio_system_t *audio) {
    if (!audio) return -1;
    
    // Initialize input
    if (audio_input_init(&audio->input) != 0) {
        return -1;
    }
    
    // Initialize output
    if (audio_output_init(&audio->output) != 0) {
        audio_input_cleanup(&audio->input);
        return -1;
    }
    
    // Initialize effects array
    audio->effects = malloc(sizeof(effect_t) * MAX_EFFECTS);
    if (!audio->effects) {
        audio_output_cleanup(&audio->output);
        audio_input_cleanup(&audio->input);
        return -1;
    }
    
    audio->num_effects = 0;
    audio->initialized = true;
    
    return 0;
}

int audio_start(audio_system_t *audio) {
    if (!audio || !audio->initialized) return -1;
    
    // Start input
    if (audio_input_start(&audio->input) != 0) {
        return -1;
    }
    
    // Start output
    if (audio_output_start(&audio->output) != 0) {
        audio_input_stop(&audio->input);
        return -1;
    }
    
    return 0;
}

int audio_stop(audio_system_t *audio) {
    if (!audio || !audio->initialized) return -1;
    
    // Stop output
    audio_output_stop(&audio->output);
    
    // Stop input
    audio_input_stop(&audio->input);
    
    return 0;
}

int audio_process(audio_system_t *audio) {
    if (!audio || !audio->initialized) return -1;
    
    // Process input
    if (audio_input_process(&audio->input) != 0) {
        return -1;
    }
    
    // Get input buffer
    s16 *input_buffer = audio->input.buffer;
    u32 buffer_size = audio->input.buffer_size;
    
    // Process effects chain
    s16 *current_buffer = input_buffer;
    s16 *temp_buffer = malloc(buffer_size);
    if (!temp_buffer) return -1;
    
    for (u32 i = 0; i < audio->num_effects; i++) {
        if (effect_process(&audio->effects[i], current_buffer, temp_buffer, buffer_size) != 0) {
            free(temp_buffer);
            return -1;
        }
        
        // Swap buffers
        s16 *swap = current_buffer;
        current_buffer = temp_buffer;
        temp_buffer = swap;
    }
    
    // Process output
    if (audio_output_process(&audio->output) != 0) {
        free(temp_buffer);
        return -1;
    }
    
    free(temp_buffer);
    return 0;
}

int audio_cleanup(audio_system_t *audio) {
    if (!audio) return -1;
    
    // Stop if running
    audio_stop(audio);
    
    // Cleanup effects
    for (u32 i = 0; i < audio->num_effects; i++) {
        effect_cleanup(&audio->effects[i]);
    }
    
    if (audio->effects) {
        free(audio->effects);
    }
    
    // Cleanup input and output
    audio_output_cleanup(&audio->output);
    audio_input_cleanup(&audio->input);
    
    // Reset structure
    memset(audio, 0, sizeof(audio_system_t));
    
    return 0;
}

// Effect management
int audio_add_effect(audio_system_t *audio, effect_type_t type, effect_params_t *params) {
    if (!audio || !audio->initialized || audio->num_effects >= MAX_EFFECTS) return -1;
    
    // Initialize new effect
    if (effect_init(&audio->effects[audio->num_effects], type, params) != 0) {
        return -1;
    }
    
    audio->num_effects++;
    return 0;
}

int audio_remove_effect(audio_system_t *audio, u32 index) {
    if (!audio || !audio->initialized || index >= audio->num_effects) return -1;
    
    // Cleanup effect
    effect_cleanup(&audio->effects[index]);
    
    // Shift remaining effects
    for (u32 i = index; i < audio->num_effects - 1; i++) {
        memcpy(&audio->effects[i], &audio->effects[i + 1], sizeof(effect_t));
    }
    
    audio->num_effects--;
    return 0;
}

int audio_set_effect_enabled(audio_system_t *audio, u32 index, bool enabled) {
    if (!audio || !audio->initialized || index >= audio->num_effects) return -1;
    
    effect_set_enabled(&audio->effects[index], enabled);
    return 0;
}

int audio_set_effect_params(audio_system_t *audio, u32 index, effect_params_t *params) {
    if (!audio || !audio->initialized || index >= audio->num_effects || !params) return -1;
    
    effect_set_params(&audio->effects[index], params);
    return 0;
}

// Helper functions
bool audio_is_initialized(audio_system_t *audio) {
    if (!audio) return false;
    return audio->initialized;
}

bool audio_is_running(audio_system_t *audio) {
    if (!audio || !audio->initialized) return false;
    return audio_input_is_recording(&audio->input) && audio_output_is_playing(&audio->output);
}

u32 audio_get_num_effects(audio_system_t *audio) {
    if (!audio || !audio->initialized) return 0;
    return audio->num_effects;
}

effect_t *audio_get_effect(audio_system_t *audio, u32 index) {
    if (!audio || !audio->initialized || index >= audio->num_effects) return NULL;
    return &audio->effects[index];
} 