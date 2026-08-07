#ifndef AUCTION_HANDLERFOR_TE__H_
#define AUCTION_HANDLERFOR_TE__H_

#include <time.h>

#include "IHandler.h"
#include "TE_Entity.h"

class Auction;

class HandlerFor_TE_ : public nsl::ITimeHandler
{
public:
    HandlerFor_TE_();
    virtual ~HandlerFor_TE_();
    virtual void init();
    void initTimeEvent();
    unsigned int onTIME_AUCTION_EXPIRE_EVENT_CHECK(nsl::InternalMsg* pArg);
    unsigned int onTIME_AUCTION_STATISTICS_COLLECTOR(nsl::InternalMsg* pArg);
    unsigned int onTIME_CHECK_CONFIG(nsl::InternalMsg* pArg);
    unsigned int onTIME_AUCTION_DB_PING(nsl::InternalMsg* pArg);
    unsigned int onTIME_AUCTION_TRY_SHUTDOWN(nsl::InternalMsg* pArg);
    unsigned int onTIME_AUCTION_UPDATE_AVERAGE_PRICE(nsl::InternalMsg* pArg);

    struct tm mOldTM;
    nsl::TE_Entity<HandlerFor_TE_>* mTimeEntity[125];
};

#endif // AUCTION_HANDLERFOR_TE__H_
