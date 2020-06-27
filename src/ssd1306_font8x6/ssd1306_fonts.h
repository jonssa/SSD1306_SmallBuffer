/*
 * ssd1306.h
 *
 *  Created on: 16 cze 2020
 *      Author: Macias
 */

#include <stdint.h>

#ifndef __SSD1306_FONTS_H__
#define __SSD1306_FONTS_H__

/*Struct containing font properties*/
typedef struct {
	const uint8_t FontWidth;
	uint8_t FontHeight;
	const uint8_t *data; //Pointer to data font data array
} FontDef;


extern FontDef Font6x8;
//extern FontDef Font_7x10;
//extern FontDef Font_11x18;
//extern FontDef Font_16x26;

#endif // __SSD1306_FONTS_H__
