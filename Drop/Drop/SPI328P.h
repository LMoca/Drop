#ifndef SPI328P_H
#define	SPI328P_H

//Defines for SPI communication
#define DIN  PORTB3
#define SCK  PORTB5
#define CS   PORTB2

#include <avr/io.h>

// Initializing SPI Communication
extern void SPIMasterInit(void);
extern void sendData(unsigned char data);
extern void clearDisplay(void);

#endif