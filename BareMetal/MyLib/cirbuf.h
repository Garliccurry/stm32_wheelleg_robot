#ifndef _CIRBUF_H
#define _CIRBUF_H

#include <stdint.h>

#define CB_TypeDef CirBuf_HandleTypeDef

typedef struct CirBuf {
    uint32_t r;
    uint32_t w;
    uint32_t len;
    uint8_t *buf;
} CirBuf_HandleTypeDef;

void CirBuf_Init(CB_TypeDef *CB_handle, uint32_t len, uint8_t *buf);
int  CirBuf_Read(CB_TypeDef *CB_handle, uint8_t *pVal);
int  CirBuf_Write(CB_TypeDef *CB_handle, uint8_t val);

#endif /* _CIRBUF_H */
