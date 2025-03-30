#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ogc/audio.h>
#include "output.h"

#define DEFAULT_SAMPLE_RATE 44100
#define DEFAULT_CHANNELS 2
#define DEFAULT_BUFFER_SIZE (DEFAULT_SAMPLE_RATE * DEFAULT_CHANNELS * sizeof(s16))

int audio_output_init(audio_output_t *output) {
    if (!output) return -1;
    
    // Set default values
    output->sample_rate = DEFAULT_SAMPLE_RATE;
    output->channels = DEFAULT_CHANNELS;
    output->buffer_size = DEFAULT_BUFFER_SIZE;
    output->is_playing = false;
    
    // Allocate buffer
    output->buffer = malloc(output->buffer_size);
    if (!output->buffer) return -1;
    
    // Initialize audio system
    AUDIO_Init(NULL);
    AUDIO_SetDSPSampleRate(AI_SAMPLERATE_48KHZ);
    
    return 0;
}

int audio_output_start(audio_output_t *output) {
    if (!output || output->is_playing) return -1;
    
    // Start audio output
    output->is_playing = true;
    
    return 0;
}

int audio_output_stop(audio_output_t *output) {
    if (!output || !output->is_playing) return -1;
    
    // Stop audio output
    output->is_playing = false;
    AUDIO_StopDMA();
    
    return 0;
}

int audio_output_process(audio_output_t *output) {
    if (!output || !output->is_playing) return -1;
    
    // Process audio output
    // TODO: Implement audio processing
    
    return 0;
}

int audio_output_cleanup(audio_output_t *output) {
    if (!output) return -1;
    
    // Stop output if playing
    audio_output_stop(output);
    
    // Free buffer
    if (output->buffer) {
        free(output->buffer);
    }
    
    // Reset structure
    memset(output, 0, sizeof(audio_output_t));
    
    return 0;
}

// Helper functions
void audio_output_set_volume(audio_output_t *output, u8 volume) {
    if (!output) return;
    // TODO: Implement volume control using AUDIO_SetVolume
}

void audio_output_set_pan(audio_output_t *output, s8 pan) {
    if (!output) return;
    // TODO: Implement pan control using AUDIO_SetPan
}

bool audio_output_is_playing(audio_output_t *output) {
    if (!output) return false;
    return output->is_playing;
} 