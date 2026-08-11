// Rebuilt from df_auction_r DWARF (HandlerFor_GP_JPN.cpp CU — class HandlerFor_GP_), 2026-08-08
#ifndef AUCTION_HANDLERFOR_GP_JPN_H_
#define AUCTION_HANDLERFOR_GP_JPN_H_

#include "IHandler.h"
#include "AuctionPacket.h"
#include "Auction.h"

class HandlerFor_GP_ : public nsl::INetWorkHandler
{
public:
    typedef unsigned int (HandlerFor_GP_::*networkFuncType)(nsl::CMsgCell*, nsl::TCPUser*);

    HandlerFor_GP_()
    {
    }
    virtual ~HandlerFor_GP_()
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
            (nsl::INetWorkHandler::networkFuncType)&HandlerFor_GP_::onAUCTION_REGIST_GP;
        mArrayFunc[1] =
            (nsl::INetWorkHandler::networkFuncType)&HandlerFor_GP_::onAUCTION_ASK_AVERAGE_PRICE_GP;
        mArrayFunc[2] =
            (nsl::INetWorkHandler::networkFuncType)&HandlerFor_GP_::onAUCTION_ASK_REGISTED_ITEM_NUM_GP;
        mArrayFunc[3] =
            (nsl::INetWorkHandler::networkFuncType)&HandlerFor_GP_::onAUCTION_REGIST_ITEM_GP;
        mArrayFunc[4] =
            (nsl::INetWorkHandler::networkFuncType)&HandlerFor_GP_::onAUCTION_REGIST_CANCEL_GP;
        mArrayFunc[5] =
            (nsl::INetWorkHandler::networkFuncType)&HandlerFor_GP_::onAUCTION_BIDDING_GP;
        mArrayFunc[6] =
            (nsl::INetWorkHandler::networkFuncType)&HandlerFor_GP_::onAUCTION_SEARCH_BY_ITEMKEY_GP;
        mArrayFunc[7] =
            (nsl::INetWorkHandler::networkFuncType)&HandlerFor_GP_::onAUCTION_SEARCH_BY_NOITEMKEY_GP;
        mArrayFunc[8] =
            (nsl::INetWorkHandler::networkFuncType)&HandlerFor_GP_::onAUCTION_MY_REGISTED_ITEM_INFO_GP;
        mArrayFunc[9] =
            (nsl::INetWorkHandler::networkFuncType)&HandlerFor_GP_::onAUCTION_MY_BIDDING_INFO_GP;
        mArrayFunc[10] =
            (nsl::INetWorkHandler::networkFuncType)&HandlerFor_GP_::onAUCTION_MY_AUCTION_HISTORY_GP;
        mArrayFunc[11] =
            (nsl::INetWorkHandler::networkFuncType)&HandlerFor_GP_::onAUCTION_OPEN_PRIVATE_STORE_GP;
        mArrayFunc[12] =
            (nsl::INetWorkHandler::networkFuncType)&HandlerFor_GP_::onAUCTION_CLOSE_PRIVATE_STORE_GP;
        mArrayFunc[13] =
            (nsl::INetWorkHandler::networkFuncType)&HandlerFor_GP_::onAUCTION_CHECK_AUCION_READY_GP;
        mArrayFunc[14] =
            (nsl::INetWorkHandler::networkFuncType)&HandlerFor_GP_::onAUCTION_ASK_OWNER_IS_VIP_GP;
    }

    bool IsPointServer()
    {
        return G_Auction()->GetPayType() == PAY_TYPE_POINT;
    }

    unsigned int onAUCTION_REGIST_GP(nsl::CMsgCell* pCell, nsl::TCPUser* u);
    unsigned int onAUCTION_ASK_AVERAGE_PRICE_GP(nsl::CMsgCell* pCell, nsl::TCPUser* u);
    unsigned int onAUCTION_ASK_REGISTED_ITEM_NUM_GP(nsl::CMsgCell* pCell, nsl::TCPUser* u);
    unsigned int onAUCTION_REGIST_ITEM_GP(nsl::CMsgCell* pCell, nsl::TCPUser* u);
    unsigned int onAUCTION_REGIST_CANCEL_GP(nsl::CMsgCell* pCell, nsl::TCPUser* u);
    unsigned int onAUCTION_BIDDING_GP(nsl::CMsgCell* pCell, nsl::TCPUser* u);
    unsigned int onAUCTION_SEARCH_BY_ITEMKEY_GP(nsl::CMsgCell* pCell, nsl::TCPUser* u);
    unsigned int onAUCTION_SEARCH_BY_NOITEMKEY_GP(nsl::CMsgCell* pCell, nsl::TCPUser* u);
    unsigned int onAUCTION_MY_REGISTED_ITEM_INFO_GP(nsl::CMsgCell* pCell, nsl::TCPUser* u);
    unsigned int onAUCTION_MY_BIDDING_INFO_GP(nsl::CMsgCell* pCell, nsl::TCPUser* u);
    unsigned int onAUCTION_MY_AUCTION_HISTORY_GP(nsl::CMsgCell* pCell, nsl::TCPUser* u);
    unsigned int onAUCTION_OPEN_PRIVATE_STORE_GP(nsl::CMsgCell* pCell, nsl::TCPUser* u);
    unsigned int onAUCTION_CLOSE_PRIVATE_STORE_GP(nsl::CMsgCell* pCell, nsl::TCPUser* u);
    unsigned int onAUCTION_CHECK_AUCION_READY_GP(nsl::CMsgCell* pCell, nsl::TCPUser* u);
    unsigned int onAUCTION_ASK_OWNER_IS_VIP_GP(nsl::CMsgCell* pCell, nsl::TCPUser* u);

private:
    nsl::INetWorkHandler::networkFuncType mArrayFunc[1024];   // @12
    char mpSzBuffer[4096];              // @8204
};

#endif // AUCTION_HANDLERFOR_GP_JPN_H_
