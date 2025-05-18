/**
* File Name: System.h
* Created by: Eric Hooks
* Date of creation: 05/28/2013
* Purpose: To hold a structure for system information.
*/

//Included Libaries
#include "Definitions.h"

//Check to see if the library is already loaded
#ifndef SYSTEM_H
#define SYSTEM_H

//System struct
struct System
{
	//Variable for the amount of system memory, in bytes
	uint32_t ramB;
};

//Function Prototypes
void init_system(system_t *s); //Function to setup the system
uint32_t detectRam(); //Function to detect the amount of ram the system has
uint32_t getRamMB(uint32_t ramKB); //Function to get ram in MB

#endif
