#include <stdint.h>
#include "cirbuf.h"
void CirBuf_AsDataInit(CirBuf_t *CirBuf, uint32_t len, AsRawData_t *buf)
{
    CirBuf->r_pos = CirBuf->w_pos = 0;
    CirBuf->len = len;
    CirBuf->asbuf = buf;
}

uint32_t CirBuf_AsDataWrite(CirBuf_t *CirBuf, uint16_t dataL, uint16_t dataR)
{
    uint32_t next_w = (CirBuf->w_pos + 1) % CirBuf->len; // 优化回绕计算
    if (next_w == CirBuf->r_pos) return WLR_ERROR;       // 缓冲区满

    CirBuf->asbuf[CirBuf->w_pos].dataL = dataL;
    CirBuf->asbuf[CirBuf->w_pos].dataR = dataR;
    CirBuf->w_pos = next_w; // 更新写位置
    return WLR_OK;
}

uint32_t CirBuf_AsDataRead(CirBuf_t *CirBuf, uint16_t *dataL, uint16_t *dataR)
{
    if (CirBuf->r_pos == CirBuf->w_pos) return WLR_ERROR; // 缓冲区空

    *dataL = CirBuf->asbuf[CirBuf->r_pos].dataL;
    *dataR = CirBuf->asbuf[CirBuf->r_pos].dataR;
    CirBuf->r_pos = (CirBuf->r_pos + 1) % CirBuf->len; // 修复：更新读位置
    return WLR_OK;
}

void CirBuf_MpuDataInit(CirBuf_t *CirBuf, uint32_t len, MpuRawData_t *buf)
{
    CirBuf->r_pos = CirBuf->w_pos = 0;
    CirBuf->len = len;
    CirBuf->mpubuf = buf;
}

uint32_t CirBuf_MpuDataWrite(CirBuf_t *CirBuf, MpuRawData_t *s16data)
{
    uint32_t next_w = (CirBuf->w_pos + 1) % CirBuf->len; // 优化回绕计算
    if (next_w == CirBuf->r_pos) return WLR_ERROR;       // 缓冲区满

    for (uint8_t idx = 0; idx < MPU6050_INT16_DATASIZE; idx++) {
        CirBuf->mpubuf[CirBuf->w_pos].data[idx] = s16data->data[idx];
    }
    CirBuf->w_pos = next_w; // 更新写位置
    return WLR_OK;
}

uint32_t CirBuf_MpuDataRead(CirBuf_t *CirBuf, MpuRawData_t *s16data)
{
    if (CirBuf->r_pos == CirBuf->w_pos) return WLR_ERROR; // 缓冲区空

    for (uint8_t idx = 0; idx < MPU6050_INT16_DATASIZE; idx++) {
        s16data->data[idx] = CirBuf->mpubuf[CirBuf->w_pos].data[idx];
    }
    CirBuf->r_pos = (CirBuf->r_pos + 1) % CirBuf->len; // 修复：更新读位置
    return WLR_OK;
}