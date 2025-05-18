/**
* File Name: System.c
* Created by: Eric Hooks
* Date of creation: 05/08/2010
* Purpose: To hold functions for the system.
*/

//Included Libaries
#include "Definitions.h"
#include "System.h"

/**
* Function to setup the system.
* Preconditions: none
* Postconditions: The system is setup.
*/
void init_system(system_t *s)
{
	//Set the number of keys
	s->ramB = detectRam();
}

/**
* Function to detect the amount of ram the system has.
* Preconditions: none
* Postconditions: The amount of ram on the system is detected.
*/
uint32_t detectRam()
{
    //Setup a value for the total
    unsigned short total;
	
    //Setup values for low and high memory
    unsigned char lowmem, highmem;
 
    //Write to port 0x70
    outportb(0x70, 0x30);
	
    //Read from port 0x71
    lowmem = inportb(0x71);
	
    //Write to port 0x70
    outportb(0x70, 0x31);
	  
    //Read from port 0x71
    highmem = inportb(0x71);
 
    //Get the total amount of ram
    total = lowmem | highmem << 8;
	
    //Return the total amount of ram
    return total;
}

/**
* Function to return amount of usable ram in MB, given in KB
*/
uint32_t getRamMB(uint32_t ramKB)
{
	return ramKB / 1024;
}
