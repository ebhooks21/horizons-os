/**
* File Name: Keyboard.c
* Created by: Eric Hooks
* Date of creation: 05/08/2010
* Purpose: To hold function for the keyboard.
*/

//Included Libaries
#include "Definitions.h"
#include "Keyboard.h"

/**
* Function to setup a keyboard.
* Preconditions: none
* Postconditions: A keyboard is setup.
*/
void init_keyboard(keyboard_t *k, int nKey)
{
	//Set the number of keys
	k->numKeys = nKey;
	
	//Set the buffer size
	k->buffSize = 0;
	
	//Set enterPressed
	k->enterPressed = 0;
	
	//Set the keyboard
	kb = k;

}

/**
* Function to clear the keyboard buffer.
* Preconditions: none
* Postconditions: The buffer is cleared.
*/
void clearKeyBuffer(keyboard_t *k)
{
	//Setup a counter
	int counter = 0;
	
	//Set enterPressed to 0
	k->enterPressed = 0;
	
	//Clear the buffer
	for(counter = 0; counter < k->buffSize; counter++)
	{
		//Clear a spot
		k->buffer[counter] = (int)" ";
	}
	
	//Set buffSize back to 0
	k->buffSize = 0;
}

/**
* Function to place a key into the buffer.
* Preconditions: none
* Postconditions: A key is placed into the buffer.
*/
void insertKey(keyboard_t *k, char key)
{
	//Place the key into the buffer
	k->buffer[k->buffSize] = key;
	
	//Increment the size of the buffer
	k->buffSize++;
}

/**
* Function to parse the buffer when enter has been hit.
* Preconditions: none
* Postconditions: The buffer is parsed and the approriate action is performed.
*/
void parseBuffer(keyboard_t *k)
{
	/**************Insert code to parse the buffer**********************/
	
	//Clear the buffer
	clearKeyBuffer(k);
}

/**
* Function to perform a backspace.
* Preconditions: none
* Postconditions: A backspace is performed if a character is there.
*/
void backspace(keyboard_t *k)
{
	//Make sure there is a key to erase
	if(k->buffSize > 0)
	{
		//Clear the last entered key
		k->buffer[k->buffSize - 1] = (int)" ";
		
		//Decrement the size of the buffer
		k->buffSize--;
	}
	
	else
	{
		//Do nothing
	}
}

/**
* Function to display the buffer on the screen.
* Preconditions: none
* Postconditions: The buffer is displayed to the screen.
*/
void displayBuffer(keyboard_t *k, int row, int col, int color)
{
	//Display the buffer
	pmWriteScr(&k->buffer[k->buffSize - 1], row, col, color);
}
