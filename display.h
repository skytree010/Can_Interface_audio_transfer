/*************************************************************************
 *
 * 
 *    File name   : display.h
 *    Description : for control display
 *					init, change background img, etc.
 *					moduled function
 *
 **************************************************************************/
#ifndef __DISPLAY_H
#define __DISPLAY_H
#include "arm_comm.h"
#include "bmp.h"
#include "bmp32.h"

#define RES_BACK_IMG1         (__resources_bin)
#define RES_BACK_IMG_SIZE1     8751
#define RES_BACK_IMG2         (__resources_bin + RES_BACK_IMG_SIZE1)
#define RES_BACK_IMG_SIZE2     259855
#define RES_BACK_IMG3         (__resources_bin + RES_BACK_IMG_SIZE1 + RES_BACK_IMG_SIZE2)
#define RES_BACK_IMG_SIZE3     258210
#define RES_BACK_IMG4         (__resources_bin + RES_BACK_IMG_SIZE1 + RES_BACK_IMG_SIZE2 + RES_BACK_IMG_SIZE3)
#define RES_BACK_IMG_SIZE4     12264
#define RES_BACK_IMG5         (__resources_bin + RES_BACK_IMG_SIZE1 + RES_BACK_IMG_SIZE2 + RES_BACK_IMG_SIZE3 + RES_BACK_IMG_SIZE4)
#define RES_BACK_IMG_SIZE5     188925

#define RES_IMG_COUNT 5
#define MAX_STRING_SIZE 256

extern uint8_t __resources_bin[];

extern FontType_t Terminal_6_8_6;
extern FontType_t Terminal_9_12_6;
extern FontType_t Terminal_18_24_12;


struct imgstruct
{
	uint8_t *usedimg;
	pPic_t pPic;
};


Boolean InitDisplay();

void GetTouchLocation(uint16_t *X, uint16_t *Y);

Boolean ChangeImg(uint8_t *imglocation);

Boolean PrintMessage(uint32_t X, uint32_t Y, const char *fmt, ...);
#endif