#ifndef AUDIO_INPUT_H
#define AUDIO_INPUT_H

#include <gccore.h>

// Audio input types
typedef enum {
    AUDIO_INPUT_NONE = 0,
    AUDIO_INPUT_MICROPHONE,
    AUDIO_INPUT_GUITAR,
    AUDIO_INPUT_DRUMS
} audio_input_type_t;

// Audio input structure
typedef struct {
    audio_input_type_t type;
    u32 sample_rate;
    u8 channels;
    u32 buffer_size;
    s16 *buffer;
    bool is_recording;
    void (*callback)(s16 *data, u32 size, void *user_data);
    void *user_data;
} audio_input_t;

// Function declarations
int audio_input_init(audio_input_t *input);
int audio_input_start(audio_input_t *input);
int audio_input_stop(audio_input_t *input);
int audio_input_process(audio_input_t *input);
int audio_input_cleanup(audio_input_t *input);

// Helper functions
void audio_input_set_callback(audio_input_t *input, void (*callback)(s16 *data, u32 size, void *user_data), void *user_data);
bool audio_input_is_recording(audio_input_t *input);
audio_input_type_t audio_input_get_type(audio_input_t *input);

#endif // AUDIO_INPUT_H 