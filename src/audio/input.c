#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ogc/audio.h>
#include "input.h"

#define DEFAULT_SAMPLE_RATE 44100
#define DEFAULT_CHANNELS 1
#define DEFAULT_BUFFER_SIZE (DEFAULT_SAMPLE_RATE * DEFAULT_CHANNELS * sizeof(s16))

int audio_input_init(audio_input_t *input) {
    if (!input) return -1;
    
    // Set default values
    input->sample_rate = DEFAULT_SAMPLE_RATE;
    input->channels = DEFAULT_CHANNELS;
    input->buffer_size = DEFAULT_BUFFER_SIZE;
    input->is_recording = false;
    input->callback = NULL;
    input->user_data = NULL;
    
    // Allocate buffer
    input->buffer = malloc(input->buffer_size);
    if (!input->buffer) return -1;
    
    return 0;
}

int audio_input_start(audio_input_t *input) {
    if (!input || input->is_recording) return -1;
    
    // Start audio input
    input->is_recording = true;
    
    return 0;
}

int audio_input_stop(audio_input_t *input) {
    if (!input || !input->is_recording) return -1;
    
    // Stop audio input
    input->is_recording = false;
    
    return 0;
}

int audio_input_process(audio_input_t *input) {
    if (!input || !input->is_recording) return -1;
    
    // Process audio input
    // TODO: Implement audio processing based on input type
    
    // Call callback if set
    if (input->callback) {
        input->callback(input->buffer, input->buffer_size, input->user_data);
    }
    
    return 0;
}

int audio_input_cleanup(audio_input_t *input) {
    if (!input) return -1;
    
    // Stop recording if active
    audio_input_stop(input);
    
    // Free buffer
    if (input->buffer) {
        free(input->buffer);
    }
    
    // Reset structure
    memset(input, 0, sizeof(audio_input_t));
    
    return 0;
}

// Helper functions
void audio_input_set_callback(audio_input_t *input, void (*callback)(s16 *data, u32 size, void *user_data), void *user_data) {
    if (!input) return;
    input->callback = callback;
    input->user_data = user_data;
}

bool audio_input_is_recording(audio_input_t *input) {
    if (!input) return false;
    return input->is_recording;
}

audio_input_type_t audio_input_get_type(audio_input_t *input) {
    if (!input) return AUDIO_INPUT_NONE;
    return input->type;
} 