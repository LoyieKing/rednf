// Rebuilt from df_auction_r (DWARF + Ghidra decompile), 2026-08-08
#include "InterHandler.h"

#include "TraceLog.h"
#include "ServiceFactory.h"
#include "Threads.h"
#include "DataPools.h"
#include "Zone.h"
#include "Character.h"
#include "GameDataPool.h"
#include "Message.h"
#include "InternalMsgDesign.h"
#include "DBTransactionDesign.h"
#include "PacketDesign.h"
#include "Auction.h"
#include "NSLDBThread.h"
#include "WorkThread.h"
#include "TimerThread.h"
#include "TCPSendThread.h"
#include "TCPUser.h"
#include "CommonDataPool.h"
#include "ServiceError.h"
#include "TE_Entity.h"
#include "HandlerFor_TE_.h"

using namespace nsl;

void InterHandler::initInterEvent()
{
    G_TraceLog()->sysLog(5, "In  initInterEvent");

    G_TraceLog()->sysLog(6, "=================\xC6\xF2\xB1\xD5\xB0\xAA DB\xB8\xA6 \xC0\xD0\xBE\xEE\xBF\xC9\xB4\xCF\xB4\xD9=================");
    tagAUCTION_DB_GET_AVERAGE_PRICE dbtr_get_average_price;
    nsl::Message* pMsg = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->createMessage(3);
    nsl::CMsgCell* pNewCell = pMsg->getCellFromMessage();
    *pNewCell << &dbtr_get_average_price;
    pApp->super_Threads.getDBThread(0)->PushTransaction(pMsg);

    G_TraceLog()->sysLog(6, "=============\xB7\xA3\xB4\xFD \xBF\xC9\xBC\xC7 \xC6\xF2\xB1\xD5\xB0\xAA DB\xB8\xA6 \xC0\xD0\xBE\xEE\xBF\xC9\xB4\xCF\xB4\xD9=============");
    tagAUCTION_DB_GET_ROI_AVERAGE_PRICE_INFO dbtr_get_roi_average_price;
    pMsg = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->createMessage(3);
    pNewCell = pMsg->getCellFromMessage();
    *pNewCell << &dbtr_get_roi_average_price;
    pApp->super_Threads.getDBThread(0)->PushTransaction(pMsg);

    G_TraceLog()->sysLog(6, "=================Auction DB\xB8\xA6 \xC0\xD0\xBE\xEE\xBF\xC9\xB4\xCF\xB4\xD9=================");
    tagAUCTION_DB_GET_REGISTED_ITEM dbtr_get_auction_main;
    pMsg = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->createMessage(3);
    pNewCell = pMsg->getCellFromMessage();
    *pNewCell << &dbtr_get_auction_main;
    pApp->super_Threads.getDBThread(0)->PushTransaction(pMsg);

    G_TraceLog()->sysLog(5, "Out initInterEvent");
}

unsigned int InterHandler::onINTER_SERVICE_UNAVAILABLE(nsl::CMsgCell* pCell)
{
    G_TraceLog()->sysLog(5, "In  onINTER_SERVICE_UNAVAILABLE");
    INTERNALMSG_SERVICE_UNAVAILABLE* pInterMsg =
        (INTERNALMSG_SERVICE_UNAVAILABLE*)pCell->GetInternalMsg();
    G_TraceLog()->sysLog(5, "Reason %s", nsl::GetErrorStr((int)pInterMsg->reason));

    GSArea* pArea = (GSArea*)G_Zone()->mArea[0];
    pArea->SetServiceRunning(false);

    PCK_AUCTION_SERVICE_UNAVAILABLE_AG pck;
    PCK_AUCTION_SERVICE_UNAVAILABLE_PG pck_pg;

    for (nsl::MAP_OBJECTS_ITER iter = pArea->getBeginIter(); !pArea->isIterEnd(iter); ++iter)
    {
        nsl::ISession* pSession = pArea->getValueFromIter(iter);
        nsl::TCPUser* pTcpUser = pSession->getTCPUser();
        if (!pTcpUser->isAboutToDisconnect() && !pTcpUser->isDisconnected())
        {
            nsl::Message* pNewMsg =
                pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->getSendMessage(pSession->getTCPUser());
            nsl::CMsgCell* pNewCell = pNewMsg->getCellFromMessage();
            if (G_Auction()->GetPayType() == PAY_TYPE_POINT)
            {
                *pNewCell << &pck_pg;
            }
            else
            {
                *pNewCell << &pck;
            }
            pNewCell->PAD();
            sendTCP_->PushSendMsg(pNewMsg);
        }
    }

    nsl::TE_Entity<HandlerFor_TE_>* pTIME_AUCTION_TRY_SHUTDOWN =
        ((GameDataPool*)pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId))->createTimeEntity();
    nsl::InternalMsg* pArg = pTIME_AUCTION_TRY_SHUTDOWN->getArg();
    pArg->workIndex = 0;
    pTIME_AUCTION_TRY_SHUTDOWN->regist(0x20, 5000, 0xffffffff, pTimeHandler,
                                       &HandlerFor_TE_::onTIME_AUCTION_TRY_SHUTDOWN);
    if (pTIME_AUCTION_TRY_SHUTDOWN->check_period == 0)
    {
        nsl::InternalMsg* pArg = pTIME_AUCTION_TRY_SHUTDOWN->getArg();
        pApp->super_Threads.getWorkThread(pArg->workIndex)
            ->PushTransaction((nsl::IMessageStruct*)pTIME_AUCTION_TRY_SHUTDOWN);
    }
    else
    {
        pApp->super_Threads.getTimerThread()->PushTimeReqEvent(pTIME_AUCTION_TRY_SHUTDOWN);
    }

    G_TraceLog()->sysLog(5, "Out  onINTER_SERVICE_UNAVAILABLE");
    return 0;
}

unsigned int InterHandler::onINTER_DESTORY_CHARACTER(nsl::CMsgCell* pCell)
{
    G_TraceLog()->sysLog(5, "In  onINTER_DESTORY_CHARACTER");
    INTERNALMSG_DESTROY_CHARACTER* pInterMsg =
        (INTERNALMSG_DESTROY_CHARACTER*)pCell->GetInternalMsg();
    int workIndex = pInterMsg->workIndex;
    bool bActiveClosing = pInterMsg->bActiveClosing;
    unsigned int characKey = pInterMsg->characKey;
    int areaIndex = pInterMsg->areaIndex;
    auction::Character* pCharacter;
    if (areaIndex == 0)
    {
        pCharacter = (auction::Character*)G_Zone()->mArea[0]->getMemberAt(characKey);
        G_Zone()->mArea[0]->unregist(characKey);
    }
    else
    {
        G_TraceLog()->sysLog(7, "Fail: onINTER_DESTORY_CHARACTER : Error");
    }

    if (pCharacter->isActiveTCPUser())
    {
        ((GameDataPool*)pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId))
            ->destroyConInfo(pInterMsg->pConInfo);
    }

    nsl::TCPUser* pUser = pCharacter->getTCPUser();
    ((GameDataPool*)pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId))
        ->destroyCharacter(pCharacter);

    if (bActiveClosing)
    {
        puts("Active Close Sync Ok");
        pUser->setActiveSyncByWorker(true);
    }
    else
    {
        puts("Passive Close Sync Ok");
        pUser->setPassiveSyncByWorker(true);
    }

    G_TraceLog()->sysLog(5, "Out onINTER_DESTORY_CHARACTER");
    return 0;
}
