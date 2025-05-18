/**
* File Name: boot2.c
* Created by: Eric Hooks
* Date of Creation: 01/21/2010
* Purpose: To be the main program for the first program for Operating systems.
* The boot loader will load the main function of this C program, which will then clear the screen.
* The program will then call an external assembly procedure to print a message to the middle of the screen.
*/

//Included Libraries
#include "GDT.h"
#include "IDT.h"
#include "Keyboard.h"
#include "System.h"
#include "Definitions.h"

//Structure for the process control block
struct ProcessControlBlock
{
	//Variable to hold the system stack pointer
	uint32_t esp;

	//Variable to hold the stack segment
	uint32_t ss;
} __attribute__((packed));

/************ Global Variables *******************/
//Descriptor tables
gdt_entry_t gdt[5]; //Global descriptor table
gdt_info_t gdtr; //Reference to the Global descriptor table
idt_entry_t idt[256]; //Interrupt descriptor table
idt_info_t idtr; //Refernce to the Interrupt descriptor table
int conCurRow = 0; //Varialbe to keep track of the console cursor's row
int conCurCol = 0; //Varialbe to keep track of the console cursor's column
int keypressed = 0;

//Process information
uint32_t pStack [10][1024]; //The process stack
int nextStack = 0; //Marker for the next stack in the stack list
pcb processes[11]; //An array of processes
int nextPcb = 0; //Marker for the next pcb in the list
pcb* queue[10]; //The RR queue
int queueBack = 0; //Variable for the back of the queue
int stackSize = 1024; //Variable to hold the stack size
pcb* curPcb; //Variable to hold the current process control block

//Hardware
system_t system; //The System itself
keyboard_t keyboard; //The Keyboard

/**
* Main function for boot2.c
* Preconditions: none
* Postconditions: The program runs.
*/
int main()
{
	//Make a variable for the temp storage
	char* temp;
	
	//Setup the system
	init_system(&system);
	
	//Clear the screen
	clearScr();
	
	//Setup the gdt
	writeScr("Setting up the Global Descriptor Table... ", conCurRow, conCurCol);
	//Fix the current column
	conCurCol += 42;
	
	//Setup descriptor 0; it is set to null
	initGDTEntry(&gdt[0], (uint32_t)0, (uint32_t)0, (uint8_t) 0, (uint8_t)0);
	//Setup the code segment descriptor (cs)
	initGDTEntry(&gdt[1], (uint32_t)0, (uint32_t)((640 * 1024) - 1), (uint8_t)0x9a, (uint8_t)0x40);
	//Setup the data segment descriptor (ds)
	initGDTEntry(&gdt[2], (uint32_t)0, (uint32_t)((640 * 1024) - 1), (uint8_t)0x92, (uint8_t)0x40);
	//Setup the stack segment descriptor (ss)
	initGDTEntry(&gdt[3], (uint32_t)0, (uint32_t)((640 * 1024) - 1), (uint8_t)0x92, (uint8_t)0x40);
	//Setup the video segment descriptor (es)
	initGDTEntry(&gdt[4], (uint32_t)0xb8000, (uint32_t)((80*25*2)-1), (uint8_t)0x92, (uint8_t)0x40);

	//Prepare the gdt information
	gdtr.gdtBase = (uint32_t)gdt;
	gdtr.gdtLimit = (uint32_t)(sizeof(gdt) - 1);

	//Load the gdt register using assembly
	loadGdtr(&gdtr);
	 
	//Load the gdt into the selectors
	loadSel((int)8, (int)16, (int)24, (int)16, (int)32);
	
	//Write done to the screen
	pmWriteScr("done", conCurRow, conCurCol, 02);
	
	//Reset the cursor
	conCurRow = 1;
	conCurCol = 0;
	
	//Setup the IDT
	pmWriteScr("Setting up the Interrupt Descriptor Table... ", conCurRow, conCurCol, 0x0f);
	initIDT();
	conCurCol += 45;
	pmWriteScr("done", conCurRow, conCurCol, 02);
	
	//Reset the cursor
	conCurRow = 2;
	conCurCol = 0;

	//Turn off interrupts
	asm("cli");

	//Setup the programmable interrupt controller
	pmWriteScr("Setting up IRQs... ", conCurRow, conCurCol, 0x0f);
	setupPIC();
	conCurCol += 19;
	pmWriteScr("done", conCurRow, conCurCol, 02);
	
	//Reset the cursor
	conCurRow = 3;
	conCurCol = 0;
	
	//Setup the keyboard
	pmWriteScr("Setting up the keyboard... ", conCurRow, conCurCol, 0x0f);
	init_keyboard(&keyboard, 104);
	conCurCol += 27;
	pmWriteScr("done", conCurRow, conCurCol, 02);
	
	//Reset the cursor
	conCurRow = 4;
	conCurCol = 0;
		
	//Detect the amount of ram the system has
	pmWriteScr("Amount of usable system memory detected(megabytes): ", conCurRow, conCurCol, 0x0f);
	conCurCol += 52;
	
	if(system.ramB > 0)
	{
		pmWriteScr(itoa(getRamMB(system.ramB), temp, 10), conCurRow, conCurCol, 02);
	}
	
	else
	{
		pmWriteScr("0", conCurRow, conCurCol, 04);
	}
	
	//Reset the cursor
	conCurRow = 5;
	conCurCol = 0;

	//Variable to hold the current stack
	uint32_t s;
	
	//Create 10 processes
	s = allocStack();
	createProcess((uint32_t)16, (uint32_t)24, (uint32_t) (s + stackSize), (uint32_t)8, (uint32_t)idle);
	
	s = allocStack();
	createProcess((uint32_t)16, (uint32_t)24, (uint32_t) (s + stackSize), (uint32_t)8, (uint32_t)prompt);

	//Start process switching
	go();
}

