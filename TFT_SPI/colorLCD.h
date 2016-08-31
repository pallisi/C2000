/*
 * colorLCD.h
 *
 * ported to C2000 Launchpad
 * this uses hardware SPI
 * tested with RobG's 2.2" lcd with touch
 *
 *  Created on: 12/30 2012
 *      Author: RobG / Timotet
 */

#ifndef C2K_TFT_SPI_COLORLCD_H_
#define C2K_TFT_SPI_COLORLCD_H_

#include "DSP28x_Project.h" // Device Header file and Examples Include File
#include "f2802x_common/include/gpio.h"
#include "f2802x_common/include/spi.h"

//project includes
#include "typedefs.h"

#define ILI9340 // 2.2" with touch

#if defined ILI9340
//
	#define LONG_EDGE_PIXELS 320
	#define SHORT_EDGE_PIXELS 240
	#define LCD_OFFSET_HEIGHT 0
	#define LCD_OFFSET_WIDTH 0
//
#endif


// GPIO defines for hardware SPI
//sclk
#define LCD_SCLK_PIN    GPIO_Number_18     //booster pack connects to pin  J1_7 on c2000 LP board
//miso for C2000
#define LCD_MISO_PIN    GPIO_Number_16     //booster pack connects to pin  J2_6 on c2000 LP board


//mosi for C2000      This is way faster than software chip select!!!
#define LCD_MOSI_PIN    GPIO_Number_17      // booster pack connects to pin J2_7 on c2000 LP board
#define SPISTE_PIN      GPIO_Number_19      // hardware slave select CE pin J2_2 on C2000 LP board


#define LCD_SCE_PIN       GPIO_Number_6   // gpio for software chip select
#define LCD_SELECT        GPIO_setLow(myGpio, LCD_SCE_PIN)  //set SCE low
#define LCD_DESELECT      GPIO_setHigh(myGpio, LCD_SCE_PIN) //set SCE high


#define LCD_DC_PIN        GPIO_Number_34    // gpio for DC test
#define LCD_DC_LO         GPIO_setLow(myGpio, LCD_DC_PIN)  //set DC low
#define LCD_DC_HI         GPIO_setHigh(myGpio, LCD_DC_PIN) //set DC high

// font sizes
#define FONT_SM		0
#define FONT_MD		1
#define FONT_LG		2
#define FONT_SM_BKG	3
#define FONT_MD_BKG	4
#define FONT_LG_BKG	5

void setOrientation(u_char orientation);
void initLCD();
//
void setColor(u_int color);
void setBackgroundColor(u_int color);
//
void clearScreen(u_char blackWhite);
void drawPixel(u_int x, u_int y);
void drawString(u_int x, u_int y, char type, char *string);
void drawCharLg(u_int x, u_int y, char c);
void drawCharMd(u_int x, u_int y, char c);
void drawCharSm(u_int x, u_int y, char c);
void drawCharLgBkg(u_int x, u_int y, char c);
void drawCharMdBkg(u_int x, u_int y, char c);
void drawCharSmBkg(u_int x, u_int y, char c);
void drawLine(u_int xStart, u_int yStart, u_int xEnd, u_int yEnd);
void drawRect(u_int xStart, u_int yStart, u_int xEnd, u_int yEnd);
void drawCircle(u_int x, u_int y, u_char radius);
//
void fillRect(u_int xStart, u_int yStart, u_int xEnd, u_int yEnd);
void fillCircle(u_int x, u_int y, u_char radius);
//
void drawLogicLine(u_int x, u_int y, u_int length, u_char height, u_char * data);
//
void drawImage(u_int x, u_int y, u_int w, u_int h, u_int * data);
void drawImageLut(u_int x, u_int y, u_int w, u_int h, u_char * data, u_int * lut);
void drawImageMono(u_int x, u_int y, u_int w, u_int h, u_char * data);
//


#define COMMAND 0
#define DATA 1
// orientation
#define ORIENTATION_VERTICAL 0
#define ORIENTATION_HORIZONTAL 1
#define ORIENTATION_VERTICAL_ROTATED 2
#define ORIENTATION_HORIZONTAL_ROTATED 3

