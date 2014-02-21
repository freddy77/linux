/*
 *  include/linux/irqchip/hip04-gic.h
 *
 *  Copyright (C) 2013-2014 Hisilicon Limited, All Rights Reserved.
 *  Copyright (C) 2013-2014 Linaro Limited, All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef __LINUX_IRQCHIP_HIP04_GIC_H
#define __LINUX_IRQCHIP_HIP04_GIC_H

/* GIC Implementation */
#define ARM_GIC_IMPLEMENTATION			(1 << 0)
#define HIP04_GIC_IMPLEMENTATION		(1 << 1)

#define HIP04_GIC_DIST_TARGET			0x800

#define HIP04_GIC_DIST_SGI_PENDING_CLEAR	0xf40
#define HIP04_GIC_DIST_SGI_PENDING_SET		0xf80

#define HIP04_GICH_APR				0x70
#define HIP04_GICH_LR0				0x80

#define HIP04_GICH_LR_PHYSID_CPUID	(0xf << GICH_LR_PHYSID_CPUID_SHIFT)

#ifndef __ASSEMBLY__
extern unsigned long arm_gic_im;
#endif

#endif	/* __LINUX_IRQCHIP_HIP04_GIC_H */
