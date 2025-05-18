; File Name: boot2.asm
; Created by: Eric Hooks
; Date of creation: 01/27/2011
; Purpose: To add assembly functions to the OS.

; Declaration
global writeScr:function
global pmWriteScr:function
global loadGdtr:function
global loadSel:function
global schedule:function
global go:function
global loadIdtr:function
global outportb:function
global spinlock:function
global unlock:function
global mSchedule:function
global sSchedule:function
global inportb:function
global reboot:function

extern dequeue
extern enqueue
extern mEnqueue
extern sEnqueue
extern curPcb

timerTicks db 0

;Start Function writeScr
writeScr:
	; Push ebp inorder to secure the stack frame
	push ebp
	
	; Move esp into ebp, so that we may save the location in the stack
	mov ebp, esp
	
	; Save all of the registers
	pushad
	
	; Push es onto the stack
	push es
	
	; Move the start of the string into esi
	mov esi, [ebp + 8]
	
	; Move the location of the video offset into 
	mov edi, 0xb8000
	
	; Move the number of rows into eax
	mov eax, [ebp + 12]
	
	; Multiply the number of rows by the screen length
	imul eax, 80
	
	; Add the number of columns to eax
	add eax, [ebp + 16]
	
	; Multiply eax by 2
	imul eax, 2
	
	; Add the new starting location to edi
	add edi, eax
	
	; Clear the direction bit
	cld
	
	printLine:
		; Check to see if the string is at the null terminator
		cmp byte [esi], 0
		
		; If the result is 0, jump to done
		jz done
		
		; Else, copy the string into the video buffer
		movsb
		
		; Move in the color bit
		mov byte [es:edi], 0Fh

		; Increment edi so that we can get the next character
		inc edi
		
		; Jump back to the start of the loop
		jmp printLine
	
	done:
		; Pop es
		pop es
		
		; Pop the registers
		popad
		
		; Pop the base pointer
		pop ebp
		
		; Return
		ret

; Start function pmWriteScr
pmWriteScr:
	; Push ebp inorder to secure the stack frame
	push ebp
	
	; Move esp into ebp, so that we may save the location in the stack
	mov ebp, esp
	
	; Save all of the registers
	pushad
	
	; Push es onto the stack
	push es

	; Push the flags
	pushf
	
	; Move the start of the string into esi
	mov esi, [ebp + 8]
	
	; Move 0 into edi to start the video segment
	mov edi, 0

	; Move 32 into eax (The offset of the descriptor table)
	mov eax, 32

	; Move eax into es to set the video segment
	mov es, eax
	
	; Move the number of rows into eax
	mov eax, [ebp + 12]
	
	; Multiply the number of rows by the screen length
	imul eax, 80
	
	; Add the number of columns to eax
	add eax, [ebp + 16]
	
	; Multiply eax by 2
	imul eax, 2
	
	; Add the new starting location to edi
	add edi, eax
	
	; Set the color bit
	mov ebx, [ebp + 20]
	
	; Clear the direction bit
	cld

	; Turn off interrupts
	cli
	
	pmPrintLine:
		; Check to see if the string is at the null terminator
		cmp byte [esi], 0
		
		; If the result is 0, jump to done
		jz pmDone
		
		; Else, copy the string into the video buffer
		movsb
		
		; Move in the color bit
		mov byte [es:edi], bl

		; Increment edi so that we can get the next character
		inc edi
		
		; Jump back to the start of the loop
		jmp pmPrintLine
	
	pmDone:
		; Pop the flags
		popf

		; Pop es
		pop es

		; Pop the registers
		popad
		
		; Pop the base pointer
		pop ebp
		
		; Return
		ret



; Start function loadGdtr
loadGdtr:
	; Push the current base pointer in order to establish the stack frame
	push ebp

	; Move the current stack pointer into the base pointer
	mov ebp, esp

	; Save all registers
	push eax

	; Move the descriptor table into eax
	mov eax, [ebp + 8]

	; Load the descriptor into the descriptor register
	lgdt [eax]

	; Pop all registers
	pop eax

	; Pop the base pointer to restore the previous stack frame
	pop ebp

	; Return
	ret

; Start function loadIdtr
loadIdtr:
	; Push the current base pointer in order to establish the stack frame
	push ebp

	; Move the current stack pointer into the base pointer
	mov ebp, esp

	; Save eax
	push eax

	; Move the descriptor table into eax
	mov eax, [ebp + 8]

	; Load the descriptor into the descriptor register
	lidt [eax]

	; Pop eax
	pop eax

	; Pop the base pointer to restore the previous stack frame
	pop ebp

	; Return
	ret