// driver specific
#define SWRESET		0x01
#define	BSTRON		0x03
#define RDDIDIF		0x04
#define RDDST		0x09
#define SLEEPIN     0x10
#define	SLEEPOUT	0x11
#define	NORON		0x13
#define	INVOFF		0x20
#define INVON      	0x21
#define	SETCON		0x25
#define DISPOFF     0x28
#define DISPON      0x29
#define CASETP      0x2A
#define PASETP      0x2B
#define RAMWRP      0x2C
#define RGBSET	    0x2D
#define	MADCTL		0x36
#define SEP			0x37
#define	COLMOD		0x3A
#define DISCTR      0xB9
#define DOR			0xBA
#define	EC			0xC0
#define RDID1		0xDA
#define RDID2		0xDB
#define RDID3		0xDC

#define SETOSC		0xB0
#define SETPWCTR4	0xB4
#define SETPWCTR5	0xB5
#define SETEXTCMD	0xC1
#define SETGAMMAP	0xC2
#define SETGAMMAN	0xC3

// ILI9340 specific
#define ILIGS			0x26
#define ILIMAC			0x36
#define ILIFCNM			0xB1
#define ILIFCIM			0xB2
#define ILIFCPM			0xB3
#define ILIDFC			0xB6
#define ILIPC1			0xC0
#define ILIPC2			0xC1
#define ILIVC1			0xC5
#define ILIVC2			0xC7
#define PWRCTRLA		0xCB
#define PWRCTRLB		0xCF
#define RDID4			0xD3
#define GER4SPI			0xD9
#define ILIPGC			0xE0
#define ILINGC			0xE1
#define DTCTRLA1		0xE8
#define DTCTRLA2		0xE9
#define DTCTRLB			0xEA
#define POSC			0xED
#define ILIGFD			0xF2
#define PRC				0xF7

// colors
#define COLOR_16_BLACK   		0x0000
#define COLOR_16_BLUE    		0x001F
#define COLOR_16_RED     		0xF800
#define COLOR_16_GREEN   		0x07E0
#define COLOR_16_CYAN    		0x07FF
#define COLOR_16_MAGENTA 		0xF81F
#define COLOR_16_YELLOW  		0xFFE0
#define COLOR_16_WHITE   		0xFFFF
//red
//orange
//blue
//green
#define COLOR_16_ORANGE			0xFD20
#define COLOR_16_ORANGE_RED		0xFA20
#define COLOR_16_DARK_ORANGE	0xFC60
#define COLOR_16_GRAY			0xBDF7
#define COLOR_16_NAVY			0x0010
#define COLOR_16_ROYAL_BLUE		0x435C
#define COLOR_16_SKY_BLUE		0x867D
#define COLOR_16_TURQUOISE		0x471A
#define COLOR_16_STEEL_BLUE		0x4416
#define COLOR_16_LIGHT_BLUE		0xAEDC
#define COLOR_16_AQUAMARINE		0x7FFA
#define COLOR_16_DARK_GREEN		0x0320
#define COLOR_16_DARK_OLIVE_GREEN	0x5345
#define COLOR_16_SEA_GREEN		0x2C4A
#define COLOR_16_SPRING_GREEN	0x07EF
#define COLOR_16_PALE_GREEN		0x9FD3
#define COLOR_16_GREEN_YELLOW	0xAFE5
#define COLOR_16_LIME_GREEN		0x3666
#define COLOR_16_FOREST_GREEN	0x2444
#define COLOR_16_KHAKI			0xF731
#define COLOR_16_GOLD			0xFEA0
#define COLOR_16_GOLDENROD		0xDD24
#define COLOR_16_SIENNA			0xA285
#define COLOR_16_BEIGE			0xF7BB
#define COLOR_16_TAN			0xD5B1
#define COLOR_16_BROWN			0xA145
#define COLOR_16_CHOCOLATE		0xD343
#define COLOR_16_FIREBRICK		0xB104
#define COLOR_16_HOT_PINK		0xFB56
#define COLOR_16_PINK			0xFE19
#define COLOR_16_DEEP			0xF8B2 //? DEEP what? PINK?
#define COLOR_16_VIOLET			0xEC1D
#define COLOR_16_DARK_VIOLET	0x901A
#define COLOR_16_PURPLE			0xA11E
#define COLOR_16_MEDIUM_PURPLE	0x939B

#endif /* C2K_TFT_SPI_COLORLCD_H_ */

