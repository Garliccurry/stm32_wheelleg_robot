#include <stdint.h>
#include "cirbuf.h"
void CirBuf_Init(CB_TypeDef *CB_handle, uint32_t len, uint8_t *buf)
{
    CB_handle->r = CB_handle->w = 0;
    CB_handle->len = len;
    CB_handle->buf = buf;
}

int CirBuf_Write(CB_TypeDef *CB_handle, uint8_t val)
{
    uint32_t next_w;

    next_w = ((CB_handle->w + 1) != CB_handle->len) ? (CB_handle->w + 1) : 0;

    if (next_w != CB_handle->r) {
        CB_handle->buf[CB_handle->w] = val;
        CB_handle->w = next_w;
        return 0;
    } else {
        return -1;
    }
}

int CirBuf_Read(CB_TypeDef *CB_handle, uint8_t *pVal)
{
    if (CB_handle->r != CB_handle->w) {
        *pVal = CB_handle->buf[CB_handle->r];

        CB_handle->r++;

        if (CB_handle->r == CB_handle->len)
            CB_handle->r = 0;
        return 0;
    } else {
        return -1;
    }
}
