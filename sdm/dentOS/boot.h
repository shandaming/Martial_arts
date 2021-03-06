/*
 * Copyright (C) 2019
 */

#ifndef OS_BOOT_H
#define OS_BOOT_H

#define LOADER_BASE 0x7c00
#define LOADER_END  0x7e00

#define KERNEL_BASE 0x100000

#define LOADER_KERN_BASE 0x90000

#define LOADER_PHYS_BASE 0x00000000

#define LOADER_SIG (LOADER_END - LOADER_SIG_LEN)
#define LOADER_PARTS (LOADER_SIG - LOADER_PARTS_LEN)

#define LOADER_SIG_LEN 2
#define LOADER_PARTS_LEN 64

#define SEL_NULL        0x00
#define SEL_KCSEG       0x08
#define SEL_KDSEG       0x10

#define STACK_POINTER 0x7C00

#endif
