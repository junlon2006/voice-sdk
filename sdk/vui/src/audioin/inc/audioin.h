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
 * Description : audioin.h
 * Author      : junlon2006@163.com
 * Date        : 2020.06.13
 *
 **************************************************************************/
#ifndef VOICE_SDK_SDK_VUI_SRC_AUDIOIN_INC_AUDIOIN_H_
#define VOICE_SDK_SDK_VUI_SRC_AUDIOIN_INC_AUDIOIN_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef void* AudioInHandle;

/**
 * Create audioin handle.
 *
 * @param[in]  void.
 *
 * @return  NULL: fail, otherwise: audioin handle.
 */
AudioInHandle AudioInCreate(void);

/**
 * Destroy audioin handle.
 *
 * @param[in]  hndl audioin handle.
 *
 * @return  none.
 */
void AudioInDestroy(AudioInHandle hndl);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* VOICE_SDK_SDK_VUI_SRC_AUDIOIN_INC_AUDIOIN_H_ */