/**
* Function to clear the screen.
* Preconditions: none
* Postconditions: The screen is cleared.
* Parameters: none
*/
void clearScr()
{
     //Create a counter for the for loop
     int i;
     
     //For-Loop to clear the screen
     for(i = 0; i < 40; i++)
     {
		//Print a blank line to the screen
		writeScr("                                                                                ", i, 0);
     }
}

/**
* Function to display a prompt to the user.
* Preconditions: none
* Postconditions: A prompt is displayed to the user.
*/
void prompt()
{
	//Display a prompt and set the cursor location
	//Increment conCurRow and reset conCurCol
	conCurRow++;
	conCurCol = 0;
	
	//Display the prompt
	pmWriteScr("prompt$ ", conCurRow, conCurCol, 0x0f);
	
	//Increment to new cursor location
	conCurCol += 8;

	//Loop forever
	while(1)
	{
		//Loop until an action occurs
		while(keyboard.enterPressed == 0)
		{
			if(keypressed == 1)
			{
				//Display the key on the screen
			displayBuffer(&keyboard, conCurRow, conCurCol, 0x0f);
			
			//Increment the current column
			conCurCol++;
			
			keypressed = 0;
			}
		}
	
		//Make sure enter was pressed
		if(keyboard.enterPressed == 1)
		{
			//Increment the row
			conCurRow++;
		
			//Display the prompt
			pmWriteScr("prompt$", conCurRow, conCurCol, 0x0f);
			
			//Increment to new cursor location
			conCurCol += 8;
		}
	}
}

/**
* Function to initialize a descriptor in the gdt.
* Preconditions: none
* Postconditions: A descriptor is setup.
* Parameters:
*	gdt_entry_t *entry - A descriptor to be setup.
*	uint32_t baseAddress - The address of the base of the descriptor.
*	uint32_t limit - The address of the limit of the descriptor.
*	uint8_t access - The information for the access part of the descriptor.
*	uint8_t attributes - The information for the attributes part of the descriptor.
*/
void initGDTEntry(gdt_entry_t *entry, uint32_t baseAddress, uint32_t limit, uint8_t access, uint8_t attributes)
{
	//Temporary variable for a uint8_t
	uint8_t temp = 0;
	
	//Setup the base address of the gdt
	entry->segBase = (baseAddress & 0xffff);
	entry->baseMid8 = (baseAddress >> 16) & 0x0ff;
	entry->baseUpper8 = (baseAddress >> 24) & 0x0ff;
	
	//Setup the limit address of the gdt
	entry->segLimit = (limit & 0x0ffff);
	entry->attributes = attributes;

	//Get the upper part of the limit
	temp = (limit >> 16) & 0x0f;
	entry->attributes = (entry->attributes | temp);
	
	//Setup the access of the gdt
	entry->access = access;
}

/**
* Function to be an idle process.
* Preconditions: none
* Postconditions: The function acts as a process.
* Parameters: none
*/
void idle()
{
    //Loop forever
	while(1);
}

