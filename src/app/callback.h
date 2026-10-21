#ifndef CALLBACK_H
#define CALLBACK_H

#include "miniaudio.h"

void recog_call(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount);
void playback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount);

#endif // CALLBACK_H
