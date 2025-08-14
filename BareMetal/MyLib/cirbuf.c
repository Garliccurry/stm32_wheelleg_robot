#include <stdint.h>
#include "cirbuf.h"
void CirBuf_AsDataInit(CirBuf_t *CirBuf, uint32_t len, AsRawDataBuf_t *buf)
{
    CirBuf->r_pos = CirBuf->w_pos = 0;
    CirBuf->len = len;
    CirBuf->buf = buf;
}

uint32_t CirBuf_AsDataWrite(CirBuf_t *CirBuf, uint16_t dataL, uint16_t dataR)
{
    uint32_t next_w = (CirBuf->w_pos + 1) % CirBuf->len; // 优化回绕计算
    if (next_w == CirBuf->r_pos) return WL_ERROR;        // 缓冲区满

    CirBuf->buf[CirBuf->w_pos].dataL = dataL;
    CirBuf->buf[CirBuf->w_pos].dataR = dataR;
    CirBuf->w_pos = next_w; // 更新写位置
    return WL_OK;
}

uint32_t CirBuf_AsDataRead(CirBuf_t *CirBuf, uint16_t *dataL, uint16_t *dataR)
{
    if (CirBuf->r_pos == CirBuf->w_pos) return WL_ERROR; // 缓冲区空

    *dataL = CirBuf->buf[CirBuf->r_pos].dataL;
    *dataR = CirBuf->buf[CirBuf->r_pos].dataR;
    CirBuf->r_pos = (CirBuf->r_pos + 1) % CirBuf->len; // 修复：更新读位置
    return WL_OK;
}

void CirBuf_MpuDataInit(CirBuf_t *CirBuf, uint32_t len, AsRawDataBuf_t *buf)
{
    CirBuf->r_pos = CirBuf->w_pos = 0;
    CirBuf->len = len;
    CirBuf->buf = buf;
}

uint32_t CirBuf_MpuDataWrite(CirBuf_t *CirBuf, uint16_t dataL, uint16_t dataR)
{
    uint32_t next_w = (CirBuf->w_pos + 1) % CirBuf->len; // 优化回绕计算
    if (next_w == CirBuf->r_pos) return WL_ERROR;        // 缓冲区满

    CirBuf->buf[CirBuf->w_pos].dataL = dataL;
    CirBuf->buf[CirBuf->w_pos].dataR = dataR;
    CirBuf->w_pos = next_w; // 更新写位置
    return WL_OK;
}

uint32_t CirBuf_MpuDataRead(CirBuf_t *CirBuf, uint16_t *dataL, uint16_t *dataR)
{
    if (CirBuf->r_pos == CirBuf->w_pos) return WL_ERROR; // 缓冲区空

    *dataL = CirBuf->buf[CirBuf->r_pos].dataL;
    *dataR = CirBuf->buf[CirBuf->r_pos].dataR;
    CirBuf->r_pos = (CirBuf->r_pos + 1) % CirBuf->len; // 修复：更新读位置
    return WL_OK;
}