/**
* Function to create a process.
* Preconditions: none
* Postconditions: A process is created.
*/
int createProcess(uint32_t ds, uint32_t ss, uint32_t stackTop, uint32_t cs, uint32_t processEntry)
{
	//Temporary pcb pointer
	pcb* p;

    //Create a pointer to the top of the stack
    uint32_t * st;
    st = (uint32_t*)stackTop;

    //Subtract 1 from st
    st--;
    
    //Set the word pointed to by st to ox200
	/*st = (uint32_t)0x0000;*/
	*st = (uint32_t)0x200;
    
     //Subtract 1 from st
	st--;

	//Set the word that st references to the value of cs
	*st = cs;

	//Subtract 1 from st
	st--;

	//Set the word that st references to the value of processEntry
	*st = processEntry;

	//Subtract 1 from st
	st--;

	//Set st to the value of ebp(0)
	*st = 0;

	//Subtract 1 from st
	st--;

	//Set st to the value of esp(0)
	*st = 0;

	//Subtract 1 from st
	st--;

	//Set st to the value of edi(0)
	*st = 0;

	//Subtract 1 from st
	st--;

	//Set st to the value of esi(0)
	*st = 0;

	//Subtract 1 from st
	st--;

	//Set st to the value of edx(0)
	*st = 0;

	//Subtract 1 from st
	st--;

	//Set st to the value of ecx(0)
	*st = 0;

	//Subtract 1 from st
	st--;

	//Set st to the value of ebx(0)
	*st = 0;

	//Subtract 1 from st
	st--;

	//Set st to the value of eax(0)
	*st = 0;

	//Subtract 1 from st
	st--;

	//Set st to the value of ds
	*st = ds;

	//Subtract 1 from st
	st--;

	//Set st to the value of es
	*st = ds;

	//Subtract 1 from st
	st--;

	//Set st to the value of fs(ds)
	*st = ds;

	//Subtract 1 from st
	st--;

	//Set st to the value of gs(ds)
	*st = ds;

	//Create the process control block
	p = allocPCB();

	//Increment nextPcb
	nextPcb++;

	//Store the value of the stack segment into the pcb
	p->ss = (uint32_t)ss;

	//Store the value of the esp into the pcb
	p->esp = (uint32_t)st;

	//Enqueue the process
	enqueue(p); 
}

/**
* Function to print to the screen after the GDT has been setup.
* Preconditions: The GDT has been setup.
* Postconditions: The screen is cleared.
*/
void postGdtClearScr()
{
	//Create a counter for the for loop
	int i = 0;

	//Clear the screen
	for(i = 0; i < 25; i++)
	{
		//Write to the screen
		pmWriteScr("                                                                                ", i, 0, 02);
	}
}


/**
* Function to allocate part of the process stack.
* Preconditions: none
* Postcondtions: Part of the process stack is allocated.
*/
uint32_t allocStack()
{
	//Variable to hold the stack
	uint32_t s;

	//Get the current part of the stack
	s = (uint32_t)pStack[nextStack];

	//Increment to the next stack
	nextStack++;

	//Return the new stack
	return s;
}

/**
* Function to allocate a pcb.
* Preconditions: none
* Postconditions: A pcb is allocated.
*/
pcb* allocPCB()
{
	//Return a pointer to a pcb
	return &(processes[nextPcb]);
}


/**
* Function to enqueue a process control block.
* Preconditions: none
* Postconditions: The pcb is enqueued.
*/
void enqueue(pcb* p)
{
	//Put the pcb on the end of the queue
	queue[queueBack] = p;

	//Increment queueBack
	queueBack++;

	//Error check queueBack
	if(queueBack == 11)
	{
		//Set queueBack to 0
		queueBack = 0;
	}
}


/**
* Function to dequeue a process control block
* Preconditions: none
* Postconditions: The pcb at the front of the queue is removed.
*/
pcb* dequeue()
{
	//Get the pcb on the front of the queue
	pcb* tmp = queue[0];

	//Counter for shifting
	int i = 0;

	//Shift all of the items forward to fix the queue
	for(i = 0; i < (queueBack); i++)
	{
		//Shift the item forward
		queue[i] = queue[i+1];
	}

	//Decrement queueBack
	queueBack--;

	//Return the pcb
	return tmp;
}

