#define F_CPU 4000000UL  //4 MHz

//Addresses for MAX7219
#define DECODE	   0x09
#define INTENSITY  0x0A
#define SCAN_LIMIT 0x0B
#define NORMAL_OP  0x0C
#define TEST	   0x0F

//Defines for MAX instruction
#define LOADSTOP	   PORTB |= (1<<CS)
#define LOADSTART	   PORTB &= (1<<CS)
#define CLKHIGH		   PORTB |= (1<<SCK)
#define CLKLOW		   PORTB &= (1<<SCK)

#include <avr/io.h>
#include <util/delay.h>

#include "SPI328P.h"

void initLEDMatrix(void);
void clearDisplay(void);
void writeMax(unsigned char address, unsigned char data);

/*
*	Function initializes the 8x8 LED display
*/
void initLEDMatrix()
{
	//add,  hex
	writeMax(DECODE, 0x00);		 //decode mode
	writeMax(SCAN_LIMIT, 0x07);  //scan limit
	writeMax(NORMAL_OP, 0x01);   //Normal Operation
	writeMax(INTENSITY, 0x04);   //define intensity
	writeMax(TEST, 0x01);		 //display test
	_delay_ms(500);
	writeMax(TEST, 0x00);		 //display test
}

/*
*	Function writes to the 8x8 LED display.
*	Passing Parameters:
*	unsigned char address	-	designated address to write to.
*	unsigned char data		-	
*/
void writeMax(unsigned char address, unsigned char data)
{
	LOADSTART;  //CS is set LOW to start loading data
	CLKHIGH;  //set CLK HIGH for incoming register address
	//send address first
	sendData(address);
	CLKLOW;  //set CLK LOW

	CLKHIGH;  //set CLK HIGH for incoming register address
	//send data
	sendData(data);
	CLKLOW;  //set CLK LOW
	LOADSTOP;  //CS is set HIGH to end loading data
	_delay_ms(5);
}

/*
*	Function clears the 8x8 LED display
*/
void clearDisplay()
{
	for(uint8_t i = 0; i <= 8; i++)
		writeMax(i, 0x00);
}