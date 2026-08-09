// HandlerFor_TE_.cpp
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "HandlerFor_TE_.h"
#include "ServiceFactory.h"
#include "Threads.h"
#include "WorkThread.h"
#include "NSLDBThread.h"
#include "TimerThread.h"
#include "GameDataPool.h"
#include "Zone.h"
#include "StatisticsCollector.h"
#include "LinuxService.h"
#include "TCPUser.h"
#include "TCPSendThread.h"
#include "Message.h"
#include "TraceLog.h"
#include "IArea.h"
#include "ISession.h"
#include "DBTransactionDesign.h"
#include "PacketDesign.h"
#include "Auction.h"

using namespace nsl;

extern Auction* G_Auction();

extern "C" int __xstat(int ver, const char* path, struct stat* buf);

extern "C" __attribute__((weak)) int stat(const char* __path, struct stat* __statbuf)
{
    return __xstat(3, __path, __statbuf);
}

namespace nsl {
extern char configpath[256];
extern bool is_config_changed;
extern bool is_config_reloaded;
}

// ctor / dtor / init: weak inline in HandlerFor_TE_.h (ORIG form)

void HandlerFor_TE_::initTimeEvent()
{
    G_TraceLog()->sysLog(8, "In  initTimeEvent");

    // Separate locals per timer so frame size matches ORIG (~0x80)
    nsl::TE_Entity<HandlerFor_TE_>* pTimeEntity0;
    nsl::InternalMsg* pArg0;
    nsl::TE_Entity<HandlerFor_TE_>* pTimeEntity1;
    nsl::InternalMsg* pArg1;
    nsl::TE_Entity<HandlerFor_TE_>* pTimeEntity2;
    nsl::InternalMsg* pArg2;
    nsl::TE_Entity<HandlerFor_TE_>* pTimeEntity3;
    nsl::InternalMsg* pArg3;
    nsl::TE_Entity<HandlerFor_TE_>* pTimeEntity4;
    nsl::InternalMsg* pArg4;
    // Extra slots to approach ORIG's 0x80 frame (member-ptr / getArg spills)
    nsl::InternalMsg* pArg0b;
    nsl::InternalMsg* pArg1b;
    nsl::InternalMsg* pArg2b;
    nsl::InternalMsg* pArg3b;
    nsl::InternalMsg* pArg4b;

    pTimeEntity0 = ((GameDataPool*)pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId))
        ->createTimeEntity();
    pArg0 = pTimeEntity0->getArg();
    pArg0->workIndex = 0;
    pTimeEntity0->regist(0x19, 2000, 0xffffffff, pTimeHandler, &HandlerFor_TE_::onTIME_AUCTION_EXPIRE_EVENT_CHECK);
    if (pTimeEntity0->check_period == 0)
    {
        pArg0b = pTimeEntity0->getArg();
        pApp->super_Threads.getWorkThread(pArg0b->workIndex)->PushTransaction((IMessageStruct*)pTimeEntity0);
    }
    else
    {
        pApp->super_Threads.getTimerThread()->PushTimeReqEvent(pTimeEntity0);
    }

    pTimeEntity1 = ((GameDataPool*)pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId))
        ->createTimeEntity();
    pArg1 = pTimeEntity1->getArg();
    pArg1->workIndex = 0;
    pTimeEntity1->regist(0x1c, 10000, 0xffffffff, pTimeHandler, &HandlerFor_TE_::onTIME_AUCTION_STATISTICS_COLLECTOR);
    if (pTimeEntity1->check_period == 0)
    {
        pArg1b = pTimeEntity1->getArg();
        pApp->super_Threads.getWorkThread(pArg1b->workIndex)->PushTransaction((IMessageStruct*)pTimeEntity1);
    }
    else
    {
        pApp->super_Threads.getTimerThread()->PushTimeReqEvent(pTimeEntity1);
    }

    pTimeEntity2 = ((GameDataPool*)pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId))
        ->createTimeEntity();
    pArg2 = pTimeEntity2->getArg();
    pArg2->workIndex = 0;
    pTimeEntity2->regist(1, 5000, 0xffffffff, pTimeHandler, &HandlerFor_TE_::onTIME_CHECK_CONFIG);
    if (pTimeEntity2->check_period == 0)
    {
        pArg2b = pTimeEntity2->getArg();
        pApp->super_Threads.getWorkThread(pArg2b->workIndex)->PushTransaction((IMessageStruct*)pTimeEntity2);
    }
    else
    {
        pApp->super_Threads.getTimerThread()->PushTimeReqEvent(pTimeEntity2);
    }

    pTimeEntity3 = ((GameDataPool*)pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId))
        ->createTimeEntity();
    pArg3 = pTimeEntity3->getArg();
    pArg3->workIndex = 0;
    pTimeEntity3->regist(0x1d, 10000, 0xffffffff, pTimeHandler, &HandlerFor_TE_::onTIME_AUCTION_DB_PING);
    if (pTimeEntity3->check_period == 0)
    {
        pArg3b = pTimeEntity3->getArg();
        pApp->super_Threads.getWorkThread(pArg3b->workIndex)->PushTransaction((IMessageStruct*)pTimeEntity3);
    }
    else
    {
        pApp->super_Threads.getTimerThread()->PushTimeReqEvent(pTimeEntity3);
    }

    pTimeEntity4 = ((GameDataPool*)pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId))
        ->createTimeEntity();
    pArg4 = pTimeEntity4->getArg();
    pArg4->workIndex = 0;
    pTimeEntity4->regist(0x28, 60000, 0xffffffff, pTimeHandler, &HandlerFor_TE_::onTIME_AUCTION_UPDATE_AVERAGE_PRICE);
    if (pTimeEntity4->check_period == 0)
    {
        pArg4b = pTimeEntity4->getArg();
        pApp->super_Threads.getWorkThread(pArg4b->workIndex)->PushTransaction((IMessageStruct*)pTimeEntity4);
    }
    else
    {
        pApp->super_Threads.getTimerThread()->PushTimeReqEvent(pTimeEntity4);
    }
    G_TraceLog()->sysLog(8, "Out initTimeEvent");
}

