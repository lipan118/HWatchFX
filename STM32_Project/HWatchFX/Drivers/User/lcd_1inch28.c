/*****************************************************************************
* | File      	:   LCD_1inch28.c
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*                Used to shield the underlying layers of each master
*                and enhance portability
*----------------
* |	This version:   V1.0
* | Date        :   2020-12-06
* | Info        :   Basic version
*
******************************************************************************/
#include "lcd_1inch28.h"

#include <stdlib.h>		//itoa()
#include <stdio.h>
#include "string.h"

LCD_1IN28_ATTRIBUTES LCD_1IN28;

UBYTE lcd_data[2]={0}; //UWORD

static void Ui16ToUin8(uint16_t adata)
{
		lcd_data[0] = (adata >>8) & 0xFF; //前面(uint8_t)为强制类型转换
		lcd_data[1] = (adata & 0xFF);
}

/******************************************************************************
function :	Hardware reset
parameter:
******************************************************************************/
static void LCD_1IN28_Reset(void)
{
    LCD_1IN28_RST_1;
		CPU_TS_Tmr_Delay_MS(100);
    LCD_1IN28_RST_0;
    CPU_TS_Tmr_Delay_MS(100);
    LCD_1IN28_RST_1;
    CPU_TS_Tmr_Delay_MS(100);
}

/******************************************************************************
function :	send command
parameter:
     Reg : Command register
******************************************************************************/
static void LCD_1IN28_SendCommand(UBYTE Reg)
{
    LCD_1IN28_DC_0;
    LCD_1IN28_CS_0;
		LCD_WRITE_BYTE(&Reg, 1);
		CPU_TS_Tmr_Delay_US(1);
		//LCD_1IN28_CS_1;
}

/******************************************************************************
function :	send data
parameter:
    Data : Write data
******************************************************************************/
static void LCD_1IN28_SendData_8Bit(UBYTE Data)
{
    LCD_1IN28_DC_1
    LCD_1IN28_CS_0;
    LCD_WRITE_BYTE(&Data, 1);
		CPU_TS_Tmr_Delay_US(1);
    LCD_1IN28_CS_1;
}

/******************************************************************************
function :	send data
parameter:
    Data : Write data
******************************************************************************/
static void LCD_1IN28_SendData_16Bit(UWORD Data)
{
    LCD_1IN28_DC_1
    LCD_1IN28_CS_0;
		Ui16ToUin8(Data);
    LCD_WRITE_BYTE(lcd_data, 2);
		CPU_TS_Tmr_Delay_US(1);
    LCD_1IN28_CS_1;
}

