/*
 * ssd1306.c
 *
 *  Created on: 16 cze 2020
 *      Author: Macias
 */

#include <math.h>
#include <stdlib.h>
#include "ssd1306.h"

/* Private functions to write bytes to SSD1306 */
static void ssd1306_WriteCommand(uint8_t byte);
static void ssd1306_WriteData(uint8_t* buffer, size_t buffer_size);

/* Private variables*/
static uint8_t SSD1306_Buffer[SSD1306_PAGE_BUFFER];		//screen buffer
static SSD1306_t SSD1306;					//screen coordinates structure

/**
  * @brief  Send command byte to SSD1306
  * @param  byte - byte to send
  * @retval none
  */
static void ssd1306_WriteCommand(uint8_t byte)
{
	I2C1_Memory_Send(SSD1306_I2C_ADDR, &byte, 0x00, 1, 10);			// put here your own function I2C write fcn
}

/**
  * @brief  Send data buffer to SSD1306
  * @param  buffer 		- pointer to buffer
  * @param  buffer_size - buffer size in bytes
  * @retval none
  */
static void ssd1306_WriteData(uint8_t *buffer, size_t buffer_size)
{
	I2C1_Memory_Send(SSD1306_I2C_ADDR, buffer, 0x40, buffer_size, 10);	// put here your own function I2C write fcn
}

/**
  * @brief  Initializes SSD1306
  * 		Check SSD1306 datasheet to find commands to write for different init options
  * @param  none
  * @retval none
  */
