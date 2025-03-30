#ifndef _AUDIO_OUTPUT_H_
#define _AUDIO_OUTPUT_H_

#include <gccore.h>
#include <ogc/audio.h>

// Audio output types
typedef enum {
    AUDIO_OUTPUT_NONE = 0,
    AUDIO_OUTPUT_SYSTEM,
    AUDIO_OUTPUT_EFFECTS
} audio_output_type_t;

// Audio output structure
typedef struct {
    audio_output_type_t type;
    u32 sample_rate;
    u32 channels;
    u32 buffer_size;
    void *buffer;
    bool is_playing;
} audio_output_t;

// Function declarations
int audio_output_init(audio_output_t *output);
int audio_output_start(audio_output_t *output);
int audio_output_stop(audio_output_t *output);
int audio_output_process(audio_output_t *output);
int audio_output_cleanup(audio_output_t *output);

// Helper functions
void audio_output_set_volume(audio_output_t *output, u8 volume);
void audio_output_set_pan(audio_output_t *output, s8 pan);
bool audio_output_is_playing(audio_output_t *output);

#endif /* _AUDIO_OUTPUT_H_ */ 