/******************************************************************************
function :	Initialize the lcd register
parameter:
******************************************************************************/
static void LCD_1IN28_InitReg(void)
{
  LCD_1IN28_SendCommand(0xEF);
	LCD_1IN28_SendCommand(0xEB);
	LCD_1IN28_SendData_8Bit(0x14); 
	
  LCD_1IN28_SendCommand(0xFE);			 
	LCD_1IN28_SendCommand(0xEF); 

	LCD_1IN28_SendCommand(0xEB);	
	LCD_1IN28_SendData_8Bit(0x14); 

	LCD_1IN28_SendCommand(0x84);			
	LCD_1IN28_SendData_8Bit(0x40); 

	LCD_1IN28_SendCommand(0x85);			
	LCD_1IN28_SendData_8Bit(0xFF); 

	LCD_1IN28_SendCommand(0x86);			
	LCD_1IN28_SendData_8Bit(0xFF); 

	LCD_1IN28_SendCommand(0x87);			
	LCD_1IN28_SendData_8Bit(0xFF);

	LCD_1IN28_SendCommand(0x88);			
	LCD_1IN28_SendData_8Bit(0x0A);

	LCD_1IN28_SendCommand(0x89);			
	LCD_1IN28_SendData_8Bit(0x21); 

	LCD_1IN28_SendCommand(0x8A);			
	LCD_1IN28_SendData_8Bit(0x00); 

	LCD_1IN28_SendCommand(0x8B);			
	LCD_1IN28_SendData_8Bit(0x80); 

	LCD_1IN28_SendCommand(0x8C);			
	LCD_1IN28_SendData_8Bit(0x01); 

	LCD_1IN28_SendCommand(0x8D);			
	LCD_1IN28_SendData_8Bit(0x01); 

	LCD_1IN28_SendCommand(0x8E);			
	LCD_1IN28_SendData_8Bit(0xFF); 

	LCD_1IN28_SendCommand(0x8F);			
	LCD_1IN28_SendData_8Bit(0xFF); 


	LCD_1IN28_SendCommand(0xB6);
	LCD_1IN28_SendData_8Bit(0x00);
	LCD_1IN28_SendData_8Bit(0x20);

	LCD_1IN28_SendCommand(0x36);
	LCD_1IN28_SendData_8Bit(0x08);//Set as vertical screen

	LCD_1IN28_SendCommand(0x3A);			
	LCD_1IN28_SendData_8Bit(0x05); 


	LCD_1IN28_SendCommand(0x90);			
	LCD_1IN28_SendData_8Bit(0x08);
	LCD_1IN28_SendData_8Bit(0x08);
	LCD_1IN28_SendData_8Bit(0x08);
	LCD_1IN28_SendData_8Bit(0x08); 

	LCD_1IN28_SendCommand(0xBD);			
	LCD_1IN28_SendData_8Bit(0x06);
	
	LCD_1IN28_SendCommand(0xBC);			
	LCD_1IN28_SendData_8Bit(0x00);	

	LCD_1IN28_SendCommand(0xFF);			
	LCD_1IN28_SendData_8Bit(0x60);
	LCD_1IN28_SendData_8Bit(0x01);
	LCD_1IN28_SendData_8Bit(0x04);

	LCD_1IN28_SendCommand(0xC3);			
	LCD_1IN28_SendData_8Bit(0x13);
	LCD_1IN28_SendCommand(0xC4);			
	LCD_1IN28_SendData_8Bit(0x13);

	LCD_1IN28_SendCommand(0xC9);			
	LCD_1IN28_SendData_8Bit(0x22);

	LCD_1IN28_SendCommand(0xBE);			
	LCD_1IN28_SendData_8Bit(0x11); 

	LCD_1IN28_SendCommand(0xE1);			
	LCD_1IN28_SendData_8Bit(0x10);
	LCD_1IN28_SendData_8Bit(0x0E);

	LCD_1IN28_SendCommand(0xDF);			
	LCD_1IN28_SendData_8Bit(0x21);
	LCD_1IN28_SendData_8Bit(0x0c);
	LCD_1IN28_SendData_8Bit(0x02);

	LCD_1IN28_SendCommand(0xF0);   
	LCD_1IN28_SendData_8Bit(0x45);
	LCD_1IN28_SendData_8Bit(0x09);
	LCD_1IN28_SendData_8Bit(0x08);
	LCD_1IN28_SendData_8Bit(0x08);
	LCD_1IN28_SendData_8Bit(0x26);
 	LCD_1IN28_SendData_8Bit(0x2A);

 	LCD_1IN28_SendCommand(0xF1);    
 	LCD_1IN28_SendData_8Bit(0x43);
 	LCD_1IN28_SendData_8Bit(0x70);
 	LCD_1IN28_SendData_8Bit(0x72);
 	LCD_1IN28_SendData_8Bit(0x36);
 	LCD_1IN28_SendData_8Bit(0x37);  
 	LCD_1IN28_SendData_8Bit(0x6F);


 	LCD_1IN28_SendCommand(0xF2);   
 	LCD_1IN28_SendData_8Bit(0x45);
 	LCD_1IN28_SendData_8Bit(0x09);
 	LCD_1IN28_SendData_8Bit(0x08);
 	LCD_1IN28_SendData_8Bit(0x08);
 	LCD_1IN28_SendData_8Bit(0x26);
 	LCD_1IN28_SendData_8Bit(0x2A);

 	LCD_1IN28_SendCommand(0xF3);   
 	LCD_1IN28_SendData_8Bit(0x43);
 	LCD_1IN28_SendData_8Bit(0x70);
 	LCD_1IN28_SendData_8Bit(0x72);
 	LCD_1IN28_SendData_8Bit(0x36);
 	LCD_1IN28_SendData_8Bit(0x37); 
 	LCD_1IN28_SendData_8Bit(0x6F);

	LCD_1IN28_SendCommand(0xED);	
	LCD_1IN28_SendData_8Bit(0x1B); 
	LCD_1IN28_SendData_8Bit(0x0B); 

	LCD_1IN28_SendCommand(0xAE);			
	LCD_1IN28_SendData_8Bit(0x77);
	
	LCD_1IN28_SendCommand(0xCD);			
	LCD_1IN28_SendData_8Bit(0x63);		


	LCD_1IN28_SendCommand(0x70);			
	LCD_1IN28_SendData_8Bit(0x07);
	LCD_1IN28_SendData_8Bit(0x07);
	LCD_1IN28_SendData_8Bit(0x04);
	LCD_1IN28_SendData_8Bit(0x0E); 
	LCD_1IN28_SendData_8Bit(0x0F); 
	LCD_1IN28_SendData_8Bit(0x09);
	LCD_1IN28_SendData_8Bit(0x07);
	LCD_1IN28_SendData_8Bit(0x08);
	LCD_1IN28_SendData_8Bit(0x03);

	LCD_1IN28_SendCommand(0xE8);			
	LCD_1IN28_SendData_8Bit(0x34);

	LCD_1IN28_SendCommand(0x62);			
	LCD_1IN28_SendData_8Bit(0x18);
	LCD_1IN28_SendData_8Bit(0x0D);
	LCD_1IN28_SendData_8Bit(0x71);
	LCD_1IN28_SendData_8Bit(0xED);
	LCD_1IN28_SendData_8Bit(0x70); 
	LCD_1IN28_SendData_8Bit(0x70);
	LCD_1IN28_SendData_8Bit(0x18);
	LCD_1IN28_SendData_8Bit(0x0F);
	LCD_1IN28_SendData_8Bit(0x71);
	LCD_1IN28_SendData_8Bit(0xEF);
	LCD_1IN28_SendData_8Bit(0x70); 
	LCD_1IN28_SendData_8Bit(0x70);

	LCD_1IN28_SendCommand(0x63);			
	LCD_1IN28_SendData_8Bit(0x18);
	LCD_1IN28_SendData_8Bit(0x11);
	LCD_1IN28_SendData_8Bit(0x71);
	LCD_1IN28_SendData_8Bit(0xF1);
	LCD_1IN28_SendData_8Bit(0x70); 
	LCD_1IN28_SendData_8Bit(0x70);
	LCD_1IN28_SendData_8Bit(0x18);
	LCD_1IN28_SendData_8Bit(0x13);
	LCD_1IN28_SendData_8Bit(0x71);
	LCD_1IN28_SendData_8Bit(0xF3);
	LCD_1IN28_SendData_8Bit(0x70); 
	LCD_1IN28_SendData_8Bit(0x70);

	LCD_1IN28_SendCommand(0x64);			
	LCD_1IN28_SendData_8Bit(0x28);
	LCD_1IN28_SendData_8Bit(0x29);
	LCD_1IN28_SendData_8Bit(0xF1);
	LCD_1IN28_SendData_8Bit(0x01);
	LCD_1IN28_SendData_8Bit(0xF1);
	LCD_1IN28_SendData_8Bit(0x00);
	LCD_1IN28_SendData_8Bit(0x07);

	LCD_1IN28_SendCommand(0x66);			
	LCD_1IN28_SendData_8Bit(0x3C);
	LCD_1IN28_SendData_8Bit(0x00);
	LCD_1IN28_SendData_8Bit(0xCD);
	LCD_1IN28_SendData_8Bit(0x67);
	LCD_1IN28_SendData_8Bit(0x45);
	LCD_1IN28_SendData_8Bit(0x45);
	LCD_1IN28_SendData_8Bit(0x10);
	LCD_1IN28_SendData_8Bit(0x00);
	LCD_1IN28_SendData_8Bit(0x00);
	LCD_1IN28_SendData_8Bit(0x00);

	LCD_1IN28_SendCommand(0x67);			
	LCD_1IN28_SendData_8Bit(0x00);
	LCD_1IN28_SendData_8Bit(0x3C);
	LCD_1IN28_SendData_8Bit(0x00);
	LCD_1IN28_SendData_8Bit(0x00);
	LCD_1IN28_SendData_8Bit(0x00);
	LCD_1IN28_SendData_8Bit(0x01);
	LCD_1IN28_SendData_8Bit(0x54);
	LCD_1IN28_SendData_8Bit(0x10);
	LCD_1IN28_SendData_8Bit(0x32);
	LCD_1IN28_SendData_8Bit(0x98);

	LCD_1IN28_SendCommand(0x74);			
	LCD_1IN28_SendData_8Bit(0x10);	
	LCD_1IN28_SendData_8Bit(0x85);	
	LCD_1IN28_SendData_8Bit(0x80);
	LCD_1IN28_SendData_8Bit(0x00); 
	LCD_1IN28_SendData_8Bit(0x00); 
	LCD_1IN28_SendData_8Bit(0x4E);
	LCD_1IN28_SendData_8Bit(0x00);					
	
  LCD_1IN28_SendCommand(0x98);			
	LCD_1IN28_SendData_8Bit(0x3e);
	LCD_1IN28_SendData_8Bit(0x07);

	LCD_1IN28_SendCommand(0x35);	
	LCD_1IN28_SendCommand(0x21);

	LCD_1IN28_SendCommand(0x11);
	CPU_TS_Tmr_Delay_MS(120);
	LCD_1IN28_SendCommand(0x29);
	CPU_TS_Tmr_Delay_MS(20);
}

