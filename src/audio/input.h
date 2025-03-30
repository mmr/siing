#ifndef _AUDIO_INPUT_H_
#define _AUDIO_INPUT_H_

#include <gccore.h>
#include <ogc/audio.h>

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
    u32 channels;
    u32 buffer_size;
    void *buffer;
    void *callback_data;
} audio_input_t;

// Function declarations
int audio_input_init(audio_input_t *input);
int audio_input_start(audio_input_t *input);
int audio_input_stop(audio_input_t *input);
int audio_input_process(audio_input_t *input);
int audio_input_cleanup(audio_input_t *input);

// Callback function type
typedef void (*audio_input_callback_t)(void *data, void *buffer, u32 size);

#endif /* _AUDIO_INPUT_H_ */ 