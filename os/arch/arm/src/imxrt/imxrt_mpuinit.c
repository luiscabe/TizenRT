/****************************************************************************
 *
 * Copyright 2018 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/
/****************************************************************************
 * arch/arm/src/imxrt/imxrt_mpuinit.c
 *
 *   Copyright (C) 2018 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <tinyara/config.h>

#include <assert.h>

#include <tinyara/userspace.h>

#include "mpu.h"
#include "cache.h"
#include "chip/imxrt_memorymap.h"

#include "imxrt_mpuinit.h"

#ifdef CONFIG_ARMV7M_MPU

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#ifndef MAX
#define MAX(a,b) a > b ? a : b
#endif

#ifndef MIN
#define MIN(a,b) a < b ? a : b
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: imxrt_mpu_initialize
 *
 * Description:
 *   Configure the MPU to permit user-space access to only restricted i.MXRT
 *   resources.
 *
 ****************************************************************************/

void imxrt_mpu_initialize(void)
{
#ifdef CONFIG_BUILD_PROTECTED
	uintptr_t datastart;
	uintptr_t dataend;
#endif

	/* Show MPU information */

	mpu_showtype();

#ifdef CONFIG_ARMV7M_DCACHE
	/* Memory barrier */

	ARM_DMB();

#ifdef CONFIG_IMXFT_QSPI
	/* Make QSPI memory region strongly ordered */

	mpu_priv_stronglyordered(IMXRT_QSPIMEM_BASE, IMXRT_QSPIMEM_SIZE);

#endif
#endif

#ifdef CONFIG_BUILD_PROTECTED
	/* Configure user flash and SRAM space */

	DEBUGASSERT(USERSPACE->us_textend >= USERSPACE->us_textstart);

	mpu_user_flash(USERSPACE->us_textstart, USERSPACE->us_textend - USERSPACE->us_textstart);

	datastart = MIN(USERSPACE->us_datastart, USERSPACE->us_bssstart);
	dataend = MAX(USERSPACE->us_dataend, USERSPACE->us_bssend);

	DEBUGASSERT(dataend >= datastart);

	mpu_user_intsram(datastart, dataend - datastart);
#endif

	/* Then enable the MPU */

	mpu_control(true, false, true);
}

/****************************************************************************
 * Name: imxrt_mpu_uheap
 *
 * Description:
 *  Map the user-heap region.
 *
 *  This logic may need an extension to handle external SDRAM).
 *
 ****************************************************************************/

#ifdef CONFIG_BUILD_PROTECTED
void imxrt_mpu_uheap(uintptr_t start, size_t size)
{
	mpu_user_intsram(start, size);
}
#endif

#endif							/* CONFIG_ARMV7M_MPU */
