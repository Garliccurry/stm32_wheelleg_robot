#ifndef _CIRBUF_H__
#define _CIRBUF_H__

#include <stdint.h>
#include "motion.h"
#include "info.h"

typedef struct {
    uint32_t      r_pos;
    uint32_t      w_pos;
    uint32_t      len;
    AsRawData_t  *asbuf;
    MpuRawData_t *mpubuf;
} CirBuf_t;

void     CirBuf_AsDataInit(CirBuf_t *CirBuf, uint32_t len, AsRawData_t *buf);
uint32_t CirBuf_AsDataRead(CirBuf_t *CirBuf, uint16_t *dataL, uint16_t *dataR);
uint32_t CirBuf_AsDataWrite(CirBuf_t *CirBuf, uint16_t dataL, uint16_t dataR);

void     CirBuf_MpuDataInit(CirBuf_t *CirBuf, uint32_t len, MpuRawData_t *buf);
uint32_t CirBuf_MpuDataRead(CirBuf_t *CirBuf, MpuRawData_t *s16data);
uint32_t CirBuf_MpuDataWrite(CirBuf_t *CirBuf, MpuRawData_t *s16data);

#endif /* _CIRBUF_H */
