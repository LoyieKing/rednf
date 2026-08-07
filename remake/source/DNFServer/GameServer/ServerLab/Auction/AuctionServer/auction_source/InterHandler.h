#ifndef AUCTION_INTERHANDLER_H_
#define AUCTION_INTERHANDLER_H_

#include "IHandler.h"

class InterHandler : public nsl::IInterHandler
{
public:
    typedef unsigned int (InterHandler::*InterFunc)(nsl::CMsgCell*);

    InterHandler();
    virtual ~InterHandler();
    virtual void init(nsl::TCPSendThread* pSendThread);
    virtual nsl::IInterHandler::interFuncType searchInterFunc(int nProtoID);

    InterFunc mArrayFunc[2048];
};

#endif // AUCTION_INTERHANDLER_H_
