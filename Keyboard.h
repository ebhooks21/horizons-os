/**
* File Name: Keyboard.h
* Created by: Eric Hooks
* Date of creation: 05/08/2011
* Purpose: To hold a structure for the keyboard.
*/

//Included Libaries
#include "Definitions.h"

//Check to see if the library is already loaded
#ifndef KEYBOARD_H
#define KEYBOARD_H

//Keyboard Struct
struct Keyboard
{
	//Pointer to the keyboard object
	keyboard_t *kb;
	
	//Variable for the number of keys the keyboard has
	int numKeys;
	
	//Variable to hold if enter was pressed
	int enterPressed;
	
	//Variable for the number of characters currently in the buffer
	int buffSize;
	
	//Variable for the keyboard's buffer
	char buffer[4096];
};

//Function Prototypes
void init_keyboard(keyboard_t *k, int nKey); //Function to setup the keyboard
void clearKeyBuffer(keyboard_t *k); //Function to clear the keyboard's buffer
void insertKey(keyboard_t *k, char key); //Function to place a key into the keyboard's buffer
void parseBuffer(keyboard_t *k); //Function to parse the command given by the user
void backspace(keyboard_t *k); //Function for the backspace key
void displayBuffer(keyboard_t *k, int row, int col, int color); //Function to display the buffer
void keyboardISR(); //Function to manage the keyboard interrupt

#endif
