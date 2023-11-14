/*
   This file is provided under the LGPL license ver 2.1
   https://github.com/kmorimatsu
*/

#include <stdlib.h>
#include "GUI_Paint.h"
#include "lib/e-Paper/EPD_2in13_V4.h"

static char imageBuffer[((EPD_2in13_V4_WIDTH % 8 == 0)? (EPD_2in13_V4_WIDTH / 8 ): (EPD_2in13_V4_WIDTH / 8 + 1)) * EPD_2in13_V4_HEIGHT];

int EPD_2in13_V4_draw(char battery, int humidity, float celsius, bool clear){
	int i;
	// Battery
	char batteryString[2];
	batteryString[0]=0x20+battery;
	batteryString[1]=0;
	
	// Humidity
	char* h100;
	char h[3];
	if (humidity<0) {
		h100="?";
		h[0]='?';
		h[1]='?';
		h[2]=0;
	} else if (99<humidity) {
		h100="1";
		h[0]='0';
		h[1]='0';
		h[2]=0;
	} else {
		h100=" ";
		if (humidity<10) sprintf(h," %d",humidity);
		else sprintf(h,"%d",humidity);
	}
	
	// Temperature
	char cfstring[14]="     C      F";
	float fahrenheit=1.8*celsius+32.0;
	int c10=(int)(celsius*10);
	int f10=(int)(fahrenheit*10+0.5);
	if (c10<0) {
		cfstring[0]='-';
		c10=-c10;
	} else {
		cfstring[0]=' ';
	} 
	if (999<c10) {
		cfstring[1]='?';
	} else if (c10<100) sprintf(cfstring+1," %d.%dC ",c10/10,c10%10);
	else sprintf(cfstring+1,"%d.%dC ",c10/10,c10%10);
	if (9999<f10 || f10<0) {
		cfstring[7]='?';
	} else if (999<f10) sprintf(cfstring+7,"%d.%dF ",f10/10,f10%10);
	else if (99<f10) sprintf(cfstring+7," %d.%dF ",f10/10,f10%10);
	else sprintf(cfstring+7,"  %d.%dF ",f10/10,f10%10);

	DEV_Module_Init();
	EPD_2in13_V4_Init();
	if (clear) {
		EPD_2in13_V4_Clear();
		DEV_Delay_ms(500);
	}

	//Create a new image cache named IMAGE_BW and fill it with white
	UBYTE *BlackImage = (UBYTE *)imageBuffer;
	Paint_NewImage(BlackImage, EPD_2in13_V4_WIDTH, EPD_2in13_V4_HEIGHT, 90, WHITE);

	//Select Image
	Paint_SelectImage(BlackImage);
	Paint_Clear(WHITE);

	/*Horizontal screen*/
	//1.Draw black image
	Paint_SelectImage(BlackImage);
	Paint_Clear(WHITE);
	Paint_DrawString_EN(0, 20, h100, &Font82, WHITE, BLACK);
	Paint_DrawString_EN(0+56, 0, h, &Font107, WHITE, BLACK);
	Paint_DrawString_EN(0+56+70*2, 20, "%", &Font82, WHITE, BLACK);
	Paint_DrawString_EN(17*5-3,98,"o",&Font8, WHITE, BLACK);
	Paint_DrawString_EN(17*12-3,98,"o",&Font8, WHITE, BLACK);
	Paint_DrawString_EN(0,98,cfstring,&Font24, WHITE, BLACK);
	Paint_DrawString_EN(0,0,batteryString,&Battery32, WHITE, BLACK);

	EPD_2in13_V4_Display(BlackImage);

	EPD_2in13_V4_Sleep();
	BlackImage = NULL;
	DEV_Delay_ms(2000);//important, at least 2s
	// close 5V
	DEV_Module_Exit();

	return 0;
}
