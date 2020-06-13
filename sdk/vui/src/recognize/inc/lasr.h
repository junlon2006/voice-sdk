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
 * Description : lasr.h
 * Author      : junlon2006@163.com
 * Date        : 2020.06.13
 *
 **************************************************************************/
#ifndef VOICE_SDK_SDK_VUI_SRC_RECOGNIZE_INC_LASR_H_
#define VOICE_SDK_SDK_VUI_SRC_RECOGNIZE_INC_LASR_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef void* LasrHandle;

/**
 * Create Lasr handle.
 *
 * @param[in]  void.
 *
 * @return  NULL: fail, otherwise: audioin handle.
 */
LasrHandle LasrCreate();

/**
 * Destrpy Lasr handle.
 *
 * @param[in]  hndl  the Lasr handle
 *
 * @return  none.
 */
void LasrDestroy(LasrHandle hndl);

/**
 * Lasr start.
 *
 * @param[in]  hndl  the Lasr handle
 * @param[in]  mode  the Lasr, 0 awaken mode, 1 cmd recognize mode
 *
 * @return  0: success, otherwise: fail.
 */
int LasrStart(LasrHandle hndl, int mode);

/**
 * Lasr stop.
 *
 * @param[in]  hndl  the Lasr handle
 *
 * @return  0: success, otherwise: fail.
 */
int LasrStop(LasrHandle hndl);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* VOICE_SDK_SDK_VUI_SRC_RECOGNIZE_INC_LASR_H_ */