/*
 * colorLCD.c
 *
 * ported to C2000 Launchpad
 * this uses hardware SPI
 * tested with RobG's 2.2" lcd with touch
 *
 *  Created on: 12/30 2012
 *      Author: RobG / Timotet
 */

#include "colorLCD.h"
#include "fonts.h"


extern GPIO_Handle myGpio;
extern SPI_Handle mySpi;
#define ORIENTATION 0

u_char colorLowByte = 0;
u_char colorHighByte = 0;
u_char bgColorLowByte = 0;
u_char bgColorHighByte = 0;
//char fontSize = 0;
//const char fontSupport[3][3] = {{0,0,0},{0,0,0},{0,0,0}};
u_char _orientation = 0;


void writeData(u_char data) {

	LCD_SELECT;                 // set SCE low
	LCD_DC_HI;                  // set DC high = data select
	SPI_write8(mySpi, data);    // shift out 8 bits
}

void writeCommand(u_char data) {

	LCD_SELECT;                 // set SCE low
	LCD_DC_LO;                  // set DC low = command select
    SPI_write8(mySpi, data);    // shift out 8 bits
}

u_int getScreenWidth() {
	if (_orientation == 0 || _orientation == 2)
		return SHORT_EDGE_PIXELS;
	else
		return LONG_EDGE_PIXELS;
}

u_int getScreenHeight() {
	if (_orientation == 0 || _orientation == 2)
		return LONG_EDGE_PIXELS;
	else
		return SHORT_EDGE_PIXELS;
}

void setArea(u_int xStart, u_int yStart, u_int xEnd, u_int yEnd) {

	u_char yStartMSB = yStart >> 8;
	u_char yEndMSB = yEnd >> 8;
	u_char xStartMSB = xStart >> 8;
	u_char xEndMSB = xEnd >> 8;

	writeCommand(CASETP);

	writeData(xStartMSB);
	writeData(xStart);

	writeData(xEndMSB);
	writeData(xEnd);

	writeCommand(PASETP);

	writeData(yStartMSB);
	writeData(yStart);

	writeData(yEndMSB);
	writeData(yEnd);

	writeCommand(RAMWRP);
	// data to follow
}
//////////////////////
// color
//////////////////////

void setColor(u_int color) {
	colorLowByte = color;
	colorHighByte = color >> 8;
}

void setBackgroundColor(u_int color) {
	bgColorLowByte = color;
	bgColorHighByte = color >> 8;
}


/////////////////
// drawing
/////////////////

void clearScreen(u_char blackWhite) {
	setArea(0, 0, getScreenWidth() - 1, getScreenHeight() - 1);
	setBackgroundColor(blackWhite ? 0x0000 : 0xFFFF);
	u_int w = getScreenWidth();
	u_int h = getScreenHeight();
	while (h != 0) {
		while (w != 0) {
			writeData(bgColorHighByte);
			writeData(bgColorLowByte);
			w--;
		}
		w = getScreenWidth();
		h--;
	}
}

void drawPixel(u_int x, u_int y) {
	setArea(x, y, x, y);
	writeData(colorHighByte);
	writeData(colorLowByte);
}

/////////////////////////////
// Draw String - type: 0=Sm, 1=Md, 2=Lg, 3=Sm/Bkg, 4=Md/Bkg, 5=Lg/Bkg
/////////////////////////////
void drawString(u_int x, u_int y, char type, char *string) {
	u_int xs = x;
	switch (type) {
	case FONT_SM:
		while (*string) {
			drawCharSm(xs, y, *string++);
			xs += 6;
		}
		break;
	case FONT_MD:
		while (*string) {
			drawCharMd(xs, y, *string++);
			xs += 8;
		}
		break;
	case FONT_LG:
		while (*string) {
			drawCharLg(xs, y, *string++);
			xs += 12;
		}
		break;
	case FONT_SM_BKG:
		while (*string) {
			drawCharSmBkg(xs, y, *string++);
			xs += 6;
		}
		break;
	case FONT_MD_BKG:
		while (*string) {
			drawCharMdBkg(xs, y, *string++);
			xs += 8;
		}
		break;
	case FONT_LG_BKG:
		while (*string) {
			drawCharLgBkg(xs, y, *string++);
			xs += 12;
		}
		break;
	}
}

