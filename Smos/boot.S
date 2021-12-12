.code16  
.section .text  

bpbBytesPerSector:
	.world 512

bpbSectorsPerCluster:
	.byte 1

bpbReservedSectors:
	.world 1

bpbNumberOfFATs:
	.byte 2

bpbRootEntries:
	.world 224

bpbTotalSectors:
	.world 2880

bpbMedia:
	.byte 0xf0

bpbSectorsPerFAT:
	.world 9

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

.globl _start  
_start:  
    movw %cs, %ax  
    movw %ax, %ds  
    movw %ax, %es  
    call DispStr  
loop1:  
    jmp loop1  
  
DispStr:  
    movw $BootMessage, %ax  
    movw %ax, %bp  
    movw $0x10, %cx  
    movw $0x1301, %ax  
    movw $0xc, %bx  
    movb $0, %dl  
    int $0x10  
    ret  
BootMessage:  
    .ascii "Hello, MN world!"  
    .org 510  
    .word 0xAA55 
