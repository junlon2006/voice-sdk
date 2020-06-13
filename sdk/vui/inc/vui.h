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
 * Description : vui.h
 * Author      : junlon2006@163.com
 * Date        : 2020.06.11
 *
 **************************************************************************/
#ifndef VOICE_SDK_SDK_VUI_INC_VUI_H_
#define VOICE_SDK_SDK_VUI_INC_VUI_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    UNI_AWAKEN_MODE = 0,
    UNI_LASR_ONLY_MODE,
    UNI_RASR_ONLY_MODE,
    UNI_LASR_RASR_MODE,
} VuiMode;

typedef void* VuiHandle;

/**
 * Create vui handle.
 *
 * @param[in]  void.
 *
 * @return  NULL: fail, otherwise: vui handle.
 */
VuiHandle VuiCeate(void);

/**
 * Destrpy vui handle.
 *
 * @param[in]  hndl  the vui handle
 *
 * @return  none.
 */
void VuiDestroy(VuiHandle hndl);

/**
 * vui start.
 *
 * @param[in]  hndl  the vui handle
 * @param[in]  mode  the vui mode
 *
 * @return  0: success, otherwise: fail.
 */
int VuiStart(VuiHandle hndl, VuiMode mode);

/**
 * vui stop.
 *
 * @param[in]  hndl  the vui handle
 *
 * @return  0: success, otherwise: fail.
 */
int VuiStop(VuiHandle hndl);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* VOICE_SDK_SDK_VUI_INC_VUI_H_ */