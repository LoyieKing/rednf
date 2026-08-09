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
        mArrayFunc[2] = &InterHandler::onINTER_DESTORY_CHARACTER;
        mArrayFunc[0] = &InterHandler::onINTER_SERVICE_UNAVAILABLE;
    }

    void initInterEvent();
    unsigned int onINTER_DESTORY_CHARACTER(nsl::CMsgCell* pCell);
    unsigned int onINTER_SERVICE_UNAVAILABLE(nsl::CMsgCell* pCell);

    // PMF 8B x 1024 @ offset 12 => sizeof(InterHandler) = 0x200c (ORIG operator new size)
    // init() already clears only 0x400 entries; HandlerFor_GA_/GP_ use the same 1024 length.
    InterFunc mArrayFunc[1024];
};

#endif // AUCTION_INTERHANDLER_H_
