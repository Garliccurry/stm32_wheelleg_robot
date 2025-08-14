#ifndef _CIRBUF_H__
#define _CIRBUF_H__

#include <stdint.h>
#include "motion.h"
#include "info.h"

typedef struct {
    uint32_t        r_pos;
    uint32_t        w_pos;
    uint32_t        len;
    AsRawDataBuf_t *buf;
} CirBuf_t;

void     CirBuf_AsDataInit(CirBuf_t *CirBuf, uint32_t len, AsRawDataBuf_t *buf);
uint32_t CirBuf_AsDataRead(CirBuf_t *CirBuf, uint16_t *dataL, uint16_t *dataR);
uint32_t CirBuf_AsDataWrite(CirBuf_t *CirBuf, uint16_t dataL, uint16_t dataR);

#endif /* _CIRBUF_H */
