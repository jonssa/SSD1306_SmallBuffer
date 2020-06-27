# SSD1306_SmallBuffer

# Introduction
This library was written for STML011K4. Because of it's small RAM memory (2kB) using full buffer for 128x64 or even 128x32 SSD1306. By writing this library buffer size is reduced to SSD1306 one page size (128 bytes).

# For who
For everybode who wants to reduce their RAM usage. This library can be used in STM32 but also in AVR based projects. Only thing You need to change to use it in other MCU is I2C write functions and change HAL_delay() function in ssd1306_Init() function to function fitting You.

# Memory usage
For now library uses:

RAM: 139 Bytes (SSD1306_t struct + FontDef struct + buffer size)
FLASH: 2080 Bytes (1504 instruction bytes + 576 font bytes)
