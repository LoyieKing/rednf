#ifndef AUCTION_HANDLERFOR_TE__H_
#define AUCTION_HANDLERFOR_TE__H_

#include <time.h>
#include <string.h>

#include "IHandler.h"
#include "TE_Entity.h"

class Auction;

class HandlerFor_TE_ : public nsl::ITimeHandler
{
public:
    // Empty ctor/dtor/init as weak inline (ORIG: W symbols; avoids prepareRun EH pads)
    HandlerFor_TE_()
    {
    }
    virtual ~HandlerFor_TE_()
    {
    }
    virtual void init()
    {
        nsl::IHandler::init();
        initTimeEvent();
        memset(&mOldTM, 0, sizeof(struct tm));
    }
    void initTimeEvent();
    unsigned long onTIME_AUCTION_EXPIRE_EVENT_CHECK(nsl::InternalMsg* pArg);
    unsigned long onTIME_AUCTION_STATISTICS_COLLECTOR(nsl::InternalMsg* pArg);
    unsigned long onTIME_CHECK_CONFIG(nsl::InternalMsg* pArg);
    unsigned long onTIME_AUCTION_DB_PING(nsl::InternalMsg* pArg);
    unsigned long onTIME_AUCTION_TRY_SHUTDOWN(nsl::InternalMsg* pArg);
    unsigned long onTIME_AUCTION_UPDATE_AVERAGE_PRICE(nsl::InternalMsg* pArg);

    struct tm mOldTM;
    nsl::TE_Entity<HandlerFor_TE_>* mTimeEntity[125];
};

#endif // AUCTION_HANDLERFOR_TE__H_
