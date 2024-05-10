
#include "stm32l0xx_hal.h"
#include "fonts.h"

/**
 * This Library is written and optimized by Olivier Van den Eede(4ilo) in 2016
 * for Stm32 Uc and HAL-i2c lib's.
 *
 * To use this library with ssd1306 oled display you will need to customize the defines below.
 *
 * This library uses 2 extra files (fonts.c/h).
 * In this files are 3 different fonts you can use:
 * 		- Font_7x10
 * 		- Font_11x18
 * 		- Font_16x26
 *
 */

#ifndef ssd1306
#define ssd1306

// I2c port as defined in main generated by CubeMx
#define SSD1306_I2C_PORT		hi2c1
// I2c address
#define SSD1306_I2C_ADDR        0x78
// SSD1306 width in pixels
#define SSD1306_WIDTH           128
// SSD1306 LCD height in pixels
#define SSD1306_HEIGHT          64

#define vccstate 0x2



//benim ekledigim--------------

#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF

#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA

#define SSD1306_SETVCOMDETECT 0xDB

#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9

#define SSD1306_SETMULTIPLEX 0xA8

#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10

#define SSD1306_SETSTARTLINE 0x40

#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22

#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8

#define SSD1306_SEGREMAP 0xA0

#define SSD1306_CHARGEPUMP 0x8D

#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2

// Scrolling #defines
#define SSD1306_ACTIVATE_SCROLL 0x2F
#define SSD1306_DEACTIVATE_SCROLL 0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A

//----------------------------



//
//  Enumeration for screen colors
//
typedef enum {
	Black = 0x00, // Black color, no pixel
	White = 0x01  //Pixel is set. Color depends on LCD
} SSD1306_COLOR;

//
//  Struct to store transformations
//
typedef struct {
	uint16_t CurrentX;
	uint16_t CurrentY;
	uint8_t Inverted;
	uint8_t Initialized;
} SSD1306_t;

//	Definition of the i2c port in main
extern I2C_HandleTypeDef SSD1306_I2C_PORT;

//
//  Function definitions
//
uint8_t ssd1306_Init(void);
void ssd1306_InvertDisplay(uint8_t i);
void ssd1306_Startscrollright(uint8_t start, uint8_t stop);
void ssd1306_Startscrollleft(uint8_t start, uint8_t stop);
void ssd1306_Startscrolldiagright(uint8_t start, uint8_t stop);
void ssd1306_Startscrolldiagleft(uint8_t start, uint8_t stop);
void ssd1306_Stopscroll(void);
void ssd1306_Dim(uint8_t dim);
void ssd1306_draw_rectangle(uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2, SSD1306_COLOR c);
void ssd1306_line_v(uint8_t y0, uint8_t y1, uint8_t x, uint8_t width, SSD1306_COLOR c);
void ssd1306_line_h(uint8_t x0, uint8_t x1, uint8_t y0, uint8_t width, SSD1306_COLOR c);
void ssd1306_draw_bitmap(uint8_t left, uint8_t top, const unsigned char *bitmap);
void ssd1306_draw_line( unsigned char x1, unsigned char y1,
		unsigned char x2, unsigned char y2,
		SSD1306_COLOR c);
void ssd1306_draw_Circle(int x0, int y0, int radius, SSD1306_COLOR c);

void ssd1306_Fill(SSD1306_COLOR color);
void ssd1306_UpdateScreen(void);
void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color);
char ssd1306_WriteChar(char ch, FontDef Font, SSD1306_COLOR color);
char ssd1306_WriteString(char* str, FontDef Font, SSD1306_COLOR color);
void ssd1306_SetCursor(uint8_t x, uint8_t y);
void ssd1306_WriteData(uint8_t* buffer, size_t buff_size);

#endif