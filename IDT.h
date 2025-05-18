/**
* File Name: IDT.h
* Created by: Eric Hooks
* Date of Creation: 05/06/2011
* Purpose: To hold the structure for the Interrupt Descriptor Table
*/

//Included Libraries
#include "Definitions.h"

//Structure for the interrupt descriptor table
struct idt_entry
{
	//Variable for the low 16 bits of the base
	uint16_t baseLow16;

	//Variable for the code selector for the interrupt
	uint16_t selector;

	//Variable for the always 0 part of the idt
	uint8_t always0;

	//Variable for the access part of the idt
	uint8_t access;

	//Variable for the high 16 bits of the idt
	uint16_t baseHigh16;
} __attribute__((packed));

//Structure that holds the idt's information
struct idt_info
{
	//Variable for the limit of the idt
	uint16_t idtLimit;

	//Variable for the base of the idt
	uint32_t idtBase;
} __attribute__((packed));

//Function Prototypes for the Interrupt Descriptor table
void initIDTEntry(idt_entry_t* entry, uint32_t base, uint16_t selector, uint8_t access); //Function to setup an Interrupt descriptor
void initIDT(); //Fucntion to setup the interrupt descriptor table