/**
* File Name: Definitions.h
* Created by: Eric Hooks
* Date of creation: 02/16/2011
* Purpose: To group all of the defines for an operating system together.
*/

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

//Define a lowercase d
#define charlowd 0x20

//Define a lowercase e
#define charlowe 0x12

//Define a lowercase f
#define charlowf 0x21

//Define a lowercase g
#define charlowg 0x22

//Define uint8_t
typedef unsigned char uint8_t;

//Define uint16_t
typedef unsigned short uint16_t;

//Define uint32_t
typedef unsigned int uint32_t;

//Define gdt_entry_t
typedef struct gdt_entry gdt_entry_t;

//Define gdt_info_t
typedef struct gdt_info gdt_info_t;

//Define PCB
typedef struct ProcessControlBlock pcb;

//Define idt_entry_t
typedef struct idt_entry idt_entry_t;

//Define idt_info_t
typedef struct idt_info idt_info_t;

//Define keyboard_t
typedef struct Keyboard keyboard_t;

//Define system_t
typedef struct System system_t;

/******Assembly Functions******/
void writeScr(char* string, int row, int col); //Function to write to the screen
void pmWriteScr(char* string, int row, int col, int color); //Function to write to the screen after setting up the gdt
void loadGdtr(gdt_info_t* gdtReg); //Function to load the gdt
void loadSel(int cs, int ds, int ss , int gs, int es); //Function to load the selector registers
void schedule(); //Function to change processes
void go(); //Function to start the first process
void loadIdtr(idt_info_t* idtReg); //Function to load the idt
void reboot(); //Make the system reboot


/******Main C Functions********/
//Function Prototypes
void clearScr(); //Function written in C to clear the screen
void idle(); //Function to be an idle process
int createProcess(uint32_t ds, uint32_t ss, uint32_t stackTop, uint32_t cs, uint32_t processEntry); //Function to create processes
void postGdtClearScr(); //Function to clear the screen after the gdt has been setup
uint32_t allocStack(); //Function to allocate the stack
pcb* allocPCB(); //Function to allocate a pcb
void enqueue(pcb* p); //Function to enqueue a pcb
pcb* dequeue(); //Function to dequeue a pcb
void divByZero(); //Function for divide by zero excpetion
void debugException(); //Function to handle debug exceptions
void genericException(); //Function to handle a generic exception
void overflowException(); //Function to handle a overflow exception
void boundException(); //Function to handle a bound exception
void invalidOpcode(); //Function to handle an invalid opcode
void fpuUnavailable(); //Function to handle if the fpu is unavailable
void doubleFault(); //Function to handle a double fault
void coprocessorSegmentOverun(); //Function to handle a coprocessor segment overrrun
void segmentNotPresent(); //Function to handle a segment not being present
void stackException(); //Function to handle a stack exception
void generalProtectionFault(); //Function to handle a general protection fault
void pageFault(); //Function to handle a page fault
void floatingPointError(); //Function to handle a floating point error
void setupPIC(); //Function to setup the Programmable Interrupt Controller
void outportb(uint16_t port, uint8_t data); //Function to write to a port
uint8_t inportb(uint16_t port); //Function to read from a port
void prompt(); //Function to setup a prompt
void startReboot(); //Function to stop all processes and reboot the system
char* itoa(uint32_t value, char* str, int base); //Convert a number to characters to be printed

/*******Global Variables for all****************/
keyboard_t* kb; //The Keyboard
#endif