//////////////////////////
// 5x7 font - this function does not draw background pixels
//////////////////////////
void drawCharSm(u_int x, u_int y, char c) {
	u_char col = 0;
	u_char row = 0;
	u_char bit = 0x01;
	u_char oc = c - 0x20;
	while (row < 8) {
		while (col < 5) {
			if (font_5x7[oc][col] & bit)
				drawPixel(x + col, y + row);
			col++;
		}
		col = 0;
		bit <<= 1;
		row++;
	}
}

////////////////
// 5x7 font - this function draws background pixels
////////////////
void drawCharSmBkg(u_int x, u_int y, char c) {
	u_char col = 0;
	u_char row = 0;
	u_char bit = 0x01;
	u_char oc = c - 0x20;
	setArea(x, y, x + 4, y + 7); // if you want to fill column between chars, change x + 4 to x + 5

	while (row < 8) {
		while (col < 5) {
			if (font_5x7[oc][col] & bit) {
				//foreground
				writeData(colorHighByte);
				writeData(colorLowByte);
			} else {
				//background
				writeData(bgColorHighByte);
				writeData(bgColorLowByte);
			}
			col++;
		}
		// if you want to fill column between chars, writeData(bgColor) here
		col = 0;
		bit <<= 1;
		row++;
	}
}

////////////////
// 11x16 font - this function does not draw background pixels
////////////////
void drawCharLg(u_int x, u_int y, char c) {
	u_char col = 0;
	u_char row = 0;
	u_int bit = 0x0001;
	u_char oc = c - 0x20;
	while (row < 16) {
		while (col < 11) {
			if (font_11x16[oc][col] & bit)
				drawPixel(x + col, y + row);
			col++;
		}
		col = 0;
		bit <<= 1;
		row++;
	}
}

////////////////
// 11x16 font - this function draws background pixels
////////////////
void drawCharLgBkg(u_int x, u_int y, char c) {
	u_char col = 0;
	u_char row = 0;
	u_int bit = 0x0001;
	u_char oc = c - 0x20;
	setArea(x, y, x + 10, y + 15);
	while (row < 16) {
		while (col < 11) {
			if (font_11x16[oc][col] & bit) {
				//foreground
				writeData(colorHighByte);
				writeData(colorLowByte);
			} else {
				//background
				writeData(bgColorHighByte);
				writeData(bgColorLowByte);
			}
			col++;
		}
		col = 0;
		bit <<= 1;
		row++;
	}
}

////////////////
// 8x12 font - this function does not draw background pixels
////////////////
void drawCharMd(u_int x, u_int y, char c) {
	u_char col = 0;
	u_char row = 0;
	u_char bit = 0x80;
	u_char oc = c - 0x20;
	while (row < 12) {
		while (col < 8) {
			if (font_8x12[oc][row] & bit)
				drawPixel(x + col, y + row);
			bit >>= 1;
			col++;
		}
		bit = 0x80;
		col = 0;
		row++;
	}
}

////////////////
// 8x12 font - this function draws background pixels
////////////////
void drawCharMdBkg(u_int x, u_int y, char c) {
	u_char col = 0;
	u_char row = 0;
	u_char bit = 0x80;
	u_char oc = c - 0x20;
	setArea(x, y, x + 7, y + 11);
	while (row < 12) {
		while (col < 8) {
			if (font_8x12[oc][row] & bit) {
				//foreground
				writeData(colorHighByte);
				writeData(colorLowByte);
			} else {
				//background
				writeData(bgColorHighByte);
				writeData(bgColorLowByte);
			}
			bit >>= 1;
			col++;
		}
		bit = 0x80;
		col = 0;
		row++;
	}

}
////////////////////////
// images
////////////////////////
//data is 16 bit color
void drawImage(u_int x, u_int y, u_int w, u_int h, u_int * data) {

}

