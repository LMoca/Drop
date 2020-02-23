/*
 * Drop.c
 *
 *	Created: 2/21/2020 11:12:56 PM
 *
 *	Author : Luigi Moca
 *
 *	Description: 
 *
 */ 
#define F_CPU 16000000UL  // 16 MHz
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

#include <stdio.h>

#include "SPI328P.h"
#include "SPI328PLedMatrix.h"

// Initializing Interrupt Functions
void initINT(void);

// Game functions
uint8_t *setDrop();
void congrats(void);
void gameOver(void);
uint8_t checkPath(uint8_t, uint8_t);

uint8_t player = 0x08;

int main(void)
{
	initINT();					// Initializing interrupts
	sei();						// Initializing global interrupts
	
	SPIMasterInit();			// Initializing SPI Master
	initLEDMatrix();			// Initializing LED Matrix
	
	clearDisplay();				// Clears the display
	
	uint8_t *obstacle;
	uint8_t obstagral = 0, RNG = 0;
	
	uint8_t gameStarted = 0;
	
    while (1)
    {
		// Generate beginning obstacles
		if(gameStarted == 0)
		{
			obstacle = setDrop();
		}
		gameStarted = 1;
		if(gameStarted == 1)
		{
			//gameStarted = 1;
			// Shift obstacles down the player's path
			for(uint8_t i = 1; i <= 7; i++)
			{
				// Shift the path down the player
				obstacle[i-1] = obstacle[i];
				if(i == 1)	writeMax(0x01, (obstacle[i-1] | player));
				else		writeMax(i, obstacle[i-1]);
			}
			
			// If player collides onto an obstacle, its game over
			if(player & obstacle[0])
			{
				gameStarted = 0;
				gameOver();
			}
			
			if(gameStarted == 1)
			{
				// Continue making paths at the last obstacle index
				obstagral = ~(obstacle[7]);
			
				RNG = rand () % 2 + 0; // Either 1 or 2
				// Determine whether to shift obstacles left or right
				// Also keeps the generated paths playable
				if(RNG == 1 && obstagral != 0x80)
				{
					// Shift obstagral to the left by 1
					obstagral = obstagral << 1;
					// Check for validity of generated path
					obstagral = checkPath(obstagral, RNG);
				}
				else if(obstagral != 0x01)
				{
					// Shift obstagral to the right by 1
					obstagral = obstagral >> 1;
					// Check for validity of generated path
					obstagral = checkPath(obstagral, RNG);
				}
				obstagral ^= 0xFF;
			
				obstacle[7] = obstagral;
				writeMax(0x08, obstacle[7]);
			}
		}
		_delay_ms(250);
	}
}

/*
*	Function initializes interrupts.
*/
void initINT()
{
	/* Using PORTB PCINT0 & PCINT1 for input interrupts
	DDRB = (0 << PB0) | (0 << PB1);		// PB0 & PB1 as inputs
	PORTB = (1 << PB0) | (1 << PB1);	// Enabling pull-up resistors
	*/
	// using PORTD INT pins
	DDRD = (0 << PD2) | (0 << PD3);		// INT0 & INT1 as inputs
	PORTD = (1 << PD2) | (1 << PD3);	// Enable pull-up resistors
	
	EIMSK = 0x03;						// Enabling INT0 & INT1
	EICRA = 0x0F;						// Detecting rising edge
	/*
	PCICR = 0x01;							// Enabling interrupts [7:0]
	PCMSK0 = (1 << PCINT1) | (1 << PCINT0);	// Selecting PCINT0 & PCINT1
	*/
}

/*
*	Stop Button Interrupt
*/
ISR(INT0_vect)
{
	if(player != 0x01)	player >>= 1;
	else				player = 0x01;
	_delay_ms(50);
}

/*
*	Reset Button Interrupt
*/
ISR(INT1_vect)
{
	if(player != 0x80)	player <<= 1;
	else				player = 0x80;
	_delay_ms(50);
	while(PIND & 0x08){};
}

