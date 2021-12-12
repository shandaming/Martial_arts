/*
 * Copyright (C) 2019
 */

#include "boot.h"

.code16
.section setup

#define CR0_PE 0x00000001      /* 保护启用 */
#define CR0_EM 0x00000004      /* 浮点仿真 */
#define CR0_PG 0x80000000      /* Paging */
#define CR0_WP 0x00010000      /* 内核模式下的写保护启动. */

.globl setup
setup:
	mov $(LOADER_KERN_BASE / 0x10), %ax // 9000
	mov %ax, %ds
	mov %ax, %es

	movl $ram_map - LOADER_PHYS_BASE - KERNEL_BASE, %eax
	movl $100, %edx
	push $0x00

	call detectMemory
	addr32 movl %eax, ram_size - LOADER_PHYS_BASE - KERNEL_BASE

	cli #清除中断(停止中断)
	cld

＃忙时轮询状态注册。

1:
	inb $0x64, %al // 从I/O端口读取一个字节
	testb $0x2, %al
	jnz 1b

#send命令用于写入输出端口。

	movb $0xd1, %al
	outb %al, $0x64

#忙时轮询状态注册。

1:
	intb $0x64, %al
	tesb $0x2, %al
	jnz 1b

#启用A20线路。

	movb $0xdf, %al
	outb %al, $0x60

#忙时轮询状态注册。

1:
	inb $0x64, al
	testb $0x2, al
	jnz 1b

/*
 * 保护模式需要GDT，所以将GDTR指向我们的GDT。 
 * 我们需要一个data32前缀来确保加载GDT描述符的所有32位（默认只加载24位）。 
 * CPU不需要addr32前缀，但ELF不执行16位重定位。
 */

	addr32 lgdt gdtdesc - LOADER_PHYS_BASE - KERNEL_BASE

/*
 * 然后我们打开CR0中的以下位：
 *  PE（保护启用）：打开保护模式。
 *  PG（Paging）：打开分页。
 *  WP（写保护）：如果未设置，则响铃0代码忽略页表中的写保护位（！）。
 *  EM（Emulation）：强制浮点指令陷阱。我们不支持浮点。
 */

	movl %cr0, %eax
	orl $CR0_PE | CRO_WP | CRO_EM, %eax
	movl %eax, %cr0

/* 
 * 我们现在处于16位段的保护模式。 
 * CPU仍然具有在％cs的段描述符中缓存的实模式代码段。 
 * 我们需要重新加载％cs，最简单的方法是使用远程跳转。 
 * 因为我们不是在32位段中运行，所以需要data32前缀来跳转到目标段中的32位偏移量。
 */

	data32 ljmp $SEL_KCSEG, $1f + LOADER_KERN_BASE - LOADER_PHYS_BASE - KERNEL_BASE

#我们现在处于32位段的保护模式。
#让汇编程序知道。

.code32

#重新加载所有其他段寄存器和堆栈指针以指向我们的新GDT。

1:
	mov $SEL_KDSEG, %al
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss

	movl $LOADER_KERN_BASE, %esi
	movl $KERNEL_BASE, %edi

copy:
	movl $0x8000, %ecx ＃32kB一次
	rep
	movsw

	cmp $KERNEL_BASE, %esi
	jnz copy

	ljmp $SEL_KCSEG, %start

#### GDT

	.glign 8
gdt:
	.quad 0x0000000000000000 #Null segment。 不用于CPU。
	.quad 0x00cf9a000000ffff #系统代码，基数为0，限制为4 GB。	
	.quad 0x00cf92000000ffff #系统数据，基数为0，限制为4 GB。

.gdtdesc
	.word gdtdesc - gdt - 1 #GDT的大小减去1个字节。
	.long gdt + LOADER_KERN_BASE - LOADER_PHYS_BASE - KERNEL_BASE #GDT的地址。

#### 4 kB页面中的物理内存大小。 这将导出到其余部分
内核的####。

.globl ram_map, ram_size
ram_size:
	.long 0x00
ram_map:
	.fill 2000 / 8, 0x08, 0x00  含义是反复拷贝0x08个字节，重复2000/8次, value = 0x00