// lut is used, ?0 means skip, sort of a mask?
void drawImageLut(u_int x, u_int y, u_int w, u_int h, u_char * data,
		u_int * lut) {

}

// each bit represents color, fg and bg colors are used, ?how about 0 as a mask?
void drawImageMono(u_int x, u_int y, u_int w, u_int h, u_char * data) {

}

////////////////////////
// shapes
////////////////////////

void drawLogicLine(u_int x, u_int y, u_int length, u_char height, u_char * data) {
	u_char last = (*data & 0x80);
	u_int counter = 0;
	while (counter < length) {
		u_char bitCounter = 0;
		u_char byte = *data;
		while (bitCounter < 8 && counter < length) {
			if (last == (byte & 0x80)) {
				//draw pixel
				u_int h = (byte & 0x80) ? (height + y) : y;
				drawPixel(x + counter, h);
			} else {
				// draw line
				if (byte & 0x80) {
					drawLine(x + counter - 1, y, x + counter, y + height);
				} else {
					drawLine(x + counter - 1, y + height, x + counter, y);
				}
			}
			last = byte & 0x80;
			byte <<= 1;
			bitCounter++;
			counter++;
		}
		*data++;
	}
}

void drawLine(u_int xStart, u_int yStart, u_int xEnd, u_int yEnd) {

	u_int x0, x1, y0, y1;
	u_int d = 0;

// handle direction
	if (yStart > yEnd) {
		y0 = yEnd;
		y1 = yStart;
	} else {
		y1 = yEnd;
		y0 = yStart;
	}

	if (xStart > xEnd) {
		x0 = xEnd;
		x1 = xStart;
	} else {
		x1 = xEnd;
		x0 = xStart;
	}

// check if horizontal
	if (y0 == y1) {
		d = x1 - x0 + 1;
		setArea(x0, y0, x1, y1);
		while (d-- > 0) {
			writeData(colorHighByte);
			writeData(colorLowByte);
		}

	} else if (x0 == x1) { // check if vertical
		d = y1 - y0 + 1;
		setArea(x0, y0, x1, y1);
		while (d-- > 0) {
			writeData(colorHighByte);
			writeData(colorLowByte);
		}

	} else { // angled
		int dx, dy;
		int sx, sy;

		if (xStart < xEnd) {
			sx = 1;
			dx = xEnd - xStart;
		} else {
			sx = -1;
			dx = xStart - xEnd;
		}

		if (yStart < yEnd) {
			sy = 1;
			dy = yEnd - yStart;
		} else {
			sy = -1;
			dy = yStart - yEnd;
		}

		int e1 = dx - dy;
		int e2;

		while (1) {
			drawPixel(xStart, yStart);
			if (xStart == xEnd && yStart == yEnd)
				break;
			e2 = 2 * e1;
			if (e2 > -dy) {
				e1 = e1 - dy;
				xStart = xStart + sx;
			}
			if (e2 < dx) {
				e1 = e1 + dx;
				yStart = yStart + sy;
			}
		}
	}
}

void drawRect(u_int xStart, u_int yStart, u_int xEnd, u_int yEnd) {

	drawLine(xStart, yStart, xEnd, yStart);
	drawLine(xStart, yEnd, xEnd, yEnd);
	drawLine(xStart, yStart, xStart, yEnd);
	drawLine(xEnd, yStart, xEnd, yEnd);
}

void drawCircle(u_int x, u_int y, u_char radius) {
	int dx = radius;
	int dy = 0;
	int xChange = 1 - 2 * radius;
	int yChange = 1;
	int radiusError = 0;
	while (dx >= dy) {
		drawPixel(x + dx, y + dy);
		drawPixel(x - dx, y + dy);
		drawPixel(x - dx, y - dy);
		drawPixel(x + dx, y - dy);
		drawPixel(x + dy, y + dx);
		drawPixel(x - dy, y + dx);
		drawPixel(x - dy, y - dx);
		drawPixel(x + dy, y - dx);
		dy++;
		radiusError += yChange;
		yChange += 2;
		if (2 * radiusError + xChange > 0) {
			dx--;
			radiusError += xChange;
			xChange += 2;
		}
	}
}

