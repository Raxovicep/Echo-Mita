#ifndef CALLBACK_H
#define CALLBACK_H

#include "miniaudio.h"

void data_callback(ma_device *pDevice, void *pOutput, const void *pInput ma_uint32 frameCount);
void playback_callback(ma_device *pDevice, void *pOutput, const void *pInput ma_uint32 frameCount);

#endif // CALLBACK_H
