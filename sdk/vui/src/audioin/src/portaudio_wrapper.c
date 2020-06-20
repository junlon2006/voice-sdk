/**************************************************************************
 * Copyright (C) 2020-2020  Junlon2006
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **************************************************************************
 *
 * Description : portaudio_wrapper.c
 * Author      : junlon2006@163.com
 * Date        : 2020.06.13
 *
 **************************************************************************/
#include "audio_wrapper.h"
#include "pub.h"
#include "portaudio.h"

#define TAG                      "portaudio"
#define NUM_INPUT_CHANNELS       (1)
#define NUM_OUTPUT_CHANNELS      (0)
#define SAMPLE_RATE              (16000)

typedef struct {
    PaStream*          pa_stream;
    CbAudioWrapperData cb_data;
    void*              user_data;
} AudioWrapper;

static int __portaudio_callback(const void* inputBuffer,
                                void* outputBuffer,
                                unsigned long numSamples,
                                const PaStreamCallbackTimeInfo* timeInfo,
                                PaStreamCallbackFlags statusFlags,
                                void* userData) {
    AudioWrapper *audio_wrapper = (AudioWrapper *)userData;
    if (audio_wrapper->cb_data) {
        audio_wrapper->cb_data(inputBuffer, numSamples << 1, audio_wrapper->user_data);
    }

    LOGD(TAG, "portaudio data. buf=%p, len=%d", inputBuffer, numSamples << 1);
}

AudioWrapperHandle AudioWrapperCreate(CbAudioWrapperData cb_data, void *user_data) {
    AudioWrapper* audio_wrapper = (AudioWrapper *)malloc(sizeof(AudioWrapper));
    assert(audio_wrapper != NULL);

    audio_wrapper->cb_data   = cb_data;
    audio_wrapper->user_data = user_data;

    PaError err;
    err = Pa_Initialize();
    if (err != paNoError) {
        LOGE(TAG, "Failed to initialize PortAudio. err=%s", Pa_GetErrorText(err));
        free(audio_wrapper);
        return NULL;
    }

    PaStreamParameters inputParameters;
    memset(&inputParameters, 0, sizeof(inputParameters));
    inputParameters.device                    = Pa_GetDefaultInputDevice();
    inputParameters.channelCount              = NUM_INPUT_CHANNELS;
    inputParameters.sampleFormat              = paInt16;
    inputParameters.suggestedLatency          = 0.0160;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(&audio_wrapper->pa_stream,
                        &inputParameters,
                        NULL,
                        SAMPLE_RATE,
                        paFramesPerBufferUnspecified,
                        paNoFlag,
                        __portaudio_callback,
                        audio_wrapper);
    if (err != paNoError) {
        LOGE(TAG, "Failed to open PortAudio default stream. err=%s", Pa_GetErrorText(err));
        free(audio_wrapper);
        return NULL;
    }

    return audio_wrapper;
}

void AudioWrapperDestroy(AudioWrapperHandle hndl) {
    AudioWrapper *audio_wrapper = (AudioWrapper *)hndl;
    Pa_StopStream(audio_wrapper->pa_stream);
    Pa_CloseStream(audio_wrapper->pa_stream);
    Pa_Terminate();

    free(audio_wrapper);
}

int AudioWrapperStart(AudioWrapperHandle hndl) {
    AudioWrapper *audio_wrapper = (AudioWrapper *)hndl;
    PaError err = Pa_StartStream(audio_wrapper->pa_stream);
    if (err != paNoError) {
        LOGE(TAG, "Failed to start PortAudio stream. err=%s", Pa_GetErrorText(err));
        return FAIL;
    }

    return OK;
}

int AudioWrapperStop(AudioWrapperHandle hndl) {
    AudioWrapper *audio_wrapper = (AudioWrapper *)hndl;
    PaError err = Pa_StopStream(audio_wrapper->pa_stream);
    if (err != paNoError) {
        LOGE(TAG, "Failed to stop PortAudio stream. err=%s", Pa_GetErrorText(err));
        return FAIL;
    }

    return OK;
}