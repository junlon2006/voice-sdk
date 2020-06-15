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
 * Description : audio_wrapper.h
 * Author      : junlon2006@163.com
 * Date        : 2020.06.13
 *
 **************************************************************************/
#ifndef VOICE_SDK_VUI_SRC_AUDIOIN_SRC_AUDIO_WRAPPER_H_
#define VOICE_SDK_VUI_SRC_AUDIOIN_SRC_AUDIO_WRAPPER_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef void* AudioWrapperHandle;
typedef void (*CbAudioWrapperData)(const char *data, unsigned int len, void *user_data);

/**
 * Create audio wrapper handle.
 *
 * @param[in]  cb_data   audio source data callback.
 * @param[in]  user_data   audio source data user_data.
 *
 * @return  NULL: fail, otherwise: audioin handle.
 */
AudioWrapperHandle AudioWrapperCreate(CbAudioWrapperData cb_data, void *user_data);

/**
 * Destroy audio wrapper handle.
 *
 * @param[in]  hndl audio wrapper handle.
 *
 * @return  none.
 */
void AudioWrapperDestroy(AudioWrapperHandle hndl);

/**
 * Start audio wrapper to capture audio.
 *
 * @param[in] hndl audio wrapper handle.
 *
 * @return  0: success, otherwise: fail.
 */
int AudioWrapperStart(AudioWrapperHandle hndl);

/**
 * Stop audio wrapper to capture audio.
 *
 * @param[in] hndl audio wrapper handle.
 *
 * @return  0: success, otherwise: fail.
 */
int AudioWrapperStop(AudioWrapperHandle hndl);

#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif  /* VOICE_SDK_VUI_SRC_AUDIOIN_SRC_AUDIO_WRAPPER_H_ */