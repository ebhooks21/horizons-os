boot1:	boot1.asm boot2
	# Create boot1
	nasm -l boot1.list -DENTRY=`sh getaddr.sh main` boot1.asm

# Create the executable file for boot2
boot2:	boot2.exe
	# Create boot2
	objcopy -R .dynamic -R .note -R .comment -S -O binary boot2.exe boot2

# Create the object file for boot2.c
boot2_c.o:	boot2.c
	# Create the object file for boot2.c
	gcc -m32 -o boot2_c.o -c boot2.c
	
boot2_asm.o:	boot2.asm
	# Create boot2_asm.o
	nasm -o boot2_asm.o -f elf32 boot2.asm
	
interrupts_c.o:	interrupts.c
	# Create interrupts_c.o
	gcc -m32 -o interrupts_c.o -c interrupts.c
	
keyboard_c.o:	Keyboard.c
	# Create keyboard_c.o
	gcc -m32 -o keyboard_c.o -c Keyboard.c
	
system_c.o:     System.c
	# Create system_c.o
	gcc -m32 -o system_c.o -c System.c

boot2.exe:	boot2_c.o	boot2_asm.o interrupts_c.o keyboard_c.o system_c.o
	# Create boot2.exe
	ld  -m elf_i386 -Ttext 0x10000 -e main -o boot2.exe boot2_asm.o boot2_c.o interrupts_c.o keyboard_c.o system_c.o

# Option to clean up the directory
.PHONY:	clean install
clean:
	# Clean up the directory
	rm -f *.o *.exe *.img boot1 boot2 *.list
		
# Option to do everything
all:
	# Create the OS
	nasm -o boot2_asm.o -f elf32 boot2.asm

	gcc -o boot2_c.o -c boot2.c

	ld  -m elf_i386 -Ttext 0x10000 -e main -o boot2.exe boot2_asm.o boot2_c.o

	objcopy -R .dynamic -R .note -R .comment -S -O binary boot2.exe boot2

	nasm -l boot1.list -DENTRY=`getaddr.sh main` boot1.asm

# Option to install the OS
install:
	# Create the disk image that the OS will go on
	bximage -fd -size=1.44 -q a.img

	# Format the image that the OS will go on
	mformat a:
	#mformat /dev/sdb
	
	# Copy bootloader onto floppy disk
	dd if=boot1 of=a.img bs=1 count=512 conv=notrunc
	
	# Copy the OS to the disk
	mcopy -o boot2 a:BOOT2
	#mcopy -o boot2 /dev/sdb:BOOT2

# Run the OS
run:
	# Run the OS in bochs
	bochs -q -f bochsrc.txt