/**
* Function to setup the IDT.
* Preconditions: none
* Postconditions: The IDT is setup.
*/
void initIDT()
{
	//Counter for a for loop
	int i = 0;
	
	//Setup entry 0 as the divide by zero interrupt
	initIDTEntry(&idt[0], (uint32_t)divByZero, (uint16_t)8, (uint8_t)0x8e);

	//Setup entry 1 to debug exception
	initIDTEntry(&idt[1], (uint32_t)debugException, (uint16_t)8, (uint8_t)0x8e);

	//Setup interrupts 2 and 3
	for(i = 2; i < 4; i++)
	{
		//Setup as the generic handler
		initIDTEntry(&idt[i], (uint32_t)genericException, (uint16_t)8, (uint8_t)0x8e);
	}

	//Setup entry 4 as an overflow exception
	initIDTEntry(&idt[4], (uint32_t)overflowException, (uint16_t)8, (uint8_t)0x8e);
	
	//Setup entry 5 as an out of bounds exception
	initIDTEntry(&idt[5], (uint32_t)boundException, (uint16_t)8, (uint8_t)0x8e);
	
	//Setup entry 6 as an invalid opcode exception
	initIDTEntry(&idt[6], (uint32_t)invalidOpcode, (uint16_t)8, (uint8_t)0x8e);
	
	//Setup entry 7 as an fpu unavailable exeption
	initIDTEntry(&idt[7], (uint32_t)fpuUnavailable, (uint16_t)8, (uint8_t)0x8e);
	
	//Setup entry 8 as a double fault
	initIDTEntry(&idt[8], (uint32_t)doubleFault, (uint16_t)8, (uint8_t)0x8e);

	//Setup entry 9 as a coprocessor segment overrun
	initIDTEntry(&idt[9], (uint32_t)coprocessorSegmentOverun, (uint16_t)8, (uint8_t)0x8e);
	
	//Setup entry 10 to the generic handler
	initIDTEntry(&idt[10], (uint32_t)genericException, (uint16_t)8, (uint8_t)0x8e);
	
	//Setup entry 11 to a segment not present
	initIDTEntry(&idt[11], (uint32_t)segmentNotPresent, (uint16_t)8, (uint8_t)0x8e);
	
	//Setup entry 12 to a stack exception
	initIDTEntry(&idt[12], (uint32_t)stackException, (uint16_t)8, (uint8_t)0x8e);
	
	//Setup entry 13 to a general protection fault
	initIDTEntry(&idt[13], (uint32_t)generalProtectionFault, (uint16_t)8, (uint8_t)0x8e);
	
	//Setup entry 14 to a page fault
	initIDTEntry(&idt[14], (uint32_t)pageFault, (uint16_t)8, (uint8_t)0x8e);
	
	//Setup entry 15 to the generic handler
	initIDTEntry(&idt[15], (uint32_t)genericException, (uint16_t)8, (uint8_t)0x8e);
	
	//Setup entry 16 to a floating point error
	initIDTEntry(&idt[16], (uint32_t)floatingPointError, (uint16_t)8, (uint8_t)0x8e);
		
	//Setup interrupts 17-31 to point to the generic handler
	for(i = 17; i < 32; i++)
	{
		initIDTEntry(&idt[i], (uint32_t)genericException, (uint16_t)8, (uint8_t)0x8e);
	}
	
	//Setup entry 32 to point to schedule
	initIDTEntry(&idt[32], (uint32_t)schedule, (uint16_t)8, (uint8_t)0x8e);

	//Setup entry 33 to point to keyboardISR
	initIDTEntry(&idt[33], (uint32_t)keyboardISR, (uint16_t)8, (uint8_t)0x8e);

	//Setup all the other interrupts to 0
	for(i = 34; i < 255; i++)
	{
		//Set the idt entry to 0
		initIDTEntry(&idt[i], (uint32_t)0, (uint16_t)0, (uint8_t)0);
	}
			
	//Prepare the idt information
	idtr.idtBase = (uint32_t)idt;
	idtr.idtLimit = (uint32_t)(sizeof(idt) - 1);

	//Load the interrupt descriptor tables
	loadIdtr(&idtr);
}

/**
* Function to setup an IDT entry.
* Preconditions: none
* Postconditions: The entry is setup.
*/
void initIDTEntry(idt_entry_t* entry, uint32_t base, uint16_t selector, uint8_t access)
{
	//Get the low 16 bits of the base address
	entry->baseLow16 = (base & 0xffff);

	//Set the selector for the idt entry
	entry->selector = selector;

	//Set the always0 portion of the idt
	entry->always0 = (uint8_t)0;

	//Set the access portion of the idt
	entry->access = access;

	//Set the high 16 bits of the base address
	entry->baseHigh16 = (base >> 16) & 0xffff;
}