/********************************************************************************
function:	Set the resolution and scanning method of the screen
parameter:
		Scan_dir:   Scan direction
********************************************************************************/
static void LCD_1IN28_SetAttributes(UBYTE Scan_dir)
{
    //Get the screen scan direction
    LCD_1IN28.SCAN_DIR = Scan_dir;
    UBYTE MemoryAccessReg = 0x08;

    //Get GRAM and LCD width and height
    if(Scan_dir == HORIZONTAL) {
        LCD_1IN28.HEIGHT	= LCD_1IN28_HEIGHT;
        LCD_1IN28.WIDTH   = LCD_1IN28_WIDTH;
        MemoryAccessReg = 0XC8;
    } else {
        LCD_1IN28.HEIGHT	= LCD_1IN28_WIDTH;
        LCD_1IN28.WIDTH   = LCD_1IN28_HEIGHT;
        MemoryAccessReg = 0X68;
    }

    // Set the read / write scan direction of the frame memory
    LCD_1IN28_SendCommand(0x36); //MX, MY, RGB mode
    LCD_1IN28_SendData_8Bit(MemoryAccessReg);	//0x08 set RGB
}

/********************************************************************************
function :	Initialize the lcd
parameter:
********************************************************************************/
void LCD_1IN28_Init(UBYTE Scan_dir)
{

    //Hardware reset
    LCD_1IN28_Reset();

    //Set the resolution and scanning method of the screen
    LCD_1IN28_SetAttributes(Scan_dir);
    
    //Set the initialization register
    LCD_1IN28_InitReg();

}