void ssd1306_Init(void)
{

    // Wait for the screen to boot
    HAL_Delay(100);

    // Init OLED
    ssd1306_WriteCommand(0xAE); //display off

    ssd1306_WriteCommand(0x20); //Set Memory Addressing Mode
    ssd1306_WriteCommand(0x10); // 00b,Horizontal Addressing Mode; 01b,Vertical Addressing Mode;
                                // 10b,Page Addressing Mode (RESET); 11b,Invalid

    ssd1306_WriteCommand(0xB0); //Set Page Start Address for Page Addressing Mode,0-7

#ifdef SSD1306_MIRROR_VERT
    ssd1306_WriteCommand(0xC0); // Mirror vertically
#else
    ssd1306_WriteCommand(0xC8); //Set COM Output Scan Direction
#endif

    ssd1306_WriteCommand(0x00); //---set low column address
    ssd1306_WriteCommand(0x10); //---set high column address

    ssd1306_WriteCommand(0x40); //--set start line address - CHECK

    ssd1306_WriteCommand(0x81); //--set contrast control register - CHECK
    ssd1306_WriteCommand(0xFF);

#ifdef SSD1306_MIRROR_HORIZ
    ssd1306_WriteCommand(0xA0); // Mirror horizontally
#else
    ssd1306_WriteCommand(0xA1); //--set segment re-map 0 to 127 - CHECK
#endif

#ifdef SSD1306_INVERSE_COLOR
    ssd1306_WriteCommand(0xA7); //--set inverse color
#else
    ssd1306_WriteCommand(0xA6); //--set normal color
#endif

// Set multiplex ratio.
#if (SSD1306_HEIGHT == 128)
    // Found in the Luma Python lib for SH1106.
    ssd1306_WriteCommand(0xFF);
#else
    ssd1306_WriteCommand(0xA8); //--set multiplex ratio(1 to 64) - CHECK
#endif

#if (SSD1306_HEIGHT == 32)
    ssd1306_WriteCommand(0x1F); //
#elif (SSD1306_HEIGHT == 64)
    ssd1306_WriteCommand(0x3F); //
#else
#error "Only 32 and 64 pixels height are supported!"
#endif

    ssd1306_WriteCommand(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content

    ssd1306_WriteCommand(0xD3); //-set display offset - CHECK
    ssd1306_WriteCommand(0x00); //-not offset

    ssd1306_WriteCommand(0xD5); //--set display clock divide ratio/oscillator frequency
    ssd1306_WriteCommand(0xF0); //--set divide ratio

    ssd1306_WriteCommand(0xD9); //--set pre-charge period
    ssd1306_WriteCommand(0x22); //

    ssd1306_WriteCommand(0xDA); //--set com pins hardware configuration - CHECK
#if (SSD1306_HEIGHT == 32)
    ssd1306_WriteCommand(0x02);
#elif (SSD1306_HEIGHT == 64)
    ssd1306_WriteCommand(0x12);
#elif (SSD1306_HEIGHT == 128)
    ssd1306_WriteCommand(0x12);
#else
#error "Only 32, 64, or 128 lines of height are supported!"
#endif

    ssd1306_WriteCommand(0xDB); //--set vcomh
    ssd1306_WriteCommand(0x20); //0x20,0.77xVcc

    ssd1306_WriteCommand(0x8D); //--set DC-DC enable
    ssd1306_WriteCommand(0x14); //
    ssd1306_WriteCommand(0xAF); //--turn on SSD1306 panel

    // Clear screen
    ssd1306_Fill(Black);

    // Set default values for screen object
    SSD1306.CurrentX = 0;
    SSD1306.CurrentY = 0;
}

/**
  * @brief  Clears screen buffer by writing to it sent color byte value
  * @param  color - Enum variable representing color
  * 		 @arg @ref Black
  *          @arg @ref White
  * @retval none
  */
inline void ssd1306_ClearBuf(SSD1306_COLOR color)
{
	/*Clear buffer by writing to it respondent color byte*/
	for(uint16_t i = 0; i < sizeof(SSD1306_Buffer); i++)
		SSD1306_Buffer[i] = (color == Black) ? 0x00 : 0xFF;
}

/**
  * @brief  Sends screen buffer by I2C to proper page address in SSD1306 RAM buffer
  * @param  page - current page(s) which has to be updated
  * @retval none
  */
inline void ssd1306_UpdatePage(uint8_t page)
{
	/*Check if page count is not bigger than pages number is current screen*/
	page %= SSD1306_PAGES_COUNT;

	/*Set current page and column address*/
	ssd1306_WriteCommand(0xB0 + page);
	ssd1306_WriteCommand(0x00);
	ssd1306_WriteCommand(0x10);

	/*First param could be only SSD1306_Buffer*/
	ssd1306_WriteData(SSD1306_Buffer, SSD1306_PAGE_BUFFER);
}

/**
  * @brief  Update current line and set pointer to new line
  * @param  color - Enum variable representing color
  * 		 @arg @ref Black
  *          @arg @ref White
  * @retval none
  */
inline void ssd1306_NewLine(SSD1306_COLOR color)
{
	ssd1306_UpdatePage(SSD1306.CurrentLine);

	/*Increment line counter, set x, y pointer on zero position in new line*/
	if(++SSD1306.CurrentLine == SSD1306_PAGES_COUNT)
		SSD1306.CurrentLine = 0;
	SSD1306.CurrentX = 0;
	SSD1306.CurrentY = 0;

	ssd1306_ClearBuf(!color);			// Need to clear buffer because in other case new line buffer will
										// contain data from previous line
}

/**
  * @brief  Set cursor position on x, y coordinates
  * @param  x	  - Column coordinates
  * @param  y	  - Row coordinates
  * @retval none
  */
inline void ssd1306_SetCursor(uint8_t x, uint8_t y)
{
    SSD1306.CurrentX = x;
    SSD1306.CurrentY = y;
    SSD1306.CurrentLine = y/8;
}

/**
  * @brief  Set cursor position on line with x coordinates
  * @param  x	  - Column coordinates
  * @param  line  - Line to write now
  * @retval non-zero value if correct
  */
inline void ssd1306_SetLine(uint8_t x, uint8_t line)
{
	SSD1306.CurrentX = x;
	SSD1306.CurrentY = line*8;
	SSD1306.CurrentLine = line;
}

/**
  * @brief  Fill the whole screen with the given color
  * @param  color - Enum variable representing color
  * 		 @arg @ref Black
  *          @arg @ref White
  * @retval none
  */
inline void ssd1306_Fill(SSD1306_COLOR color)
{
    ssd1306_ClearBuf(color);
    for(uint32_t i=0; i < SSD1306_PAGES_COUNT; i++)
    	ssd1306_UpdatePage(i);
}

/**
  * @brief  Draw one pixel to screen buffer
  * @param  x	  - Column coordinates
  * @param  y	  - Row coordinates
  * @param  color - Enum variable representing color
  * 		 @arg @ref Black
  *          @arg @ref White
  * @retval non-zero value if correct
  */
char ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color)
{
	/*Check if pixels wont't be drawn outside screen*/
    if(x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT)
    {
        return 0;
    }

    /*Write pixel into screen buffer*/
    if(color == White)
    {
        SSD1306_Buffer[x + (((y/8) % SSD1306_PAGES_IN_BUFFER) * SSD1306_WIDTH)] |= 1 << (y % 8);
    }
    else
    {
    	SSD1306_Buffer[x + (((y/8) % SSD1306_PAGES_IN_BUFFER) * SSD1306_WIDTH)] &= ~(1 << (y % 8));
    }
    return 1;
}