/**
* Function to setup the Programmable Interrupt Controller.
* Preconditions: none
* Postconditions: The Programmable Interrupt Controller is setup.
*/
void setupPIC()
{
	//Start 8259 master initialization
	outportb(0x20, 0x11);

	//Start 8259 slave initialization
	outportb(0xA0, 0x11);

	//Set master base interrupt vector (idt 32-38)
	outportb(0x21, 0x20);

	//Set slave base interrupt vector (idt 39-45)
	outportb(0xA1, 0x28);

	//Set cascade
	outportb(0x21, 0x04);

	//Turn on irq2
	outportb(0xA1, 0x02);

	//Finish 8259 initialization
	outportb(0x21, 0x01);

	//Finish 8259 initialization
	outportb(0xA1, 0x01);

	//Finish 8259 initialization
	outportb(0x21, 0x0);

	//Finish 8259 initialization
	outportb(0xA1, 0x0);

	//Turn on clock IRQ
	outportb(0x21, 0xfe); 
	
	//Turn on the keyboard
	outportb(0x21, (inportb(0x21) & 0xfd));

	//Turn off all others
	outportb(0xa1, 0xff);
}

/**
* Routine to handle a keyboard interrupt.
* Preconditions: none
* Postconditions: The keyboard interrupt is handled.
*/
void keyboardISR()
{
	//Handle the interrupt
	uint8_t scancode = inportb(0x60);
	
	switch(scancode)
	{
		//A key
		case 0x1e:
			//Place the key into the buffer
			insertKey(&keyboard, 'a');
			
			//Let the system know that a key press has occurred
			keypressed = 1;
			
			//Display the key on the screen
			//displayBuffer(&keyboard, conCurRow, conCurCol, 0x0f);
			
			//Increment the current column
			//conCurCol++;
			
			//Break
			break;
		
		//B key
		case 0x30:
			//Place the key into the buffer
			insertKey(&keyboard, 'b');
			
			//Display the key on the screen
			displayBuffer(&keyboard, conCurRow, conCurCol, 0x0f);
			
			//Increment the current column
			conCurCol++;
			
			//Break
			break;
		
		//C key
		case 0x2e:
			//Place the key into the buffer
			insertKey(&keyboard, 'c');
			
			//Display the key on the screen
			displayBuffer(&keyboard, conCurRow, conCurCol, 0x0f);
			
			//Increment the current column
			conCurCol++;
			
			//Break
			break;
			
		//Enter key
		case 0x1c:
			//Set enter pressed to 1
			//keyboard.enterPressed = 1;
			startReboot();
			
			/************************Parse the buffer**********/
			
			//Clear the buffer
			//clearKeyBuffer(&keyboard);
			
			//Break
			break;
	}
	
	//Tell the system to start handling interrups again
	outportb(0x20, 0x20);
	
	//Turn interrupts back on
	asm("sti");
}

/**
* Function to display graphics.
* Preconditons: none
* Postconditions: A graphic is shown.
*/
void displayGraph(int scr[][81])
{
	//Create 2 counters
	int row = 0;
	int col = 0;
	
	//Display the graphic
	for(row = 0; row < 25; row++)
	{
		for(col = 0; col < 80; col++)
		{
			//Write the info to the screen
			pmWriteScr(" ", row, col, scr[row][col]);
		}
	}
}

/**
* Function to stop all processes and reboot the system.
* Preconditions: none
* Postconditions: The system is rebooted.
*/
void startReboot()
{
	//Clear the screen
	postGdtClearScr();

	//Output a message
	pmWriteScr("System Rebooting....", 1, 0, 0x04);
	int i, j;
	//For loop for displaying message
	for(i = -9999; i < 9999; i++)
	{
		for(j = 0; j < 999; j++)
		{}
	}
	//Reboot
	reboot();
}

/**
* Function to convert a number to characters.
* Preconditions: none
* Postconditions: A number is converted to a character.
*/
char * itoa( uint32_t value, char * str, int base )
{
    char * rc;
    char * ptr;
    char * low;
    // Check for supported base.
    if ( base < 2 || base > 36 )
    {
        *str = '\0';
        return str;
    }
    rc = ptr = str;
    // Set '-' for negative decimals.
    if ( value < 0 && base == 10 )
    {
        *ptr++ = '-';
    }
    // Remember where the numbers start.
    low = ptr;
    // The actual conversion.
    do
    {
        // Modulo is negative for negative value. This trick makes abs() unnecessary.
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
        value /= base;
    } while ( value );
    // Terminating the string.
    *ptr-- = '\0';
    // Invert the numbers.
    while ( low < ptr )
    {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    return rc;
}
