#ifndef AUCTION_INTERHANDLER_H_
#define AUCTION_INTERHANDLER_H_

#include "IHandler.h"

class InterHandler : public nsl::IInterHandler
{
public:
    typedef unsigned int (InterHandler::*InterFunc)(nsl::CMsgCell*);

    InterHandler()
    {
    }
    virtual ~InterHandler()
    {
    }
    virtual void init()
    {
        for (int i = 0; i < 0x400; i = i + 1)
        {
            mArrayFunc[i] = NULL;
        }
        registFuncMap();
        nsl::IHandler::init();
        initInterEvent();
    }
    virtual nsl::IInterHandler::interFuncType searchInterFunc(int nProtoID)
    {
        return (nsl::IInterHandler::interFuncType)mArrayFunc[nProtoID];
    }

    void registFuncMap()
    {
        // ORIG：mArrayFunc 为基类 PMF 类型，派生→基 PMF 需显式转换
        // （GCC 4.4 拒绝隐式转换），转换产生 edx 临时装载形态。
        mArrayFunc[2] =
            (nsl::IInterHandler::interFuncType)&InterHandler::onINTER_DESTORY_CHARACTER;
        mArrayFunc[0] =
            (nsl::IInterHandler::interFuncType)&InterHandler::onINTER_SERVICE_UNAVAILABLE;
    }

    void initInterEvent();
    unsigned int onINTER_DESTORY_CHARACTER(nsl::CMsgCell* pCell);
    unsigned int onINTER_SERVICE_UNAVAILABLE(nsl::CMsgCell* pCell);

    // PMF 8B x 1024 @ offset 12 => sizeof(InterHandler) = 0x200c (ORIG operator new size)
    // init() already clears only 0x400 entries; HandlerFor_GA_/GP_ use the same 1024 length.
    nsl::IInterHandler::interFuncType mArrayFunc[1024];
};

#endif // AUCTION_INTERHANDLER_H_
