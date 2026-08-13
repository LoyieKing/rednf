#ifndef NSL_GETMESSAGEBUFFER_H_
#define NSL_GETMESSAGEBUFFER_H_

#include "TMsgCell.h"

namespace nsl {

class CMsgCell;

inline CMsgCell* GetMessageBuffer(int size)
{
    if (size > 0x80000 || size < 0)
    {
        throw (const char*)__FUNCTION__;
    }
    CMsgCell* pBuf = NULL;
    if (size <= 0x10)
    {
        pBuf = new TMsgCell<16>();
    }
    else if (size <= 0x20)
    {
        pBuf = new TMsgCell<32>();
    }
    else if (size <= 0x40)
    {
        pBuf = new TMsgCell<64>();
    }
    else if (size <= 0x80)
    {
        pBuf = new TMsgCell<128>();
    }
    else if (size <= 0x100)
    {
        pBuf = new TMsgCell<256>();
    }
    else if (size <= 0x200)
    {
        pBuf = new TMsgCell<512>();
    }
    else if (size <= 0x400)
    {
        pBuf = new TMsgCell<1024>();
    }
    else if (size <= 0x800)
    {
        pBuf = new TMsgCell<2048>();
    }
    else if (size <= 0x1000)
    {
        pBuf = new TMsgCell<4096>();
    }
    else if (size <= 0x2000)
    {
        pBuf = new TMsgCell<8192>();
    }
    else if (size <= 0x4000)
    {
        pBuf = new TMsgCell<16384>();
    }
    else if (size <= 0x8000)
    {
        pBuf = new TMsgCell<32768>();
    }
    else if (size <= 0x10000)
    {
        pBuf = new TMsgCell<65536>();
    }
    else if (size <= 0x20000)
    {
        pBuf = new TMsgCell<131072>();
    }
    else if (size <= 0x40000)
    {
        pBuf = new TMsgCell<262144>();
    }
    else if (size <= 0x80000)
    {
        pBuf = new TMsgCell<524288>();
    }
    else
    {
        pBuf = new TMsgCell<409600>();
    }
    return pBuf;
}

} // namespace nsl

#endif // NSL_GETMESSAGEBUFFER_H_
