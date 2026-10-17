#ifndef AUDIO_CAPTURE
#define AUDIO_CAPTURE

#include <miniaudio/miniaudio.h>

// Class AudioCapture
typedef struct {
  ma_device device;
  int status;
} AudioCapture;

// Create object
AudioCapture *create_capture(ma_format format, ma_uint32 channles,
                             ma_uint32 sample_rate,
                             ma_device_data_proc data_callback,
                             void *user_data);

// Methods
void start_capture(AudioCapture *ctx);
void stop_capture(AudioCapture *ctx);
void delete_capture(AudioCapture *ctx);
#endif // AUDIO_CAPTURE
