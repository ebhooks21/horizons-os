/**
* File Name: interrupts.c
* Created by: Eric Hooks
* Date of Creation: 03/23/2011
* Purpose: To hold the functions that are used to handle interrupts.
*/
#include "Definitions.h"
#include "ConGraph.h"

/**
* Routine to handle divide by zero interrupts.
* Preconditions: none
* Postconditions: A Divide by zero interrupt is handled.
*/
void divByZero()
{
	//Write to the screen that a divide by zero interrupt has occured.
	displayGraph(errorScr);
	pmWriteScr("Divide by Zero Exception Occured.", 12, 25, 42);
	
	//Loop forever
	while(1);
}

/**
* Routine to handle Debug Exceptions.
* Preconditions: none
* Postconditions: A debug exception is handled.
*/
void debugException()
{
	//Write to the screen that a debug exception has occured.
	displayGraph(errorScr);
	pmWriteScr("A Debug exception occured.", 16, 0, 02);

	//Loop forever
	while(1);
}

/**
* Routine to handle generic exceptions.
* Preconditions: none
* Postconditions: A generic exception is handled.
*/
void genericException()
{
	//Write a generic message to the screen
	displayGraph(errorScr);
	pmWriteScr("An exception occured.", 16, 0, 02);

	//Loop forever
	while(1);
}

/**
* Routine to handle overflow exceptions.
* Preconditions: none
* Postconditions: An overflow exception is handled.
*/
void overflowException()
{
	//Write to the screen that an overflow exception has occured
	displayGraph(errorScr);
	pmWriteScr("An overflow exception occured.", 16, 0, 02);

	//Loop forever
	while(1);
}

/**
* Routine to handle Bound exceptions.
* Preconditions: none
* Postconditions: A bound exception is handled.
*/
void boundException()
{
	//Write to the screen that a bound exeption has occured.
	displayGraph(errorScr);
	pmWriteScr("A bound exception occured.", 16, 0, 02);

	//Loop forever
	while(1);
}

/**
* Routine to handle an invalid opcode.
* Preconditions: none
* Postconditions: An invalid opcode is handled.
*/
void invalidOpcode()
{
	//Write to the screen that an ivalid opcode has been encountered
	displayGraph(errorScr);
	pmWriteScr("Invalid Opcode.", 16, 0, 02);

	//Loop forever
	while(1);
}

/**
* Routine to handle the FPU being unavailable.
* Preconditions: none
* Postconditions: FPU being unavailable is handled.
*/
void fpuUnavailable()
{
	//Write to the screen that the fpu is unavailable
	displayGraph(errorScr);
	pmWriteScr("The Floating Point Unit is unavailable.", 16, 0, 02);

	//Loop forever
	while(1);
}

/**
* Routine to handle a double fault.
* Preconditions: none
* Postconditions: A double fault is handled.
*/
void doubleFault()
{
    //Write to the screen that a double fault has occured.
	displayGraph(errorScr);
    pmWriteScr("A Double Fault Occured.", 16, 0, 02);
    
    //Loop forever
    while(1);
}

/**
* Routine to handle a coprocessor segment overrun.
* Preconditions: none
* Postconditions: The interrupt is handled.
*/
void coprocessorSegmentOverun()
{
    //Write to the screen that the interrupt has occured
	displayGraph(errorScr);
    pmWriteScr("A coprocessor segment overrun has occured.", 16, 0, 02);
    
    //Loop forever
    while(1);
}

/**
* Routine to handle a segemnent not present exeption.
* Preconditions: none
* Postconditions: A segment not present exception is handled.
*/
void segmentNotPresent()
{
    //Write to the screen that a segment is not present
	displayGraph(errorScr);
    pmWriteScr("Segment not present.", 16, 0, 02);
    
    //Loop forever
    while(1);
}

/**
* Routine to handle a stack exception.
* Preconditions: none
* Postconditions: A stack exception is handled.
*/
void stackException()
{
    //Write to the screen that a segment is not present
	displayGraph(errorScr);
    pmWriteScr("A stack exception has occured.", 16, 0, 02);
    
    //Loop forever
    while(1);
}

/**
* Routine to handle a general protection fault.
* Preconditions: none
* Postconditions: A general protection fault is handled.
*/
void generalProtectionFault()
{
    //Write to the screen that a segment is not present
	displayGraph(errorScr);
    pmWriteScr("A General Protection Fault has occured.", 16, 0, 02);
    
    //Loop forever
    while(1);
}

/**
* Routine to handle a page fault.
* Preconditions: none
* Postconditions: A page fault is handled.
*/
void pageFault()
{
    //Write to the screen that a segment is not present
	displayGraph(errorScr);
    pmWriteScr("A Page Fault has occured.", 16, 0, 02);
    
    //Loop forever
    while(1);
}

/**
* Routine to handle a floating point error.
* Preconditions: none
* Postconditions: A floating point error is handled.
*/
void floatingPointError()
{
    //Write to the screen that a segment is not present
    pmWriteScr("A Floating Point Error has occured.", 16, 0, 02);
    
    //Loop forever
    while(1);
}