/**
  * @brief  Write one character into buffer
  * @param  ch	  - character which has to be written to screen buffer
  * @param  Font  - character font which is defined in fonts.c
  * @param  color - Enum variable representing color
  * 		 @arg @ref Black
  *          @arg @ref White
  * @retval character if correct
  */
char ssd1306_WriteChar(char ch, FontDef Font, SSD1306_COLOR color)
{
    uint8_t current_char;

    /*Check if character is valid or is new line*/
    if ((ch < 32 || ch > 126) && ch != '\n')
        return 0;

    /*Draw whole character into screen buffer*/
    if(ch != '\n')
	{
		for(uint8_t x = 0; x < Font.FontWidth; x++)
		{
			current_char = Font.data[(ch - 32) * Font.FontWidth + x];
			for(uint8_t y = 0; y < Font.FontHeight; y++)
			{
				if((current_char >> y) & 0x01)
				{
					if(!ssd1306_DrawPixel(SSD1306.CurrentX + x, y, (SSD1306_COLOR) color))
						return 0;
				}
				else
				{
					if(!ssd1306_DrawPixel(SSD1306.CurrentX + x, y, (SSD1306_COLOR)!color))
						return 0;
				}
			}
		}
		/*Update current x position*/
		SSD1306.CurrentX += Font.FontWidth;
	}
    return ch;
}

/**
  * @brief  Write whole string to screen buffer
  * @param  str	  - Pointer to string to display
  * @param  Font  - Character font which is defined in fonts.c
  * @param  color - Enum variable representing color
  * 		 This parameter can be one of the following values:
  * 		 @arg @ref Black
  *          @arg @ref White
  * @param  clr_buffer - Defines if clear screen buffer before writing string
  * 		 This parameter can be one of the following values:
  * 		 @arg @ref DISPLAY_SET
  *          @arg @ref DISPLAY_RESET
  * @param  display - Defines if display buffer after writing string
  * 		 This parameter can be one of the following values:
  * 		 @arg @ref CLR_BUFFER_SET
  *          @arg @ref CLR_BUFFER_RESET
  * @retval Pointer to string if correct
  */
char *ssd1306_WriteLine(char *str, FontDef Font, SSD1306_COLOR color, uint8_t clr_buffer, uint8_t display)
{
	if(clr_buffer)
		ssd1306_ClearBuf(!color);

	while(*str)
	{
		if((SSD1306.CurrentX + Font.FontWidth) > SSD1306_WIDTH || *str == '\n')
			ssd1306_NewLine(color);

		if (ssd1306_WriteChar(*str, Font, color) != *str)
			return NULL;

		str++;
	}

	if(display)
		ssd1306_UpdatePage(SSD1306.CurrentLine);

	return str;
}

/**
  * @brief  Write integer to screen buffer
  * @param  str	  - Number to display
  * @param  Font  - Character font which is defined in fonts.c
  * @param  color - Enum variable representing color
  * 		 This parameter can be one of the following values:
  * 		 @arg @ref Black
  *          @arg @ref White
  * @param  clr_buffer - Defines if clear screen buffer before writing string
  * 		 This parameter can be one of the following values:
  * 		 @arg @ref DISPLAY_SET
  *          @arg @ref DISPLAY_RESET
  * @param  display - Defines if display buffer after writing string
  * 		 This parameter can be one of the following values:
  * 		 @arg @ref CLR_BUFFER_SET
  *          @arg @ref CLR_BUFFER_RESET
  * @retval non-zero value if correct
  */
char ssd1306_WriteInt(uint32_t num, FontDef Font, SSD1306_COLOR color, uint8_t clr_buffer, uint8_t display)
{
	if(clr_buffer)
		ssd1306_ClearBuf(!color);

	uint8_t i = 0;
	char buf[11];
	itoa(num, buf, 10);

	while(buf[i])
	{
		if((SSD1306.CurrentX + Font.FontWidth) > SSD1306_WIDTH)
			ssd1306_NewLine(White);

		if(ssd1306_WriteChar(buf[i], Font, color) != buf[i])
			return 0;

		i++;
	}

	if(display)
		ssd1306_UpdatePage(SSD1306.CurrentLine);

	return 1;
}

