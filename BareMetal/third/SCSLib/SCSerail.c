/*
 * SCServo.c
 * ���ض��Ӳ���ӿڲ����
 * ����: 2024.12.2
 * ����: txl
 */
#include <stdint.h>
#include "usart.h"

static uint8_t wBuf[128];
static uint8_t wLen = 0;

void ftUart_Send(uint8_t *nDat, int nLen);
int  ftUart_Read(uint8_t *nDat, int nLen);
void ftBus_Delay(void);

// UART �������ݽӿ�
int readSCS(unsigned char *nDat, int nLen)
{
    return FTUart_Read(nDat, nLen);
}

// UART �������ݽӿ�
int writeSCS(unsigned char *nDat, int nLen)
{
    while (nLen--) {
        if (wLen < sizeof(wBuf)) {
            wBuf[wLen] = *nDat;
            wLen++;
            nDat++;
        }
    }
    return wLen;
}

//���ջ�����ˢ��
void rFlushSCS()
{
    FTBus_Delay();
}

//���ͻ�����ˢ��
void wFlushSCS()
{
    if (wLen) {
        FTUart_Send(wBuf, wLen);
        wLen = 0;
    }
}
