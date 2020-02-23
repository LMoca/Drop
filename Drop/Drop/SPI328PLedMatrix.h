#ifndef SPILedMatrix_H
#define SPILedMatrix_H


#include <avr/io.h>
#include <util/delay.h>

// Initializing LED Matrix Module
extern void initLEDMatrix(void);
extern void clearDisplay(void);
extern void writeMax(unsigned char address, unsigned char data);

#endif