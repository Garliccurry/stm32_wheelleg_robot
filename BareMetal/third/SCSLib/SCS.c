/*
 * SCS.c
 * SCS���ж��Э�����
 * ����: 2025.3.3
 * ����: txl
 */

#include "INST.h"
#include "SCS.h"

static uint8_t Level = 1; //������صȼ�1,Ĭ��дָ���Ӧ��
static uint8_t End = 0;   //��������С�˽ṹ,Ĭ��С�˴洢��ʽ
static uint8_t u8Status;  //���״̬
static uint8_t u8Error;   //ͨ��״̬

void setEnd(uint8_t _End)
{
    End = _End;
}

uint8_t getEnd(void)
{
    return End;
}

void setLevel(uint8_t _Level)
{
    Level = _Level;
}

int getState(void)
{
    return u8Status;
}

int getLastError(void)
{
    return u8Error;
}

// 1��16λ�����Ϊ2��8λ��
// DataLΪ��λ��DataHΪ��λ
void Host2SCS(uint8_t *DataL, uint8_t *DataH, int Data)
{
    if (End) {
        *DataL = (Data >> 8);
        *DataH = (Data & 0xff);
    } else {
        *DataH = (Data >> 8);
        *DataL = (Data & 0xff);
    }
}

// 2��8λ�����Ϊ1��16λ��
// DataLΪ��λ��DataHΪ��λ
int SCS2Host(uint8_t DataL, uint8_t DataH)
{
    int Data;
    if (End) {
        Data = DataL;
        Data <<= 8;
        Data |= DataH;
    } else {
        Data = DataH;
        Data <<= 8;
        Data |= DataL;
    }
    return Data;
}

void writeBuf(uint8_t ID, uint8_t MemAddr, uint8_t *nDat, uint8_t nLen, uint8_t Fun)
{
    uint8_t i;
    uint8_t msgLen = 2;
    uint8_t bBuf[6];
    uint8_t CheckSum = 0;
    bBuf[0] = 0xff;
    bBuf[1] = 0xff;
    bBuf[2] = ID;
    bBuf[4] = Fun;
    if (nDat) {
        msgLen += nLen + 1;
        bBuf[3] = msgLen;
        bBuf[5] = MemAddr;
        writeSCS(bBuf, 6);

    } else {
        bBuf[3] = msgLen;
        writeSCS(bBuf, 5);
    }
    CheckSum = ID + msgLen + Fun + MemAddr;
    if (nDat) {
        for (i = 0; i < nLen; i++) {
            CheckSum += nDat[i];
        }
        writeSCS(nDat, nLen);
    }
    CheckSum = ~CheckSum;
    writeSCS(&CheckSum, 1);
}

//��ͨдָ��
//���ID��MemAddr�ڴ����ַ��д�����ݣ�д�볤��
int genWrite(uint8_t ID, uint8_t MemAddr, uint8_t *nDat, uint8_t nLen)
{
    rFlushSCS();
    writeBuf(ID, MemAddr, nDat, nLen, INST_WRITE);
    wFlushSCS();
    return Ack(ID);
}

//�첽дָ��
//���ID��MemAddr�ڴ����ַ��д�����ݣ�д�볤��
int regWrite(uint8_t ID, uint8_t MemAddr, uint8_t *nDat, uint8_t nLen)
{
    rFlushSCS();
    writeBuf(ID, MemAddr, nDat, nLen, INST_REG_WRITE);
    wFlushSCS();
    return Ack(ID);
}

//�첽дִ����
int regAction(uint8_t ID)
{
    rFlushSCS();
    writeBuf(ID, 0, NULL, 0, INST_REG_ACTION);
    wFlushSCS();
    return Ack(ID);
}
#define WL_OK 0
//ͬ��дָ��
//���ID[]���飬IDN���鳤�ȣ�MemAddr�ڴ����ַ��д�����ݣ�д�볤��
void syncWrite(uint8_t ID[], uint8_t IDN, uint8_t MemAddr, uint8_t *nDat, uint8_t nLen)
{
    uint8_t mesLen = ((nLen + 1) * IDN + 4);
    uint8_t Sum = 0;
    uint8_t bBuf[7];
    uint8_t i, j;

    bBuf[0] = 0xff;
    bBuf[1] = 0xff;
    bBuf[2] = 0xfe;
    bBuf[3] = mesLen;
    bBuf[4] = INST_SYNC_WRITE;
    bBuf[5] = MemAddr;
    bBuf[6] = nLen;

    if (rFlushSCS() != WL_OK) {
        return;
    }
    writeSCS(bBuf, 7);

    Sum = 0xfe + mesLen + INST_SYNC_WRITE + MemAddr + nLen;

    for (i = 0; i < IDN; i++) {
        writeSCS(&ID[i], 1);
        writeSCS(nDat + i * nLen, nLen);
        Sum += ID[i];
        for (j = 0; j < nLen; j++) {
            Sum += nDat[i * nLen + j];
        }
    }
    Sum = ~Sum;
    writeSCS(&Sum, 1);
    wFlushSCS();
}

int writeByte(uint8_t ID, uint8_t MemAddr, uint8_t bDat)
{
    rFlushSCS();
    writeBuf(ID, MemAddr, &bDat, 1, INST_WRITE);
    wFlushSCS();
    return Ack(ID);
}

