/*
 * SCS.h
 * ���ش��ж��ͨ��Э�����
 * ����: 2025.3.3
 * ����: txl
 */

#ifndef _SCS_H
#define _SCS_H

#include <stdint.h>

enum SCS_ERR_LIST {
    SCS_ERR_NO_REPLY = 1,
    SCS_ERR_CRC_CMP = 2,
    SCS_ERR_SLAVE_ID = 3,
    SCS_ERR_BUFF_LEN = 4,
};

extern int  genWrite(uint8_t ID, uint8_t MemAddr, uint8_t *nDat, uint8_t nLen);                 //��ͨдָ��
extern int  regWrite(uint8_t ID, uint8_t MemAddr, uint8_t *nDat, uint8_t nLen);                 //�첽дָ��
extern int  regAction(uint8_t ID);                                                              //�첽дִ����
extern void syncWrite(uint8_t ID[], uint8_t IDN, uint8_t MemAddr, uint8_t *nDat, uint8_t nLen); //ͬ��дָ��
extern int  writeByte(uint8_t ID, uint8_t MemAddr, uint8_t bDat);                               //д1���ֽ�
extern int  writeWord(uint8_t ID, uint8_t MemAddr, uint16_t wDat);                              //д2���ֽ�
extern int  Read(uint8_t ID, uint8_t MemAddr, uint8_t *nData, uint8_t nLen);                    //��ָ��
extern int  readByte(uint8_t ID, uint8_t MemAddr);                                              //��1���ֽ�
extern int  readWord(uint8_t ID, uint8_t MemAddr);                                              //��2���ֽ�
extern int  Ping(uint8_t ID);                                                                   // Pingָ��

extern void writeBuf(uint8_t ID, uint8_t MemAddr, uint8_t *nDat, uint8_t nLen, uint8_t Fun);
extern void Host2SCS(uint8_t *DataL, uint8_t *DataH, int Data); // 1��16λ�����Ϊ2��8λ��
extern int  SCS2Host(uint8_t DataL, uint8_t DataH);             // 2��8λ�����Ϊ1��16λ��
extern int  Ack(uint8_t ID);                                    //Ӧ��
extern int  checkHead(void);                                    //֡ͷ���

extern void    setEnd(uint8_t _End);
extern uint8_t getEnd(void);
extern void    setLevel(uint8_t _Level);
extern int     getState(void);
extern int     getLastError(void);

//Ӳ���ӿں���
extern int      writeSCS(uint8_t *nDat, int nLen);
extern int      readSCS(uint8_t *nDat, int nLen);
extern uint32_t FTBus_Delay(void);
extern uint32_t rFlushSCS(void);
extern void     wFlushSCS(void);

#endif
