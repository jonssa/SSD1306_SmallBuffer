/*
 * ssd1306.h
 *
 *  Created on: 16 cze 2020
 *      Author: Macias
 */

#ifndef __SSD1306_H__
#define __SSD1306_H__

#include <stddef.h>
#include <_ansi.h>
#include "ssd1306_fonts.h"

#define DISPLAY_SET				0x01
#define DISPLAY_RESET			0x00

#define CLR_BUFFER_SET			0x01
#define CLR_BUFFER_RESET		0x00

// SSD1306 I2C address
#ifndef SSD1306_I2C_ADDR
#define SSD1306_I2C_ADDR        0x3C
#endif

// SSD1306 height in pixels
#ifndef SSD1306_HEIGHT
#define SSD1306_HEIGHT          64
#endif

// SSD1306 width in pixels
#ifndef SSD1306_WIDTH
#define SSD1306_WIDTH           128
#endif

// Pages count is current display
#ifndef SSD1306_PAGES_COUNT
#define SSD1306_PAGES_COUNT		SSD1306_HEIGHT / 8
#endif
/*
#define SSD1306_FULL_BUFFER		0

#if SD1306_FULL_BUFFER==1
// SSD1306 one page buffer
#ifndef SSD1306_PAGE_BUFFER
#define SSD1306_PAGE_BUFFER 	(SSD1306_WIDTH * 8)
#endif

#else*/

#ifndef	SSD1306_PAGES_IN_BUFFER
#define SSD1306_PAGES_IN_BUFFER 1
#endif

// Size of screen buffer
#ifndef SSD1306_PAGE_BUFFER
#define SSD1306_PAGE_BUFFER 	(SSD1306_WIDTH * SSD1306_PAGES_IN_BUFFER)
#endif

//#endif

// Enumeration for screen colors
typedef enum {
    Black = 0x00, // Black color, no pixel
    White = 0x01  // Pixel is set. Color depends on OLED
} SSD1306_COLOR;

// Struct to store coordinates
typedef struct {
    uint8_t CurrentX;
    uint8_t CurrentY;
    uint8_t CurrentLine;
} SSD1306_t;

/* SSD1306 functions  ****************************************************/
/******* Init function */
void ssd1306_Init(void);

/******* Commands function */
void ssd1306_ClearBuf(SSD1306_COLOR color);
void ssd1306_UpdatePage(uint8_t page);
void ssd1306_NewLine(SSD1306_COLOR color);
void ssd1306_SetCursor(uint8_t x, uint8_t y);
void ssd1306_SetLine(uint8_t x, uint8_t line);

/******* Draw functions */
void ssd1306_Fill(SSD1306_COLOR color);
char ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color);
char ssd1306_WriteChar(char ch, FontDef Font, SSD1306_COLOR color);
char *ssd1306_WriteLine(char *str, FontDef Font, SSD1306_COLOR color, uint8_t clr_buffer, uint8_t display);
char ssd1306_WriteInt(uint32_t num, FontDef Font, SSD1306_COLOR color, uint8_t clr_buffer, uint8_t display);

#endif // __SSD1306_H__
