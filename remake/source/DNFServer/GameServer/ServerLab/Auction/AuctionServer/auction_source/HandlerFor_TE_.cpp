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

HandlerFor_TE_::HandlerFor_TE_()
{
}

HandlerFor_TE_::~HandlerFor_TE_()
{
}

void HandlerFor_TE_::init()
{
    nsl::IHandler::init();
    initTimeEvent();
    memset(&mOldTM, 0, sizeof(struct tm));
}

void HandlerFor_TE_::initTimeEvent()
{
    G_TraceLog()->sysLog(8, "In  initTimeEvent");
    nsl::TE_Entity<HandlerFor_TE_>* pTimeEntity;
    nsl::InternalMsg* pArg;

    pTimeEntity = ((GameDataPool*)pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId))
        ->createTimeEntity();
    pArg = pTimeEntity->getArg();
    pArg->workIndex = 0;
    pTimeEntity->regist(0x19, 2000, 0xffffffff, pTimeHandler, &HandlerFor_TE_::onTIME_AUCTION_EXPIRE_EVENT_CHECK);
    if (pTimeEntity->check_period == 0)
    {
        pApp->super_Threads.getWorkThread(pTimeEntity->getArg()->workIndex)->PushTransaction((IMessageStruct*)pTimeEntity);
    }
    else
    {
        pApp->super_Threads.getTimerThread()->PushTimeReqEvent(pTimeEntity);
    }

    pTimeEntity = ((GameDataPool*)pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId))
        ->createTimeEntity();
    pArg = pTimeEntity->getArg();
    pArg->workIndex = 0;
    pTimeEntity->regist(0x1c, 10000, 0xffffffff, pTimeHandler, &HandlerFor_TE_::onTIME_AUCTION_STATISTICS_COLLECTOR);
    if (pTimeEntity->check_period == 0)
    {
        pApp->super_Threads.getWorkThread(pTimeEntity->getArg()->workIndex)->PushTransaction((IMessageStruct*)pTimeEntity);
    }
    else
    {
        pApp->super_Threads.getTimerThread()->PushTimeReqEvent(pTimeEntity);
    }

    pTimeEntity = ((GameDataPool*)pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId))
        ->createTimeEntity();
    pArg = pTimeEntity->getArg();
    pArg->workIndex = 0;
    pTimeEntity->regist(1, 5000, 0xffffffff, pTimeHandler, &HandlerFor_TE_::onTIME_CHECK_CONFIG);
    if (pTimeEntity->check_period == 0)
    {
        pApp->super_Threads.getWorkThread(pTimeEntity->getArg()->workIndex)->PushTransaction((IMessageStruct*)pTimeEntity);
    }
    else
    {
        pApp->super_Threads.getTimerThread()->PushTimeReqEvent(pTimeEntity);
    }

    pTimeEntity = ((GameDataPool*)pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId))
        ->createTimeEntity();
    pArg = pTimeEntity->getArg();
    pArg->workIndex = 0;
    pTimeEntity->regist(0x1d, 10000, 0xffffffff, pTimeHandler, &HandlerFor_TE_::onTIME_AUCTION_DB_PING);
    if (pTimeEntity->check_period == 0)
    {
        pApp->super_Threads.getWorkThread(pTimeEntity->getArg()->workIndex)->PushTransaction((IMessageStruct*)pTimeEntity);
    }
    else
    {
        pApp->super_Threads.getTimerThread()->PushTimeReqEvent(pTimeEntity);
    }

    pTimeEntity = ((GameDataPool*)pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId))
        ->createTimeEntity();
    pArg = pTimeEntity->getArg();
    pArg->workIndex = 0;
    pTimeEntity->regist(0x28, 60000, 0xffffffff, pTimeHandler, &HandlerFor_TE_::onTIME_AUCTION_UPDATE_AVERAGE_PRICE);
    if (pTimeEntity->check_period == 0)
    {
        pApp->super_Threads.getWorkThread(pTimeEntity->getArg()->workIndex)->PushTransaction((IMessageStruct*)pTimeEntity);
    }
    else
    {
        pApp->super_Threads.getTimerThread()->PushTimeReqEvent(pTimeEntity);
    }
    G_TraceLog()->sysLog(8, "Out initTimeEvent");
}

unsigned long HandlerFor_TE_::onTIME_AUCTION_EXPIRE_EVENT_CHECK(nsl::InternalMsg* pArg)
{
    G_TraceLog()->sysLog(8, "In  onTIME_AUCTION_EXPIRE_EVENT_CHECK");
    Zone* pZone = G_Zone();
    if (((GSArea*)pZone->mArea[0])->IsServiceRunning())
    {
        G_Auction()->ProcessMostRecentExpireItem();
    }
    G_TraceLog()->sysLog(8, "Out  onTIME_AUCTION_EXPIRE_EVENT_CHECK");
    return 0;
}

unsigned long HandlerFor_TE_::onTIME_AUCTION_STATISTICS_COLLECTOR(nsl::InternalMsg* pArg)
{
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
    int rst = stat(nsl::configpath, &st);
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
    tagAUCTION_DB_PING dbtrPing;
    nsl::Message* pMsg = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->createMessage(3);
    nsl::CMsgCell* pNewCell = pMsg->getCellFromMessage();
    *pNewCell << &dbtrPing;
    pApp->super_Threads.getDBThread(0)->PushTransaction(pMsg);
    return 0;
}

unsigned long HandlerFor_TE_::onTIME_AUCTION_TRY_SHUTDOWN(nsl::InternalMsg* pArg)
{
    G_TraceLog()->sysLog(7, "In  onTIME_AUCTION_TRY_SHUTDOWN");
    size_t pendingWorkNumSum = 0;
    for (int i = 0; i < pApp->super_Threads.getWorkThreadNum(); i++)
    {
        pendingWorkNumSum += pApp->super_Threads.getWorkThread(i)->GetQueueSizeNoLock();
    }
    int pendingDbNum = pApp->super_Threads.getDBThread(0)->mQueueSize;
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
            nsl::ISession* pSession = pArea->getValueFromIter(iter);
            nsl::TCPUser* u = pSession->getTCPUser();
            nsl::Message* pNewMsg = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->getSendMessage(u);
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
    Zone* pZone = G_Zone();
    if ((((GSArea*)pZone->mArea[0])->IsServiceRunning()) && (mOldTM.tm_mday != now_tm->tm_mday) && (now_tm->tm_hour == 6))
    {
        G_Auction()->UpdateAveragePrice();
        mOldTM.tm_sec = now_tm->tm_sec;
        mOldTM.tm_min = now_tm->tm_min;
        mOldTM.tm_hour = now_tm->tm_hour;
        mOldTM.tm_mday = now_tm->tm_mday;
        mOldTM.tm_mon = now_tm->tm_mon;
        mOldTM.tm_year = now_tm->tm_year;
        mOldTM.tm_wday = now_tm->tm_wday;
        mOldTM.tm_yday = now_tm->tm_yday;
        mOldTM.tm_isdst = now_tm->tm_isdst;
        mOldTM.tm_gmtoff = now_tm->tm_gmtoff;
        mOldTM.tm_zone = now_tm->tm_zone;
        G_TraceLog()->sysLog(5, "UPDATE AVERAGE PRICE");
    }
    G_TraceLog()->sysLog(8, "Out  onTIME_AUCTION_UPDATE_AVERAGE_PRICE");
    return 0;
}
