/*************************************************************************
 *
 * 
 *    File name   : display.c
 *    Description : for control display
 *					init, change background img, etc.
 *					moduled function
 *
 **************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include "includes.h"
#include "display.h"

struct imgstruct stimg[RES_IMG_COUNT];

Boolean InitDisplay()
{
	STM_BacklightInit();

	SDRAM_Init();

	if (NULL == (stimg[0].pPic = GLCD_LoadPNG(RES_BACK_IMG1, RES_BACK_IMG_SIZE1)))
	{
		while (1);
	}
	stimg[0].usedimg = RES_BACK_IMG1;

	GLCD_Init(stimg[0].pPic, NULL);
	GLCD_Ctrl(TRUE);
        
	GLCD_SetFont(&Terminal_18_24_12, GLCD_COLOR_BLACK, GLCD_COLOR_WHITE, Layer1);

	STM_BacklightOn();

	I2C1_Init();
	if (!STEMPE811_Init())
	{
		while (1);
	}
	touch_control_init();

	PrintMessage(100, 400, "        ");
        if (NULL == (stimg[1].pPic = GLCD_LoadPNG(RES_BACK_IMG2, RES_BACK_IMG_SIZE2)))
	{
		while (1);
	}
	stimg[1].usedimg = RES_BACK_IMG2;
        PrintMessage(100, 400, "                ");
	if (NULL == (stimg[2].pPic = GLCD_LoadPNG(RES_BACK_IMG3, RES_BACK_IMG_SIZE3)))
	{
		while (1);
	}
	stimg[2].usedimg = RES_BACK_IMG3;
        PrintMessage(100, 400, "                        ");
	if (NULL == (stimg[3].pPic = GLCD_LoadPNG(RES_BACK_IMG4, RES_BACK_IMG_SIZE4)))
	{
		while (1);
	}
	stimg[3].usedimg = RES_BACK_IMG4;
        PrintMessage(100, 400, "                                ");
        return 1;
}

void GetTouchLocation(uint16_t *X, uint16_t *Y)
{
	Int8U touch_get[4];
	uint32_t xyz_dt, x_dt, y_dt;
	uint16_t x_scale = 800, y_scale = 480;

	i2c_touch_get_data(touch_get);
	xyz_dt = (touch_get[0] << 24) | (touch_get[1] << 16) | (touch_get[2] << 8) | (touch_get[3] << 0);
	x_dt = ((xyz_dt >> 20) & 0x00000FFF);
	y_dt = ((xyz_dt >> 8) & 0x00000FFF);

	if (y_dt < 4000)
	{
		y_dt = 4000 - y_dt;
	}

	*Y = y_dt / 8;

	if (*Y <= 0)
	{
		*Y = 0;
	}
	else if (*Y > y_scale)
	{
		*Y = y_scale - 1;
	}
	x_dt -= 40;
	*X = x_dt / 5;
	if (*X <= 0)
	{
		*X = 0;
	}
	else if (*X > x_scale)
	{
		*X = x_scale - 1;
	}
}

Boolean ChangeImg(uint8_t *imglocation)
{
	int i;
	for (i = 0; i < RES_IMG_COUNT; i++)
	{
		if (stimg[i].usedimg == imglocation)
		{
			GLCD_ShowPic(0, 0, stimg[i].pPic, 0, Layer1);
			return 1;
		}
	}
	return 0;
}

Boolean PrintMessage(uint32_t X, uint32_t Y, const char *fmt, ...)
{
	va_list ap;
	char s[MAX_STRING_SIZE];
	char *p_char = s;
	va_start(ap, fmt);
	vsprintf(s, fmt, ap);
	va_end(ap);
	GLCD_SetWindow(X, Y, X + 600, Y + 40, Layer1);
	GLCD_TextSetPos(0, 0, Layer1);
	while (0 != *p_char)
	{
		if (-1 == GLCD_putchar(*p_char++, Layer1))
		{
			break;
		}
	}
        return 1;
}