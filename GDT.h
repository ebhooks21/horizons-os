/**
* File Name: GDT.h
* Created by: Eric Hooks
* Date of Creation: 05/06/2011
* Purpose: To be the global descriptor table.
*/

//Included Libraries
#include "Definitions.h"

//Structure for the GDT(Global Descriptor Table)
struct gdt_entry
{
    //Variable for segment limit
    uint16_t segLimit;
    
    //Variable for segment base
    uint16_t segBase;
    
    //Variable for the middle 8 bits of the base segment
    uint8_t baseMid8;

    //Variable for the access portion of the limit
    uint8_t access;

    //Variable for the attributes part of the base
    uint8_t attributes;

    //Variable for the upper 8 bits of the base
    uint8_t baseUpper8;
} __attribute__((packed));

//Structure that holds the gdt's information
struct gdt_info
{
    //Variable to hold the base address of the gdt
    uint16_t gdtLimit;
    
    //Variable to hold the limit address of the gdt
    uint32_t gdtBase;
} __attribute__((packed));

//Function prototypes for the GDT
void initGDTEntry(gdt_entry_t* entry, uint32_t baseAddress, uint32_t limit, uint8_t access, uint8_t attributes); //Function to setup a descriptor
void loadSel(int cs, int ds, int ss, int gs, int es); //Function to load the selectors with the descriptors