/********************************************************************************
function:	Sets the start position and size of the display area
parameter:
		Xstart 	:   X direction Start coordinates
		Ystart  :   Y direction Start coordinates
		Xend    :   X direction end coordinates
		Yend    :   Y direction end coordinates
********************************************************************************/
void LCD_1IN28_SetWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend)
{
		//set the X coordinates
    LCD_1IN28_SendCommand(0x2A);
    LCD_1IN28_SendData_16Bit(Xstart);
    LCD_1IN28_SendData_16Bit(Xend);
	
		//set the Y coordinates
    LCD_1IN28_SendCommand(0x2B);
	  LCD_1IN28_SendData_16Bit(Ystart);
    LCD_1IN28_SendData_16Bit(Yend);

		LCD_1IN28_SendCommand(0x2C);

}

/******************************************************************************
function :	Clear screen
parameter:
******************************************************************************/
void LCD_1IN28_Clear(UWORD Color)
{
	UWORD i,j;
	LCD_1IN28_SetWindows(0, 0, LCD_1IN28_WIDTH-1, LCD_1IN28_HEIGHT-1);

	LCD_1IN28_DC_1

	for(i = 0; i < LCD_1IN28_WIDTH; i++){
		for(j = 0; j < LCD_1IN28_HEIGHT; j++){
			Ui16ToUin8(Color);
			LCD_WRITE_BYTE(lcd_data, 2);
			CPU_TS_Tmr_Delay_US(1);
		}
	}
}

/******************************************************************************
function :	Sends the image buffer in RAM to displays
parameter:
******************************************************************************/
void LCD_1IN28_Display(UWORD *Image)
{
    UWORD i,j, color;
    LCD_1IN28_SetWindows(0, 0, LCD_1IN28_WIDTH-1, LCD_1IN28_HEIGHT-1);
		LCD_1IN28_DC_1
    for(i = 0; i < LCD_1IN28_WIDTH; i++){
      for(j = 0; j < LCD_1IN28_HEIGHT; j++){
				color = (*Image+i*LCD_1IN28_WIDTH+j);
				Ui16ToUin8(color);
				LCD_WRITE_BYTE(lcd_data, 2);
      }
    }
}

void LCD_1IN28_DisplayWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD *Image)
{
    // display
    UDOUBLE Addr = 0;
		UDOUBLE totalpoint = (Xend-Xstart+1)*(Yend-Ystart+1);
    LCD_1IN28_SetWindows(Xstart, Ystart, Xend , Yend);
    LCD_1IN28_DC_1
		LCD_1IN28_CS_0
	
		//SPI
//		for(Addr=0;Addr<totalpoint;Addr++)
//		{
//			LCD_1IN28_SendData_16Bit(*(Image+Addr));
//		}
		
		//DMA
		LCD_WRITE_PACK((uint8_t*)Image, totalpoint<<1);
	
}

/******************************************************************************
function: Draw a point
parameter	:
	    X	: 	Set the X coordinate
	    Y	:	Set the Y coordinate
	  Color :	Set the color
******************************************************************************/
void LCD_1IN28_DrawPaint(UWORD x, UWORD y, UWORD Color)
{
	LCD_1IN28_SetWindows(x,y,x,y);
	LCD_1IN28_SendData_16Bit(Color); 	    
}

/*******************************************************************************
function:
	Setting backlight
parameter	:
	  value : Range 0~1000   Duty cycle is value/1000	
*******************************************************************************/
void LCD_1IN28_SetBackLight(UWORD Value)
{
//	DEV_Set_PWM(Value);
}