unsigned long HandlerFor_TE_::onTIME_AUCTION_EXPIRE_EVENT_CHECK(nsl::InternalMsg* pArg)
{
    unsigned long return_code = 0;
    G_TraceLog()->sysLog(8, "In  onTIME_AUCTION_EXPIRE_EVENT_CHECK");
    if (((GSArea*)G_Zone()->mArea[0])->IsServiceRunning())
    {
        return_code = G_Auction()->ProcessMostRecentExpireItem();
    }
    G_TraceLog()->sysLog(8, "Out  onTIME_AUCTION_EXPIRE_EVENT_CHECK");
    return 0;
}

unsigned long HandlerFor_TE_::onTIME_AUCTION_STATISTICS_COLLECTOR(nsl::InternalMsg* pArg)
{
    unsigned long result = 0;
    G_TraceLog()->sysLog(8, "In  onTIME_AUCTION_STATISTICS_COLLECTOR");
    G_StatisticsCollector()->LoggingPerSec();
    G_StatisticsCollector()->DataInitialization(false);
    pApp->super_Threads.getWorkThread(0)->InitTransactionCntPerSec();
    pApp->super_Threads.getDBThread(0)->mTransactionCntPerSec = 0;
    G_TraceLog()->sysLog(8, "Out  onTIME_AUCTION_STATISTICS_COLLECTOR");
    return 0;
}

unsigned long HandlerFor_TE_::onTIME_CHECK_CONFIG(nsl::InternalMsg* pArg)
{
    struct stat st;
    // volatile forces ORIG: store rst, reload, shr $0x1f, test (not cmpl/jns)
    volatile int rst = stat(nsl::configpath, &st);
    if (rst < 0)
    {
        G_TraceLog()->sysLog(7, "onTIME_CHECK_CONFIG stat() : path[%s]", nsl::configpath);
        return 1;
    }
    static time_t old_time = 0;
    if (st.st_mtim.tv_sec != old_time)
    {
        nsl::is_config_changed = true;
    }
    if (nsl::is_config_reloaded != false)
    {
        G_TraceLog()->setMask();
        nsl::is_config_reloaded = false;
    }
    old_time = st.st_mtim.tv_sec;
    return 0;
}

