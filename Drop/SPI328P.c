//Defines for SPI communication
#define DIN  PORTB3
#define SCK  PORTB5
#define CS   PORTB2

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

// Initializing SPI Communication
void SPIMasterInit(void);
void sendData(unsigned char data);

void SPIMasterInit()
{
	//PORTB is the port where SPI peripheral is located
	// PB2 - DIN (MOSI)
	// PB1 - CLK (SCK)
	// PB0 - CS  (SS)
	DDRB = (1<<DIN) | (1<<SCK) | (1<<CS);  // MOSI, SCK, & CS

	//enable SPI interrupt, enable SPI, Master Select, MSB transmitted first, F_CPU/16
	SPCR = /*(1<< SPIE) |*/ (1<<SPE) | (1<<MSTR) | (0<<DORD) | (0<<SPR1) | (1<<SPR0);
	SPSR = 0x00;
}

// Because the LED matrix has 16-bits on its register,
// the SPDR must be able to shift the bits to account for it.
void sendData(unsigned char data)
{
	//load the first 8-bit data to buffer
	SPDR = data;
	while(!(SPSR & (1<<SPIF)));
}