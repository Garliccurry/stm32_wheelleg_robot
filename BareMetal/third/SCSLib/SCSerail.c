/*
 * SCServo.c
 * 飞特舵机硬件接口层程序
 * 日期: 2024.12.2
 * 作者: txl
 */
#include <stdint.h>
#include "usart.h"

static uint8_t wBuf[128];
static uint8_t wLen = 0;

void ftUart_Send(uint8_t *nDat, int nLen);
int ftUart_Read(uint8_t *nDat, int nLen);
void ftBus_Delay(void);

//UART 接收数据接口
int readSCS(unsigned char *nDat, int nLen)
{
	return lFTUart_Read(nDat, nLen);
}

//UART 发送数据接口
int writeSCS(unsigned char *nDat, int nLen)
{
	while(nLen--){
		if(wLen<sizeof(wBuf)){
			wBuf[wLen] = *nDat;
			wLen++;
			nDat++;
		}
	}
	return wLen;
}

//接收缓冲区刷新
void rFlushSCS()
{
	vFTBus_Delay();
}

//发送缓冲区刷新
void wFlushSCS()
{
	if(wLen){
		vFTUart_Send(wBuf, wLen);
		wLen = 0;
	}
}