unsigned long HandlerFor_TE_::onTIME_AUCTION_DB_PING(nsl::InternalMsg* pArg)
{
    unsigned long result = 0;
    tagAUCTION_DB_PING dbtrPing;
    nsl::Message* pMsg = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->createMessage(3);
    nsl::CMsgCell* pNewCell = pMsg->getCellFromMessage();
    *pNewCell << &dbtrPing;
    pApp->super_Threads.getDBThread(0)->PushTransaction(pMsg);
    return 0;
}

unsigned long HandlerFor_TE_::onTIME_AUCTION_TRY_SHUTDOWN(nsl::InternalMsg* pArg)
{
    unsigned long result = 0;
    G_TraceLog()->sysLog(7, "In  onTIME_AUCTION_TRY_SHUTDOWN");
    size_t pendingWorkNumSum = 0;
    for (int i = 0; i < pApp->super_Threads.getWorkThreadNum(); i = i + 1)
    {
        // No intermediate temp: ORIG does load-sum / add / store-sum
        pendingWorkNumSum = pendingWorkNumSum + pApp->super_Threads.getWorkThread(i)->GetQueueSizeNoLock();
    }
    int pendingDbNum = 0;
    pendingDbNum = pApp->super_Threads.getDBThread(0)->mQueueSize;
    G_TraceLog()->sysLog(5, "work queue size: %d, db queue size: %d", (int)pendingWorkNumSum, pendingDbNum);
    nsl::IArea* pArea = G_Zone()->mArea[0];
    if ((pendingWorkNumSum == 0) && (pendingDbNum == 0))
    {
        nsl::TSystem<nsl::LinuxSystem>::sleep(1000);
        G_TraceLog()->sysLog(5, "These two should be 0, work queue size: %d, db queue size: %d", (int)pendingWorkNumSum, pendingDbNum);
        nsl::LinuxService::getInstance()->controlStop();
    }
    else
    {
        PCK_AUCTION_SERVICE_UNAVAILABLE_AG pck;
        nsl::MAP_OBJECTS_ITER iter = pArea->getBeginIter();
        while (!pArea->isIterEnd(iter))
        {
            nsl::ISession* pCharacter = pArea->getValueFromIter(iter);
            nsl::Message* pNewMsg =
                pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)
                    ->getSendMessage(pCharacter->getTCPUser());
            nsl::CMsgCell* pNewCell = pNewMsg->getCellFromMessage();
            *pNewCell << &pck;
            pNewCell->PAD();
            sendTCP_->PushSendMsg(pNewMsg);
            ++iter;
        }
        G_TraceLog()->sysLog(7, "Out  onTIME_AUCTION_TRY_SHUTDOWN");
    }
    return 0;
}

unsigned long HandlerFor_TE_::onTIME_AUCTION_UPDATE_AVERAGE_PRICE(nsl::InternalMsg* pArg)
{
    G_TraceLog()->sysLog(8, "In  onTIME_AUCTION_UPDATE_AVERAGE_PRICE");
    time_t current_time;
    time(&current_time);
    struct tm* now_tm = localtime(&current_time);
    // Nested ifs (not &&) — ORIG short-circuits without materializing a bool
    if (((GSArea*)G_Zone()->mArea[0])->IsServiceRunning())
    {
        if (mOldTM.tm_mday != now_tm->tm_mday)
        {
            if (now_tm->tm_hour == 6)
            {
                G_Auction()->UpdateAveragePrice();
                mOldTM = *now_tm;
                G_TraceLog()->sysLog(5, "UPDATE AVERAGE PRICE");
            }
        }
    }
    G_TraceLog()->sysLog(8, "Out  onTIME_AUCTION_UPDATE_AVERAGE_PRICE");
    return 0;
}
