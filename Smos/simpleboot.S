/*
 * Copyright (C) 2019
 */

.code16
.section .data
.section .text

.globl _start
_start:

	movw %cs, %ax
	movw %ax, %ds
	movw %ax, %es
	call print
loop1:
	jmp loop1

/* 为什么要写在后面才能显示？ */
.type print @function
print:
	movw $msg, %ax
	movw %ax, %bp
	movw $0x10, %cx
	movw $0x1301, %ax
	movw $0xc, %bx
	movb $0, %dl
	int $0x10
	ret

msg:
	.ascii "Hello World."
	.org 510 # 指定一个地址，后面的程序或数据从这个地址值开始分配。
	.word 0xaa55 #启动签名  512的最后2字节

/*
	. = _start + 510 # 从开始到510个字节遍历
	.byte 0x55
	.byte 0xaa
	*/

/*
as -gstabs boot.s -o boot.o                              break *0x7c00
ld -Ttext=0x7c00 --oformat binary -o boot.bin boot.o  
dd if=/dev/zero of=emptydisk.img bs=512 count=2880  
dd if=boot.bin of=Finix.img bs=512 count=1  
dd if=emptydisk.img of=Finix.img skip=1 seek=1 bs=512 count=2879 


as test.S -o test.o
ld -Ttext 0x7c00 --oformat = binary test.o -o test.bin

--oformat = binary开关告诉链接器您希望输出文件是纯二进制映像（没有启动代码，没有重定位，......）.
-Ttext 0x7c00告诉链接器您希望将“文本”（代码段）地址加载到0x7c00，从而计算绝对寻址的正确地址。.

如何将可执行代码复制到可启动设备然后进行测试？
要创建1.4mb大小的软盘映像，请在命令提示符下键入以下内容。

dd if = / dev / zero of = floppy.img bs = 512 count = 2880
要将代码复制到软盘映像文件的引导扇区，请在命令提示符下键入以下内容。

dd if = test.bin of = floppy.img
要测试程序，请在命令提示符下键入以下内容

Bochs的
如果未安装bochs，则可以键入以下命令

sudo apt-get install bochs-x
*/