/////////////////////////
// fill
/////////////////////////

void fillRect(u_int xStart, u_int yStart, u_int xEnd, u_int yEnd) {
	setArea(xStart, yStart, xEnd, yEnd);
	u_int x = xEnd - xStart + 1;
	u_int y = yEnd - yStart + 1;
	u_int z = y;
	while (x != 0) {
		while (z != 0) {
			writeData(colorHighByte);
			writeData(colorLowByte);
			z--;
		}
		z = y;
		x--;
	}
}

void fillCircle(u_int x, u_int y, u_char radius) {
	int dx = radius;
	int dy = 0;
	int xChange = 1 - 2 * radius;
	int yChange = 1;
	int radiusError = 0;
	while (dx >= dy) {
		drawLine(x + dy, y + dx, x - dy, y + dx);
		drawLine(x - dy, y - dx, x + dy, y - dx);
		drawLine(x - dx, y + dy, x + dx, y + dy);
		drawLine(x - dx, y - dy, x + dx, y - dy);
		dy++;
		radiusError += yChange;
		yChange += 2;
		if (2 * radiusError + xChange > 0) {
			dx--;
			radiusError += xChange;
			xChange += 2;
		}
	}
}


	////////////////////////////////////
	// gamma, lut, and other inits
	////////////////////////////////////

	/////////////////////////////////////////////////
	// ILI9340 based display
	/////////////////////////////////////////////////


void initLCD() {
		writeCommand(PWRCTRLA);
		writeData(0x39);
		writeData(0x2C);
		writeData(0x00);
		writeData(0x34);
		writeData(0x02);

		writeCommand(PWRCTRLB);
		writeData(0x00);
		writeData(0XC1);
		writeData(0X30);

		writeCommand(DTCTRLA1);
		writeData(0x85);
		writeData(0x00);
		writeData(0x78);

		writeCommand(DTCTRLB);
		writeData(0x00);
		writeData(0x00);

		writeCommand(POSC);
		writeData(0x64);
		writeData(0x03);
		writeData(0X12);
		writeData(0X81);

		writeCommand(PRC);
		writeData(0x20);

		writeCommand(ILIPC1);
		writeData(0x23);
		writeCommand(ILIPC2);
		writeData(0x10);
		writeCommand(ILIVC1);
		writeData(0x3e);
		writeData(0x28);
		writeCommand(ILIVC2);
		writeData(0x86);

		setOrientation(ORIENTATION);

		writeCommand(COLMOD);
		writeData(0x55);

		writeCommand(ILIFCNM);
		writeData(0x00);
		writeData(0x18);

		writeCommand(ILIDFC);
		writeData(0x08);
		writeData(0x82);
		writeData(0x27);

		writeCommand(ILIGFD);
		writeData(0x00);
		writeCommand(ILIGS);
		writeData(0x01);

		writeCommand(ILIPGC);
		const u_char gamma1[] = { 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1,
				0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00 };
		u_char c = 0;
		while (c < 16) {
			writeData(gamma1[c]);
			c++;
		}

		writeCommand(ILINGC);
		const u_char gamma2[] = { 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1,
				0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F };
		c = 0;
		while (c < 16) {
			writeData(gamma2[c]);
			c++;
		}

		writeCommand(SLEEPOUT);
		DSP28x_usDelay(12);
		writeCommand(DISPON);
		writeCommand(RAMWRP);

}

	void setOrientation(u_char orientation) {

		writeCommand(ILIMAC);

		switch (orientation) {
		case 1:
			writeData(0xE8);
			_orientation = 1;
			break;
		case 2:
			writeData(0x88);
			_orientation = 2;
			break;
		case 3:
			writeData(0x28);
			_orientation = 3;
			break;
		default:
			writeData(0x48);
			_orientation = 0;
		}
	}


