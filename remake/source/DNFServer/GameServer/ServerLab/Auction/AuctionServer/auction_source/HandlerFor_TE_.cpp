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

    // ORIG DWARF：每段 timer 一个词法块（entity+pArg），分支 getArg 在 if 内
    // 嵌套块复用同名 pArg；槽位按声明序连续 -0x44..-0xc
    {
        nsl::TE_Entity<HandlerFor_TE_>* ptime_expire_event;
        nsl::InternalMsg* pArg;
        ptime_expire_event = ((GameDataPool*)pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId))
            ->createTimeEntity();
        pArg = ptime_expire_event->getArg();
        pArg->workIndex = 0;
        ptime_expire_event->regist(0x19, 2000, 0xffffffff, pTimeHandler,
                                   &HandlerFor_TE_::onTIME_AUCTION_EXPIRE_EVENT_CHECK);
        if (ptime_expire_event->check_period == 0)
        {
            nsl::InternalMsg* pArg;
            pArg = ptime_expire_event->getArg();
            pApp->super_Threads.getWorkThread(pArg->workIndex)
                ->PushTransaction((IMessageStruct*)ptime_expire_event);
        }
        else
        {
            pApp->super_Threads.getTimerThread()->PushTimeReqEvent(ptime_expire_event);
        }
    }
    {
        nsl::TE_Entity<HandlerFor_TE_>* ptime_stlog_event;
        nsl::InternalMsg* pArg;
        ptime_stlog_event = ((GameDataPool*)pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId))
            ->createTimeEntity();
        pArg = ptime_stlog_event->getArg();
        pArg->workIndex = 0;
        ptime_stlog_event->regist(0x1c, 10000, 0xffffffff, pTimeHandler,
                                  &HandlerFor_TE_::onTIME_AUCTION_STATISTICS_COLLECTOR);
        if (ptime_stlog_event->check_period == 0)
        {
            nsl::InternalMsg* pArg;
            pArg = ptime_stlog_event->getArg();
            pApp->super_Threads.getWorkThread(pArg->workIndex)
                ->PushTransaction((IMessageStruct*)ptime_stlog_event);
        }
        else
        {
            pApp->super_Threads.getTimerThread()->PushTimeReqEvent(ptime_stlog_event);
        }
    }
    {
        nsl::TE_Entity<HandlerFor_TE_>* ptime_cfg_event;
        nsl::InternalMsg* pArg;
        ptime_cfg_event = ((GameDataPool*)pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId))
            ->createTimeEntity();
        pArg = ptime_cfg_event->getArg();
        pArg->workIndex = 0;
        ptime_cfg_event->regist(1, 5000, 0xffffffff, pTimeHandler,
                                &HandlerFor_TE_::onTIME_CHECK_CONFIG);
        if (ptime_cfg_event->check_period == 0)
        {
            nsl::InternalMsg* pArg;
            pArg = ptime_cfg_event->getArg();
            pApp->super_Threads.getWorkThread(pArg->workIndex)
                ->PushTransaction((IMessageStruct*)ptime_cfg_event);
        }
        else
        {
            pApp->super_Threads.getTimerThread()->PushTimeReqEvent(ptime_cfg_event);
        }
    }
    {
        nsl::TE_Entity<HandlerFor_TE_>* pTimeDbPing;
        nsl::InternalMsg* pArg;
        pTimeDbPing = ((GameDataPool*)pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId))
            ->createTimeEntity();
        pArg = pTimeDbPing->getArg();
        pArg->workIndex = 0;
        pTimeDbPing->regist(0x1d, 10000, 0xffffffff, pTimeHandler,
                            &HandlerFor_TE_::onTIME_AUCTION_DB_PING);
        if (pTimeDbPing->check_period == 0)
        {
            nsl::InternalMsg* pArg;
            pArg = pTimeDbPing->getArg();
            pApp->super_Threads.getWorkThread(pArg->workIndex)
                ->PushTransaction((IMessageStruct*)pTimeDbPing);
        }
        else
        {
            pApp->super_Threads.getTimerThread()->PushTimeReqEvent(pTimeDbPing);
        }
    }
    {
        nsl::TE_Entity<HandlerFor_TE_>* pTimeUpdateAveragePrice;
        nsl::InternalMsg* pArg;
        pTimeUpdateAveragePrice = ((GameDataPool*)pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId))
            ->createTimeEntity();
        pArg = pTimeUpdateAveragePrice->getArg();
        pArg->workIndex = 0;
        pTimeUpdateAveragePrice->regist(0x28, 60000, 0xffffffff, pTimeHandler,
                                        &HandlerFor_TE_::onTIME_AUCTION_UPDATE_AVERAGE_PRICE);
        if (pTimeUpdateAveragePrice->check_period == 0)
        {
            nsl::InternalMsg* pArg;
            pArg = pTimeUpdateAveragePrice->getArg();
            pApp->super_Threads.getWorkThread(pArg->workIndex)
                ->PushTransaction((IMessageStruct*)pTimeUpdateAveragePrice);
        }
        else
        {
            pApp->super_Threads.getTimerThread()->PushTimeReqEvent(pTimeUpdateAveragePrice);
        }
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
    int pendingWorkNumSum = 0;
    for (int i = 0; i < pApp->super_Threads.getWorkThreadNum(); i = i + 1)
    {
        // ORIG: load-sum / add / store-sum (call result first, then sum)
        pendingWorkNumSum = pApp->super_Threads.getWorkThread(i)->GetQueueSizeNoLock() + pendingWorkNumSum;
    }
    int pendingDbNum = 0;
    pendingDbNum = pApp->super_Threads.getDBThread(0)->mQueueSize;
    G_TraceLog()->sysLog(5, "work queue size: %d, db queue size: %d", pendingWorkNumSum, pendingDbNum);
    GSArea* pArea = (GSArea*)G_Zone()->mArea[0];
    nsl::ISession* pCharacter;
    if ((pendingWorkNumSum == 0) && (pendingDbNum == 0))
    {
        nsl::TSystem<nsl::LinuxSystem>::sleep(1000);
        G_TraceLog()->sysLog(5, "These two should be 0, work queue size: %d, db queue size: %d", pendingWorkNumSum, pendingDbNum);
        nsl::LinuxService::getInstance()->controlStop();
        return 0;
    }
    else
    {
        PCK_AUCTION_SERVICE_UNAVAILABLE_AG pck;
        nsl::MAP_OBJECTS_ITER iter = pArea->getBeginIter();
        while (!pArea->isIterEnd(iter))
        {
            pCharacter = pArea->getValueFromIter(iter);
            register nsl::TCPUser* pTCPUser = pCharacter->getTCPUser();
            nsl::Message* pNewMsg =
                pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)
                    ->getSendMessage(pTCPUser);
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