/*
*	Function sets up the game Drop
*	Returns path array.
*/
uint8_t *setDrop()
{
	
	static uint8_t obstacle[8] = {0,0,0,0,0,0,0,0};
	uint8_t obstagral = 0, RNG = 0;
	
	// Reference player position to generate a fair path
	obstagral = player | (player<<1) | (player>>1);
	obstagral ^= 0xFF;
	obstacle[0] = obstagral | player;
	writeMax(0x01, obstacle[0]);
	
	for(uint8_t i = 1; i <= 7; i++)
	{
		obstagral ^= 0xFF;
		RNG = rand () % 2 + 0; // Either 1 or 2
		// Determine whether to shift obstacles left or right
		if(RNG == 1)	obstagral = obstagral << 1;
		else			obstagral = obstagral >> 1;
		
		obstagral = checkPath(obstagral, RNG);
		
		obstagral ^= 0xFF;
		
		obstacle[i] = obstagral /*| player*/;
		
		writeMax(i+1, obstacle[i]);
		_delay_ms(300);
	}
		
	return obstacle;
}

/*
*	Function makes sure the 
*	generated path is valid.
*	Passing Parameter:
*	uint8_t validPath - 
*/
uint8_t checkPath(uint8_t validPath, uint8_t RNG)
{
	// Bits have shifted to the left
	if(RNG == 1)
	{
		switch(validPath)
		{
			case 0x00:
				validPath |= 0x01;
			break;
			case 0x01:
				validPath = 0x80;
			break;
			case 0x02:
				validPath |= 0x01;
			break;
			case 0x06:
				validPath |= 0x01;
			break;
			case 0x80:
				validPath |= 0x80;
			break;
			case  0x40:
				validPath = 0xC0;
			break;
		}
	}
	// Bits have shifted to the right
	else
	{
		switch(validPath)
		{
			case 0x00:
				validPath |= 0x80;
			break;
			case 0x40:
				validPath = 0xC0;
			break;
			case 0x01:
				validPath |= 0x01;
			break;
			case 0x60:
				validPath |= 0x80;
			break;
			case 0x80:
				validPath = 0xE0;
			break;
		}
	}
	// Returns the valid path
	return validPath;
}
/*
*	Function executes when player wins the game.
*/
void congrats()
{
	uint8_t y = 0, l = 0;
	
	uint8_t image_1[8] = {0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00};
	uint8_t image_2[8] = {0x00, 0x00, 0x3C, 0x24, 0x24, 0x3C, 0x00, 0x00};
	uint8_t image_3[8] = {0x00, 0x7E, 0x42, 0x5A, 0x5A, 0x42, 0x7E, 0x00};
	uint8_t image_4[8] = {0xFF, 0x81, 0xBD, 0xA5, 0xA5, 0xBD, 0x81, 0xFF};
	
	for(y = 8; y > 0; --y)
	{
		writeMax(y, image_1[y-1]);
	}
	_delay_ms(600);
	for(y = 8; y > 0; --y)
	{
		writeMax(y, image_2[y-1]);
	}
	_delay_ms(600);
	for(y = 8; y > 0; --y)
	{
		writeMax(y, image_3[y-1]);
	}
	_delay_ms(600);
	for(y = 8; y > 0; --y)
	{
		writeMax(y, image_4[y-1]);
	}
	_delay_ms(600);
	
	while(l < 3)
	{
		for(y = 8; y > 0; --y)
		{
			writeMax(y, image_3[y-1]);
		}
		_delay_ms(600);
		for(y = 8; y > 0; --y)
		{
			writeMax(y, image_4[y-1]);
		}
		_delay_ms(600);
		l++;
	}
	clearDisplay();  //Clears display
}

/*
	Function executes when the player loses the game.
*/
void gameOver(void)
{
	//resetPress = 0;
	// Array displays an X on display
	uint8_t XImage[8] = {0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81};
	
	for(uint8_t k = 0; k < 5; k++)
	{
		if(/*!resetPress*/1)
		{
			for(uint8_t z = 8; z > 0; --z)
			{
				writeMax(z, XImage[z-1]);
			}
			_delay_ms(500);
			clearDisplay();
			_delay_ms(500);
		}
	}
}