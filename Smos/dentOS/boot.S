/*
 * Copyright (C) 2019
 */

#include "boot.h"

.code16 // 16位
.globl _start // 开始处
_start:

bpbBytesPerSector:  	DW 512 // 扇区的字节数。这必须是2的幂。通常对于软盘，它是512字节
bpbSectorsPerCluster: 	DB 1 // 指示每个群集的扇区数。在我们的例子中，我们想要一个扇区集群
bpbReservedSectors: 	DW 1 // 一个保留扇区是不包括在FAT12扇区数。即，不是根目录的一部分。在我们的例子中，包含我们的引导加载程序的Bootsector将不属于此目录。因此，bpbReservedSectors应为1。这也意味着保留的扇区（我们的引导加载程序）将不包含文件分配表
bpbNumberOfFATs: 	    DB 2 // 表示磁盘上的文件分配表（FAT）的数量。FAT12文件系统总是有2个FAT。
bpbRootEntries: 	    DW 224 // 软盘在其根目录中最多包含224个目录
bpbTotalSectors: 	    DW 2880 // 软盘中有2,880个扇区。
bpbMedia: 	            DB 0xF0
bpbSectorsPerFAT: 	    DW 9
/*
该介质描述符字节（bpbMedia）是包含有关磁盘信息的字节码。这个字节是一个位模式：
位0：侧面/头部= 0如果是单面的，1是双面的
位1：如果每个FAT有9个扇区，则大小= 0，如果有8个扇区则为1。
位2：密度= 0，如果它有80个磁道，如果它是40磁道，则为1。
位3：如果是固定磁盘（如硬盘驱动器），则键入= 0，如果可移动则为1（如软盘驱动器）
第4位到第7位未使用，始终为1。
0xF0 = 11110000二进制。这意味着它是单面的，每个FAT 9个扇区，80个磁道，并且是可移动磁盘。 看看bpbSectorsPerFAT，你会发现它也是9。
*/

bpbSectorsPerTrack: 	DW 18 // 每个轨道有18个扇区
bpbHeadsPerCylinder: 	DW 2 // 表示有2个头代表一个圆柱体
bpbHiddenSectors: 	    DD 0 // 表示从物理磁盘的开始和卷的开始的扇区数
bpbTotalSectorsBig:     DD 0
bsDriveNumber: 	        DB 0
// 软盘驱动器是驱动器0？

bsUnused: 	            DB 0
bsExtBootSignature: 	DB 0x29
/*
Boot Signiture表示此BIOS参数块（此OEM表）的类型和版本。价值观是：
0x28和0x29表示这是MS / PC-DOS版本4.0 Bios参数块（BPB）
我们有0x29，所以这是我们使用的版本。
*/

bsSerialNumber:	        DD 0xa0a1a2a3 // 序列号由格式化它的实用程序分配。序列号对于该特定软盘是唯一的，并且没有两个序列号应该是相同的
bsVolumeLabel: 	        DB "MOS FLOPPY " // 是一个字符串，用于指示磁盘上的内容。有些操作系统会将其显示为名称。 注意：此字符串*必须*为11个字节。不多也不少
bsFileSystem: 	        DB "FAT12   " // 用于相同的目的，而不是更多。注意：此字符串*必须*为8个字节，不多也不少

	xor %ax, %ax // 异或清0
/*
cs是代码段寄存器
ds是数据段寄存器
ss是堆栈段寄存器
es是扩展段寄存器
fs是标志段寄存器
gs是全局段寄存器
SI是源变址寄存器，DI是目的变址寄存器
ESP：栈指针寄存器，其内存放着一个指针，该指针永远指向系统栈最上面一个栈帧的栈顶。
EBP：基址指针寄存器，其内存放着一个指针，该指针永远指向系统栈最上面一个栈帧的底部。
*/
	// 将ds, es, ss 都置为0
	mov %ax, %ds
	mov %ax, %es 
	mov %ax, %ss

	mov $0x7C00 %esp // 栈指针指向0x7c00. 将boot.S加载到0x7C00 堆栈从偏移量0x7C00向下扩展到0x0000。

	call print
	.string "[bootloader] initializing...\n"

// https://blog.csdn.net/williamwanglei/article/details/10518811
	mov $(LOADER_KERN_BASE / 0x10), %ax // 0x90000 
	mov %ax, %es #指向0x9000 // 将自己移动到 0x9000
	xor %bx, %bx

	call load_setup

	call load_efi

load_setup:
	pusha
	call print
	.string "[bootloader] Configurating boot sectors...\n"

	// 设置硬盘 开始加载setup
/*
ah = 0x02 读磁盘扇区到内存； al = 需要读出的扇区数量
ch = 磁道(柱面)号的低8位；   cl = 开始扇区(位0-5)，磁道号高2位(位6-7)；
dh = 磁头号；		    dl = 驱动器号(如果是硬盘则位7要置位)；
es:bx -> 指向数据缓冲区；如果出错则CF标志置位，ah中是错误码。
*/
	mov $0x0220, %ax # AH->sr AL->nrSec
	mov $0x0002, %cx # CH->cyl CL->sec
	mov $0x0000, %dh # DH->head DL->drive DH =头号
	call print
	.string "[bootloader] Resetting hard drive...\n"

	#Reset hard diver 重置硬盘
	int $0x13 # 中断 INT 0x13 / AH = 0x02 - 磁盘：读取 扇区（S）进入存储器？？
	or %ah, %ah
	jz popa_ret

	call print
	.string "[bootloader] loaded!\n"

	hlt // 停止执行
	jmp hit

load_efi:
	# Start address from EFI (offset 0x18) EFI的起始地址(偏移0x18)
	mov %es:0x18, %dx
	mov %dx, addr_bootable 
	movw $(LOADER_KERN_BASE / 0x10), addr_bootable + 2 // 将EFI加载到 0x1000

	call print
	.string "[bootloader] EFI address loaded\n"
	ljump *addr_bootable
	ret


print:
	xchg %si, %ss:(%esp)
	push %ax

print_next_char:
	move %cs:(%si), %al
	inc %si
	test %al, %al # 测试字符是否为0（结束）
	jz 1f #前一个指令测试为0则跳到1:
	call put_char
	jmp print_next_char

1:
	pop %ax
	xchg %si, %ss:(%esp)
	ret

put_char:
/*
INT 0x10 - 视频电话输出
AH = 0x0E 
AL =写入
BH的字符- 页码（应为0）
BL =前景色（仅限图形模式）
*/
	pusha
	sub %bh, %bh 
	mov %0x0e, %ah # int 10h的子功能0xe（视频电传输出）。
	int $0x10

popa_ret:
	popa 
	ret #error code still in CF

addr_bootable:
	.word 0x0000
	.word 0x0000

	.org LOADER_SIG-LOADER_BASE
	.word 0xaa55 # 告诉BIOS是可引导的.
