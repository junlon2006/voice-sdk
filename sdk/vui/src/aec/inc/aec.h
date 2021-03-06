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
 * Description : aec.h
 * Author      : junlon2006@163.com
 * Date        : 2020.06.13
 *
 **************************************************************************/
#ifndef VOICE_SDK_SDK_VUI_SRC_AEC_INC_AEC_H_
#define VOICE_SDK_SDK_VUI_SRC_AEC_INC_AEC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "pub.h"

typedef void* AecHandle;

/**
 * Create Aec handle.
 *
 * @param[in]  event_router   event send API.
 *
 * @return  NULL: fail, otherwise: audioin handle.
 */
AecHandle AecCreate(CbEventRouter event_router);

/**
 * Destrpy Aec handle.
 *
 * @param[in]  hndl  the Aec handle
 *
 * @return  none.
 */
void AecDestroy(AecHandle hndl);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* VOICE_SDK_SDK_VUI_SRC_AEC_INC_AEC_H_ */