; Start function loadSel
loadSel:
	; Push the current base pointer to establish the stack frame
	push ebp

	; Move the stack pointer into ebp
	mov ebp, esp

	; Push eax
	push eax

	; Move data segment into eax
	mov eax, [ebp + 12]

	; Move data segment into ds
	mov ds, eax

	; Setup es
	mov eax, [ebp + 24]

	; Move the video segment in to es
	mov es, eax

	; Setup gs and fs
	mov eax, [ebp + 20]
	mov fs, eax
	mov eax, [ebp + 20]
	mov gs, eax

	; Setup ss
	mov eax, [ebp + 16]

	; Move the stack segment into ss
	mov ss, eax

	; Move the code segment into eax
	mov eax,  [ebp + 8]

	; Push eax
	push eax

	; Push loadcs
	push loadcs

	; Return far
	retf

loadcs:
	; Pop eax
	pop eax

	; Pop ebp
	pop ebp

	; Return
	ret

; Start schedule function
schedule:
	; Push eax in order to save it
	push eax

	; Clear out eax and place 0x20 into al
	mov eax, 0
	mov al, 0x20

	; Write the contents of al to port 0x20
	out 0x20, al

	; Pop eax off the stack
	pop eax

	; Increment timerTicks
	inc BYTE [timerTicks]

	; Check to see if timerTicks is 3 if so switch processes
	cmp BYTE [timerTicks], 3

	; If timerTicks is equal to 3, switch
	je switch

	; Return with interrupt
	iret

switch:
	; Set timerTicks back to 0
	mov BYTE [timerTicks], 0

	; Save all general purpose registers
	pushad

	; Save the other selectors
	push ds
	push es
	push fs
	push gs

	; Save the esp register of the current process
	mov eax, [curPcb]
	mov [eax], esp
	mov [eax + 4], ss

	; enqueue the pcb of the current process
	push eax
	call enqueue

	; Pop eax back off the stack
	pop eax

	; dequeue the next process
	call dequeue

	; Set the esp register to the value of the pcb's esp register
	mov ss, [eax + 4]
	
	; set the ss register to that of the pcb's ss register
	mov esp, [eax]

	; Make the process current
	mov [curPcb], eax

	; Restore gs
	pop gs
	
	; Restore fs
	pop fs

	; Restore es
	pop es

	; Restore ds
	pop ds

	; Restore all other registers
	popad

	; Return with interrupt
	iret

; Start the go function
go:
	; Call dequeue
	call dequeue

	; Put the pointer to the pcb into esp
	mov esp, [eax]

	; Move the stack selector of the pcb into the current stack selector
	mov ss, [eax + 4]

	; Make the process currrent
	mov [curPcb], eax

	; Pop the state of the process
	pop gs
	pop fs
	pop es
	pop ds

	; Restore all of the general purpose registers
	popad

	; Return with interrupt
	iret

; Start function outportb
outportb:
	; Push ebp
	push ebp

	; Move esp into ebp to establish the stack frame
	mov ebp, esp

	; Push the flags register
	pushf

	; Push all the data regisers except for eax
	pushad

	; Clear edx and then get the port to write to and place it in dx
	mov edx, 0
	mov dx, [ebp + 8]

	; Clear out eax and then get the information to write to the port and place it in al
	mov eax, 0
	mov al, [ebp + 12]

	; Write to the port
	out dx, al

	; Restore the data registers
	popad

	; Restore the flags
	popf

	; Restore the previous stack frame
	pop ebp

	; Return
	ret

; Start function inportb
inportb:
	; Push ebp
	push ebp
	
	; Move esp into ebp to establish the stack frame
	mov ebp, esp
	
	; Push the flags
	pushf
	
	; Push edx
	push edx
	
	; Clear edx and get the port to read from and place it in dx
	mov edx, 0
	mov dx, [ebp + 8]
	
	; Clear out eax
	mov eax, 0
	
	; Read from port
	in al, dx
	
	; Pop edx
	pop edx
	
	; Pop the flags
	popf
	
	; Pop ebp
	pop ebp
	
	; Return
	ret
	
; Start function reboot
reboot:
	; Push ebp
	push ebp
	
	; Move esp into ebp
	mov ebp, esp
	
	; Push the flags
	pushf
	
	; Push the registers
	pushad
	
	rLoop:
		; Read in from the keyboard
		in al, 0x64
		
		; See if the port is empty
		test al, 00000010b
		
		; Jump back if it is not empty
		jne rLoop
		
	; Send the reboot signal
	mov al, 0xFE
	out 0x64, al
	
	; Pop the registes
	popad
	
	; Pop the flags
	popf
	
	; Pop ebp
	pop ebp
	
	; Return
	ret
	