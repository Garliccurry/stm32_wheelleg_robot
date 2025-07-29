#ifndef _CIRBUF_H
#define _CIRBUF_H

#include <stdint.h>

#define CB_TypeDef CirBuf_HandleTypeDef
#define pCB_TypeDef pCirBuf_HandleTypeDef

typedef struct CirBuf {
	uint32_t r;
	uint32_t w;
	uint32_t len;
	uint8_t *buf;
}CirBuf_HandleTypeDef, *pCirBuf_HandleTypeDef;

void CirBuf_Init(pCB_TypeDef pCB, uint32_t len, uint8_t *buf);
int CirBuf_Read(pCB_TypeDef pCB, uint8_t *pVal);
int CirBuf_Write(pCB_TypeDef pCB, uint8_t val);

#endif /* _CIRBUF_H */