int writeWord(uint8_t ID, uint8_t MemAddr, uint16_t wDat)
{
    uint8_t buf[2];
    Host2SCS(buf + 0, buf + 1, wDat);
    rFlushSCS();
    writeBuf(ID, MemAddr, buf, 2, INST_WRITE);
    wFlushSCS();
    return Ack(ID);
}

//��ָ��
//���ID��MemAddr�ڴ����ַ����������nData�����ݳ���nLen
int Read(uint8_t ID, uint8_t MemAddr, uint8_t *nData, uint8_t nLen)
{
    int     Size;
    uint8_t bBuf[4];
    uint8_t calSum;
    uint8_t i;
    rFlushSCS();
    writeBuf(ID, MemAddr, &nLen, 1, INST_READ);
    wFlushSCS();
    u8Error = 0;
    if (!checkHead()) {
        u8Error = SCS_ERR_NO_REPLY;
        return 0;
    }
    if (readSCS(bBuf, 3) != 3) {
        u8Error = SCS_ERR_NO_REPLY;
        return 0;
    }
    if (bBuf[0] != ID && ID != 0xfe) {
        u8Error = SCS_ERR_SLAVE_ID;
        return 0;
    }
    if (bBuf[1] != (nLen + 2)) {
        u8Error = SCS_ERR_BUFF_LEN;
        return 0;
    }
    Size = readSCS(nData, nLen);
    if (Size != nLen) {
        u8Error = SCS_ERR_NO_REPLY;
        return 0;
    }
    if (readSCS(bBuf + 3, 1) != 1) {
        u8Error = SCS_ERR_NO_REPLY;
        return 0;
    }
    calSum = bBuf[0] + bBuf[1] + bBuf[2];
    for (i = 0; i < Size; i++) {
        calSum += nData[i];
    }
    calSum = ~calSum;
    if (calSum != bBuf[3]) {
        u8Error = SCS_ERR_CRC_CMP;
        return 0;
    }
    u8Status = bBuf[2];
    return Size;
}

//��1�ֽڣ���ʱ����-1
int readByte(uint8_t ID, uint8_t MemAddr)
{
    uint8_t bDat;
    int     Size = Read(ID, MemAddr, &bDat, 1);
    if (Size != 1) {
        return -1;
    } else {
        return bDat;
    }
}

//��2�ֽڣ���ʱ����-1
int readWord(uint8_t ID, uint8_t MemAddr)
{
    uint8_t  nDat[2];
    int      Size;
    uint16_t wDat;
    Size = Read(ID, MemAddr, nDat, 2);
    if (Size != 2)
        return -1;
    wDat = SCS2Host(nDat[0], nDat[1]);
    return wDat;
}

// Pingָ����ض��ID����ʱ����-1
int Ping(uint8_t ID)
{
    uint8_t bBuf[4];
    uint8_t calSum;
    rFlushSCS();
    writeBuf(ID, 0, NULL, 0, INST_PING);
    wFlushSCS();
    u8Status = 0;
    if (!checkHead()) {
        u8Error = SCS_ERR_NO_REPLY;
        return -1;
    }
    u8Error = 0;
    if (readSCS(bBuf, 4) != 4) {
        u8Error = SCS_ERR_NO_REPLY;
        return -1;
    }
    if (bBuf[0] != ID && ID != 0xfe) {
        u8Error = SCS_ERR_SLAVE_ID;
        return -1;
    }
    if (bBuf[1] != 2) {
        u8Error = SCS_ERR_BUFF_LEN;
        return -1;
    }
    calSum = ~(bBuf[0] + bBuf[1] + bBuf[2]);
    if (calSum != bBuf[3]) {
        u8Error = SCS_ERR_CRC_CMP;
        return -1;
    }
    u8Status = bBuf[2];
    return bBuf[0];
}

int checkHead(void)
{
    uint8_t bDat;
    uint8_t bBuf[2] = {0, 0};
    uint8_t Cnt = 0;
    while (1) {
        if (!readSCS(&bDat, 1)) {
            return 0;
        }
        bBuf[1] = bBuf[0];
        bBuf[0] = bDat;
        if (bBuf[0] == 0xff && bBuf[1] == 0xff) {
            break;
        }
        Cnt++;
        if (Cnt > 10) {
            return 0;
        }
    }
    return 1;
}

//ָ��Ӧ��
int Ack(uint8_t ID)
{
    uint8_t bBuf[4];
    uint8_t calSum;
    u8Error = 0;
    if (ID != 0xfe && Level) {
        if (!checkHead()) {
            u8Error = SCS_ERR_NO_REPLY;
            return 0;
        }
        u8Status = 0;
        if (readSCS(bBuf, 4) != 4) {
            u8Error = SCS_ERR_NO_REPLY;
            return 0;
        }
        if (bBuf[0] != ID) {
            u8Error = SCS_ERR_SLAVE_ID;
            return 0;
        }
        if (bBuf[1] != 2) {
            u8Error = SCS_ERR_BUFF_LEN;
            return 0;
        }
        calSum = ~(bBuf[0] + bBuf[1] + bBuf[2]);
        if (calSum != bBuf[3]) {
            u8Error = SCS_ERR_CRC_CMP;
            return 0;
        }
        u8Status = bBuf[2];
    }
    return 1;
}
