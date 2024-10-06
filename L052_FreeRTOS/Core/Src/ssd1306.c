
#include"ssd1306.h"


// Screenbuffer
static uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

// Screen object
static SSD1306_t SSD1306;


//
//  Send a byte to the command register
//
static void ssd1306_WriteCommand(uint8_t command)
{
	HAL_I2C_Mem_Write(&hi2c1,SSD1306_I2C_ADDR,0x00,1,&command,1,10);
}


//
//	Initialize the oled screen
//
uint8_t ssd1306_Init(void)
{	
	// Wait for the screen to boot
	HAL_Delay(100);
	
	/* Init LCD */
	ssd1306_WriteCommand(0xAE); //display off
	ssd1306_WriteCommand(0x20); //Set Memory Addressing Mode   
	ssd1306_WriteCommand(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	ssd1306_WriteCommand(0xB0); //Set Page Start Address for Page Addressing Mode,0-7
	ssd1306_WriteCommand(0xC8); //Set COM Output Scan Direction
	ssd1306_WriteCommand(0x00); //---set low column address
	ssd1306_WriteCommand(0x10); //---set high column address
	ssd1306_WriteCommand(0x40); //--set start line address
	ssd1306_WriteCommand(0x81); //--set contrast control register
	ssd1306_WriteCommand(0xFF);
	ssd1306_WriteCommand(0xA1); //--set segment re-map 0 to 127
	ssd1306_WriteCommand(0xA6); //--set normal display
	ssd1306_WriteCommand(0xA8); //--set multiplex ratio(1 to 64)
	ssd1306_WriteCommand(0x3F); //
	ssd1306_WriteCommand(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	ssd1306_WriteCommand(0xD3); //-set display offset
	ssd1306_WriteCommand(0x00); //-not offset
	ssd1306_WriteCommand(0xD5); //--set display clock divide ratio/oscillator frequency
	ssd1306_WriteCommand(0xF0); //--set divide ratio
	ssd1306_WriteCommand(0xD9); //--set pre-charge period
	ssd1306_WriteCommand(0x22); //
	ssd1306_WriteCommand(0xDA); //--set com pins hardware configuration
	ssd1306_WriteCommand(0x12);
	ssd1306_WriteCommand(0xDB); //--set vcomh
	ssd1306_WriteCommand(0x20); //0x20,0.77xVcc
	ssd1306_WriteCommand(0x8D); //--set DC-DC enable
	ssd1306_WriteCommand(0x14); //
	ssd1306_WriteCommand(0xAF); //--turn on SSD1306 panel
	
	// Clear screen
	ssd1306_Fill(Black);
	
	// Flush buffer to screen
	ssd1306_UpdateScreen();
	
	// Set default values for screen object
	SSD1306.CurrentX = 0;
	SSD1306.CurrentY = 0;
	
	SSD1306.Initialized = 1;
	
	return 1;
}


//----------benim ekledigim--------------------


void ssd1306_InvertDisplay(uint8_t i)
{
  if (i) {
	  ssd1306_WriteCommand(SSD1306_INVERTDISPLAY);
  } else {
	  ssd1306_WriteCommand(SSD1306_NORMALDISPLAY);
  }
}


// startscrollright
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void ssd1306_Startscrollright(uint8_t start, uint8_t stop)
{
	ssd1306_WriteCommand(SSD1306_RIGHT_HORIZONTAL_SCROLL);
	ssd1306_WriteCommand(0X00);
	ssd1306_WriteCommand(start);
	ssd1306_WriteCommand(0X00);
	ssd1306_WriteCommand(stop);
	ssd1306_WriteCommand(0X00);
	ssd1306_WriteCommand(0XFF);
	ssd1306_WriteCommand(SSD1306_ACTIVATE_SCROLL);
}



// startscrollleft
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void ssd1306_Startscrollleft(uint8_t start, uint8_t stop)
{
	ssd1306_WriteCommand(SSD1306_LEFT_HORIZONTAL_SCROLL);
	ssd1306_WriteCommand(0X00);
	ssd1306_WriteCommand(start);
	ssd1306_WriteCommand(0X00);
	ssd1306_WriteCommand(stop);
	ssd1306_WriteCommand(0X00);
	ssd1306_WriteCommand(0XFF);
	ssd1306_WriteCommand(SSD1306_ACTIVATE_SCROLL);
}


// startscrolldiagright
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void ssd1306_Startscrolldiagright(uint8_t start, uint8_t stop)
{
	ssd1306_WriteCommand(SSD1306_SET_VERTICAL_SCROLL_AREA);
	ssd1306_WriteCommand(0X00);
	ssd1306_WriteCommand(SSD1306_HEIGHT);
	ssd1306_WriteCommand(SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
	ssd1306_WriteCommand(0X00);
	ssd1306_WriteCommand(start);
	ssd1306_WriteCommand(0X00);
	ssd1306_WriteCommand(stop);
	ssd1306_WriteCommand(0X01);
	ssd1306_WriteCommand(SSD1306_ACTIVATE_SCROLL);
}

// startscrolldiagleft
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void ssd1306_Startscrolldiagleft(uint8_t start, uint8_t stop)
{
	ssd1306_WriteCommand(SSD1306_SET_VERTICAL_SCROLL_AREA);
	ssd1306_WriteCommand(0X00);
	ssd1306_WriteCommand(SSD1306_HEIGHT);
	ssd1306_WriteCommand(SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
	ssd1306_WriteCommand(0X00);
	ssd1306_WriteCommand(start);
	ssd1306_WriteCommand(0X00);
	ssd1306_WriteCommand(stop);
	ssd1306_WriteCommand(0X01);
	ssd1306_WriteCommand(SSD1306_ACTIVATE_SCROLL);
}

void ssd1306_Stopscroll(void)
{
	ssd1306_WriteCommand(SSD1306_DEACTIVATE_SCROLL);
}

// Dim the display
// dim = 1: display is dimmed
// dim = 0: display is normal
void ssd1306_Dim(uint8_t dim)
{
  uint8_t contrast;

  if (dim) {
    contrast = 0; // Dimmed display
  } else {
    if (vccstate == SSD1306_EXTERNALVCC) {
      contrast = 0x9F;
    } else {
      contrast = 0xCF;
    }
  }
  // the range of contrast to too small to be really useful
  // it is useful to dim the display
  ssd1306_WriteCommand(SSD1306_SETCONTRAST);
  ssd1306_WriteCommand(contrast);
}

//gxf:

void ssd1306_draw_rectangle(uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2, SSD1306_COLOR c)
{
	uint8_t i,j,buffer;
	if(x1>x2)
	{
		buffer = x2;
		x2 = x1;
		x1 = buffer;
	}

	if(y1>y2)
	{
		buffer = y2;
		y2 = y1;
		y1 = buffer;
	}
	for(i=x1;i<=x2;i++)
		for(j=y1;j<y2;j++)
			ssd1306_DrawPixel(i, j, c);
}


void ssd1306_line_h(uint8_t x0, uint8_t x1, uint8_t y0, uint8_t width, SSD1306_COLOR c)
{
	if(x0>x1)
	{
		uint8_t temp = x0;
		x0 = x1;
		x1 = temp;
	}
	uint8_t i,j;

	if(width%2)
	{
		uint8_t start = y0-(width-1)/2;
		for(i=0;i<width;i++)
			for(j=x0;j<=x1;j++)
				ssd1306_DrawPixel(j, start+i, c);
	}
	else
	{
		uint8_t start = y0-width/2;
		for(i=0;i<width;i++)
			for(j=x0;j<=x1;j++)
				ssd1306_DrawPixel(j, start+i, c);
	}

}


void ssd1306_draw_bitmap(uint8_t left, uint8_t top, const unsigned char *bitmap)
{
	uint16_t width = bitmap[1]<<8 | bitmap[0];
	uint16_t height = bitmap[3]<<8 | bitmap[2];

	uint8_t i,j,k;
	uint8_t byte;
	uint8_t x,y;
	for(k=0;k<=(height/8);k++)
	{
		for(i=0;i<=width;i++)
		{
			byte = bitmap[i+3+k*width];
			for(j=0;j<8;j++)
			{
				x = left+i-1;
				y = top+j+(k<<3);
				if(x-left>=0 && x-left<width && y-top>=0 && y-top<height) //clipping
				{
					if(byte&(0x01<<j))
						ssd1306_DrawPixel(x, y, White);
					else
						ssd1306_DrawPixel(x, y, Black);
				}
			}
		}
	}
}



void ssd1306_draw_line( unsigned char x1, unsigned char y1,
		unsigned char x2, unsigned char y2,
		SSD1306_COLOR c)
{
	unsigned char length, xTmp, yTmp, i, y, yAlt;
	int m;

	if(x1 == x2) {									// vertical line
		// x1|y1 must be the upper point
		if(y1 > y2) {
			xTmp = x1;
			yTmp = y1;
			x1 = x2;
			y1 = y2;
			x2 = xTmp;
			y2 = yTmp;
		}

		length = y2-y1;
		for(i=0; i<=length; i++) {
			ssd1306_DrawPixel(x1, y1+i, c);
		}
	} else if(y1 == y2) {							// horizontal line
		// x1|y1 must be the left point
		if(x1 > x2) {
			xTmp = x1;
			yTmp = y1;
			x1 = x2;
			y1 = y2;
			x2 = xTmp;
			y2 = yTmp;
		}

		length = x2-x1;
		for(i=0; i<=length; i++) {
			ssd1306_DrawPixel(x1+i, y1, c);
		}
	} else {
		// x1 must be smaller than x2
		if(x1 > x2) {
			xTmp = x1;
			yTmp = y1;
			x1 = x2;
			y1 = y2;
			x2 = xTmp;
			y2 = yTmp;
		}

		if((y2-y1) >= (x2-x1) || (y1-y2) >= (x2-x1)) {	// angle larger or equal 45�
			length = x2-x1;								// not really the length :)
			m = ((y2-y1)*200)/length;
			yAlt = y1;
			for(i=0; i<=length; i++) {
				y = ((m*i)/200)+y1;

				if((m*i)%200 >= 100)
					y++;
				else if((m*i)%200 <= -100)
					y--;

				ssd1306_draw_line(x1+i, yAlt, x1+i, y, c ); /* wuff wuff recurs. */

				if(length<=(y2-y1) && y1 < y2)
					yAlt = y+1;
				else if(length<=(y1-y2) && y1 > y2)
					yAlt = y-1;
				else
					yAlt = y;
			}
		} else {										// angle smaller 45�
			// y1 must be smaller than y2
			if(y1 > y2) {
				xTmp = x1;
				yTmp = y1;
				x1 = x2;
				y1 = y2;
				x2 = xTmp;
				y2 = yTmp;
			}
			length = y2-y1;
			m = ((x2-x1)*200)/length;
			yAlt = x1;
			for(i=0; i<=length; i++) {
				y = ((m*i)/200)+x1;

				if((m*i)%200 >= 100)
					y++;
				else if((m*i)%200 <= -100)
					y--;

				ssd1306_draw_line(yAlt, y1+i, y, y1+i, c);
				if(length<=(x2-x1) && x1 < x2)
					yAlt = y+1;
				else if(length<=(x1-x2) && x1 > x2)
					yAlt = y-1;
				else
					yAlt = y;
			}
		}
	}
}

//-----------------------------------------------

//
//  Fill the whole screen with the given color
//
void ssd1306_Fill(SSD1306_COLOR color) 
{
	/* Set memory */
	uint32_t i;

	for(i = 0; i < sizeof(SSD1306_Buffer); i++)
	{
		SSD1306_Buffer[i] = (color == Black) ? 0x00 : 0xFF;
	}
}

//
//  Write the screenbuffer with changed to the screen
//
void ssd1306_UpdateScreen(void) 
{
	uint8_t i;
	
	for (i = 0; i < 8; i++) {
		ssd1306_WriteCommand(0xB0 + i);
		ssd1306_WriteCommand(0x00);
		ssd1306_WriteCommand(0x10);

		HAL_I2C_Mem_Write(&hi2c1,SSD1306_I2C_ADDR,0x40,1,&SSD1306_Buffer[SSD1306_WIDTH * i],SSD1306_WIDTH,100);
	}
}



//
//	Draw one pixel in the screenbuffer
//	X => X Coordinate
//	Y => Y Coordinate
//	color => Pixel color
//
void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color)
{
	if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) 
	{
		// Don't write outside the buffer
		return;
	}
	
	// Check if pixel should be inverted
	if (SSD1306.Inverted) 
	{
		color = (SSD1306_COLOR)!color;
	}
	
	// Draw in the right color
	if (color == White)
	{
		SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
	} 
	else 
	{
		SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
	}
}

//
//  Draw 1 char to the screen buffer
//	ch 		=> char om weg te schrijven
//	Font 	=> Font waarmee we gaan schrijven
//	color 	=> Black or White
//
char ssd1306_WriteChar(char ch, FontDef Font, SSD1306_COLOR color)
{
	uint32_t i, b, j;
	
	// Check remaining space on current line
	if (SSD1306_WIDTH <= (SSD1306.CurrentX + Font.FontWidth) ||
		SSD1306_HEIGHT <= (SSD1306.CurrentY + Font.FontHeight))
	{
		// Not enough space on current line
		return 0;
	}
	
	// Use the font to write
	for (i = 0; i < Font.FontHeight; i++)
	{
		b = Font.data[(ch - 32) * Font.FontHeight + i];
		for (j = 0; j < Font.FontWidth; j++)
		{
			if ((b << j) & 0x8000) 
			{
				ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR) color);
			} 
			else 
			{
				ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR)!color);
			}
		}
	}
	
	// The current space is now taken
	SSD1306.CurrentX += Font.FontWidth;
	
	// Return written char for validation
	return ch;
}

//
//  Write full string to screenbuffer
//
char ssd1306_WriteString(char* str, FontDef Font, SSD1306_COLOR color)
{
	// Write until null-byte
	while (*str) 
	{
		if (ssd1306_WriteChar(*str, Font, color) != *str)
		{
			// Char could not be written
			return *str;
		}
		
		// Next char
		str++;
	}
	
	// Everything ok
	return *str;
}

//
//	Position the cursor
//
void ssd1306_SetCursor(uint8_t x, uint8_t y) 
{
	SSD1306.CurrentX = x;
	SSD1306.CurrentY = y;
}

void ssd1306_WriteData(uint8_t* buffer, size_t buff_size) {
	HAL_I2C_Mem_Write(&SSD1306_I2C_PORT, SSD1306_I2C_ADDR, 0x40, 1, buffer, buff_size, HAL_MAX_DELAY);
}
