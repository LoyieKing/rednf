// Rebuilt from df_auction_r DWARF (HandlerFor_GA_.cpp CU), 2026-08-08
#ifndef AUCTION_HANDLERFOR_GA__H_
#define AUCTION_HANDLERFOR_GA__H_

#include "IHandler.h"
#include "AuctionPacket.h"
#include "Auction.h"

class HandlerFor_GA_ : public nsl::INetWorkHandler
{
public:
    typedef unsigned int (HandlerFor_GA_::*networkFuncType)(nsl::CMsgCell*, nsl::TCPUser*);

    HandlerFor_GA_()
    {
    }
    virtual ~HandlerFor_GA_()
    {
    }
    virtual void init()
    {
        nsl::IHandler::init();
        registFuncMap();
        G_Auction();
    }
    virtual nsl::INetWorkHandler::networkFuncType searchNetworkFunc(int nProtoID)
    {
        return (nsl::INetWorkHandler::networkFuncType)mArrayFunc[nProtoID];
    }

    void registFuncMap()
    {
        for (int i = 0; i <= 0x3ff; i = i + 1)
        {
            mArrayFunc[i] = NULL;
        }
        mArrayFunc[0] =
            (nsl::INetWorkHandler::networkFuncType)&HandlerFor_GA_::onAUCTION_REGIST_GA;
        mArrayFunc[1] =
            (nsl::INetWorkHandler::networkFuncType)&HandlerFor_GA_::onAUCTION_ASK_AVERAGE_PRICE_GA;
        mArrayFunc[2] =
            (nsl::INetWorkHandler::networkFuncType)&HandlerFor_GA_::onAUCTION_ASK_REGISTED_ITEM_NUM_GA;
        mArrayFunc[3] =
            (nsl::INetWorkHandler::networkFuncType)&HandlerFor_GA_::onAUCTION_REGIST_ITEM_GA;
        mArrayFunc[4] =
            (nsl::INetWorkHandler::networkFuncType)&HandlerFor_GA_::onAUCTION_REGIST_CANCEL_GA;
        mArrayFunc[5] =
            (nsl::INetWorkHandler::networkFuncType)&HandlerFor_GA_::onAUCTION_BIDDING_GA;
        mArrayFunc[6] =
            (nsl::INetWorkHandler::networkFuncType)&HandlerFor_GA_::onAUCTION_SEARCH_BY_ITEMKEY_GA;
        mArrayFunc[7] =
            (nsl::INetWorkHandler::networkFuncType)&HandlerFor_GA_::onAUCTION_SEARCH_BY_NOITEMKEY_GA;
        mArrayFunc[8] =
            (nsl::INetWorkHandler::networkFuncType)&HandlerFor_GA_::onAUCTION_MY_REGISTED_ITEM_INFO_GA;
        mArrayFunc[9] =
            (nsl::INetWorkHandler::networkFuncType)&HandlerFor_GA_::onAUCTION_MY_BIDDING_INFO_GA;
        mArrayFunc[10] =
            (nsl::INetWorkHandler::networkFuncType)&HandlerFor_GA_::onAUCTION_MY_AUCTION_HISTORY_GA;
        mArrayFunc[11] =
            (nsl::INetWorkHandler::networkFuncType)&HandlerFor_GA_::onAUCTION_OPEN_PRIVATE_STORE_GA;
        mArrayFunc[12] =
            (nsl::INetWorkHandler::networkFuncType)&HandlerFor_GA_::onAUCTION_CLOSE_PRIVATE_STORE_GA;
        mArrayFunc[13] =
            (nsl::INetWorkHandler::networkFuncType)&HandlerFor_GA_::onAUCTION_CHECK_AUCION_READY_GA;
        mArrayFunc[14] =
            (nsl::INetWorkHandler::networkFuncType)&HandlerFor_GA_::onAUCTION_BUY_ITEM_APIECE_GA;
    }

    bool IsGoldServer()
    {
        return G_Auction()->GetPayType() == PAY_TYPE_GOLD;
    }

    unsigned int onAUCTION_REGIST_GA(nsl::CMsgCell* pCell, nsl::TCPUser* u);
    unsigned int onAUCTION_ASK_AVERAGE_PRICE_GA(nsl::CMsgCell* pCell, nsl::TCPUser* u);
    unsigned int onAUCTION_ASK_REGISTED_ITEM_NUM_GA(nsl::CMsgCell* pCell, nsl::TCPUser* u);
    unsigned int onAUCTION_REGIST_ITEM_GA(nsl::CMsgCell* pCell, nsl::TCPUser* u);
    unsigned int onAUCTION_REGIST_CANCEL_GA(nsl::CMsgCell* pCell, nsl::TCPUser* u);
    unsigned int onAUCTION_BIDDING_GA(nsl::CMsgCell* pCell, nsl::TCPUser* u);
    unsigned int onAUCTION_SEARCH_BY_ITEMKEY_GA(nsl::CMsgCell* pCell, nsl::TCPUser* u);
    unsigned int onAUCTION_SEARCH_BY_NOITEMKEY_GA(nsl::CMsgCell* pCell, nsl::TCPUser* u);
    unsigned int onAUCTION_MY_REGISTED_ITEM_INFO_GA(nsl::CMsgCell* pCell, nsl::TCPUser* u);
    unsigned int onAUCTION_MY_BIDDING_INFO_GA(nsl::CMsgCell* pCell, nsl::TCPUser* u);
    unsigned int onAUCTION_MY_AUCTION_HISTORY_GA(nsl::CMsgCell* pCell, nsl::TCPUser* u);
    unsigned int onAUCTION_OPEN_PRIVATE_STORE_GA(nsl::CMsgCell* pCell, nsl::TCPUser* u);
    unsigned int onAUCTION_CLOSE_PRIVATE_STORE_GA(nsl::CMsgCell* pCell, nsl::TCPUser* u);
    unsigned int onAUCTION_CHECK_AUCION_READY_GA(nsl::CMsgCell* pCell, nsl::TCPUser* u);
    unsigned int onAUCTION_BUY_ITEM_APIECE_GA(nsl::CMsgCell* pCell, nsl::TCPUser* u);

private:
    nsl::INetWorkHandler::networkFuncType mArrayFunc[1024];   // @12
    char mpSzBuffer[4096];              // @8204
};

#endif // AUCTION_HANDLERFOR_GA__H_
