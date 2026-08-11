#include <stdio.h>
#include <string.h>
#include <string>
#include <new>

#include "ServiceFactory.h"
#include "Threads.h"
#include "TCPSocket.h"
#include "TraceLog.h"
#include "Script.h"
#include "DataPool.h"
#include "WorkThread.h"
#include "System.h"
#include "TimerThread.h"
#include "TCPThread.h"
#include "TCPSendThread.h"
#include "NSLDBThread.h"
#include "TCPDispatcher.h"
#include "InterDispatcher.h"
#include "DBDispatcher.h"
#include "TActiveConnect.h"
#include "EncyptTools.h"
#include "Message.h"
#include "DBConnection.h"

namespace nsl {

/*
 * ORIG startup() constructs these with operator new + C1 and no EH landing pads
 * (ctors are trivial / non-throwing). Calling via throw()-spec aliases prevents
 * gcc from emitting _ZdlPv/_Unwind_Resume around the construction.
 */
void call_TCPDispatcher_ctor(TCPDispatcher* p) throw() asm("_ZN3nsl13TCPDispatcherC1Ev");
void call_InterDispatcher_ctor(InterDispatcher* p) throw() asm("_ZN3nsl15InterDispatcherC1Ev");
void call_DBDispatcher_ctor(DBDispatcher* p) throw() asm("_ZN3nsl12DBDispatcherC1Ev");

char configpath[256];

Threads::Threads()
{
    puts("Start Threads~");
    mMostLittleOrderQueue = 0;
    mMostLittleSendQueue = 0;
    mWorkThreadNum = 0;
}

int Threads::getScopedLittleWorkIdx(int start, int end)
{
    int ret = start;
    // ORIG DWARF：most_little_queue_size 为 unsigned int。
    unsigned int most_little_queue_size = threadWork_[start]->orderQueue.size();
    for (int i = start; i < end; i = i + 1)
    {
        if (threadWork_[i]->orderQueue.size() < most_little_queue_size)
        {
            most_little_queue_size = threadWork_[i]->orderQueue.size();
            ret = i;
        }
    }
    return ret;
}

void Threads::setMostLittleQueue()
{
    // ORIG DWARF 声明序：compare(54) 先于 most_little_queue(56)。
    int compare;
    int most_little_queue = (int)threadWork_[0]->orderQueue.size();
    for (int i = 1; i < mWorkThreadNum; i = i + 1)
    {
        // ORIG DWARF 局部：compare(54)。
        compare = (int)threadWork_[i]->orderQueue.size();
        if (compare < most_little_queue)
        {
            // ORIG：most_little_queue = compare 先于 mMostLittleOrderQueue = i。
            most_little_queue = compare;
            mMostLittleOrderQueue = i;
        }
    }
    most_little_queue = (int)threadTCPSend_[0]->sendQueue.size();
    for (int i = 1; i < 1; i = i + 1)
    {
        compare = (int)threadTCPSend_[i]->sendQueue.size();
        if (compare < most_little_queue)
        {
            // ORIG：同上顺序。
            most_little_queue = compare;
            mMostLittleSendQueue = i;
        }
    }
}

EncyptTools::EncyptTools()
{
    puts("Start EncryptTools~");
    encyptTool_ = NULL;
}

INetWorkHandler* IHandlers::getNetWorkHandler(int category)
{
    std::map<int, INetWorkHandler*>::iterator iter =
        mNetWorkHandlers.find(category);
    if (iter != mNetWorkHandlers.end())
    {
        return mNetWorkHandlers[category];
    }
    return NULL;
}

IInterHandler* IHandlers::getInterHandler(int category)
{
    std::map<int, IInterHandler*>::iterator iter =
        mInterHandlers.find(category);
    if (iter != mInterHandlers.end())
    {
        return mInterHandlers[category];
    }
    return NULL;
}

IDBHandler* IHandlers::getDBHandler(int category)
{
    std::map<int, IDBHandler*>::iterator iter =
        mDBHandlers.find(category);
    if (iter != mDBHandlers.end())
    {
        return mDBHandlers[category];
    }
    return NULL;
}

ServiceFactory::ServiceFactory(char* _ServiceFactoryName, char* _ServiceConfigName)
{
    setTick();
    printf("_ServiceFactoryName-%s\n", _ServiceFactoryName);
    printf("_ServiceConfigName-%s\n", _ServiceConfigName);
    std::string serviceConfigName(_ServiceConfigName);
    // ORIG 二进制实测：第一个 find_last_of 用 "."（去扩展名），后两个用 "/"。
    // ORIG: iVar1 常驻 ebx；size 有栈槽 -0xc（store+reload）。
    register int iVar1 = (int)serviceConfigName.find_last_of(".");
    int size = (iVar1 - (int)serviceConfigName.find_last_of("/")) + -1;
    // ORIG: 无 (int) 强转（lea 0x1(%eax),%edx 形态）。
    serviceConfigName =
        serviceConfigName.substr(serviceConfigName.find_last_of("/") + 1, size);
    memset(ServiceFactoryName, 0, 0x15);
    memset(ServiceConfigName, 0, 0x51);
    strncpy(ServiceFactoryName, _ServiceFactoryName, 0x14);
    strncpy(ServiceConfigName, serviceConfigName.c_str(), 0x50);
}

ServiceFactory::~ServiceFactory()
{
}

void ServiceFactory::setTick()
{
    tick_ = nsl::get_ms_tick();
}

__int64 ServiceFactory::getTick() const
{
    return tick_;
}

int ServiceFactory::startup()
{
    int ret;
    puts("\xbc\xad\xb9\xf6 \xb6\xf3\xc0\xcc\xba\xea\xb7\xaf\xb8\xae \xb1\xb8\xb5\xbf \xbd\xc3\xc0\xdb...");
    printf("%d\n", (int)super_IHandlers.mInterHandlers.size());
    ret = 0;
    // ORIG: 赋值在条件内（mov n; shr $0x1f; test; je），失败提前 return。
    if ((ret = G_TraceLog()->init(ServiceFactoryName, ServiceConfigName)) < 0)
    {
        return ret;
    }
    super_DataPools.dataPool_ = new DataPool;
    super_PortInfo.setUDPPort(G_Script()->findIntValue(0, 5));
    super_PortInfo.setTCPPort(G_Script()->findIntValue(0, 4));
    TCPSocket::SetRecvBufSize(G_Script()->findIntValue(0, 10));
    TCPSocket::SetSendBufSize(G_Script()->findIntValue(0, 0xb));
    printf("%d\n", (int)super_IHandlers.mInterHandlers.size());
    super_Threads.threadTCPSend_[0] = new TCPSendThread;
    puts("TCPSendThread \xb0\xb4\xc3\xbc \xbf\xcf\xb7\xe1...");
    super_IActiveConManager.activeConManager_ = new ActiveConManager;
    puts("TimerThread \xb0\xb4\xc3\xbc \xbb\xfd\xbc\xba...");
    super_Threads.threadTimer_ = new TimerThread;
    puts("TimerThread \xb0\xb4\xc3\xbc \xbf\xcf\xb7\xe1...");
    G_TraceLog()->setLogSendThread(super_Threads.logSendThread_);
    puts("TCPThread \xb0\xb4\xc3\xbc \xbb\xfd\xbc\xba...");
    super_Threads.threadTCP_ = new TCPThread;
    super_Threads.threadTCP_->setPort(super_PortInfo.getTCPPort());
    puts("TCPThread \xb0\xb4\xc3\xbc \xbf\xcf\xb7\xe1...");
    puts("NSLDBThread \xb0\xb4\xc3\xbc \xbb\xfd\xbc\xba...");
    super_Threads.threadDB_[0] = new NSLDBThread;
    puts("NSLDBThread \xb0\xb4\xc3\xbc \xbf\xcf\xb7\xe1...");
    /* ORIG: operator new + C1, no EH pads (throw()-spec ctor aliases) */
    TCPDispatcher* pTCPDispatcher =
        (TCPDispatcher*)::operator new(sizeof(TCPDispatcher));
    call_TCPDispatcher_ctor(pTCPDispatcher);
    super_Dispatchers.dispatcherTCP = pTCPDispatcher;
    puts("TCPDispatcher has been created...");
    InterDispatcher* pInterDispatcher =
        (InterDispatcher*)::operator new(sizeof(InterDispatcher));
    call_InterDispatcher_ctor(pInterDispatcher);
    super_Dispatchers.mpInterDispatcher = pInterDispatcher;
    puts("InterDispatcher has been created...");
    DBDispatcher* pDBDispatcher = (DBDispatcher*)::operator new(sizeof(DBDispatcher));
    call_DBDispatcher_ctor(pDBDispatcher);
    super_Dispatchers.dispatcherDB = pDBDispatcher;
    puts("DBDispatcher \xb0\xb4\xc3\xbc \xbf\xcf\xb7\xe1...");
    puts("\xbc\xad\xba\xf1\xbd\xba \xbe\xb2\xb7\xb9\xb5\xe5 \xb1\xb8\xb5\xbf \xbd\xc3\xc0\xdb");
    G_TraceLog()->sysLog(0, "\xbc\xad\xba\xf1\xbd\xba \xbe\xb2\xb7\xb9\xb5\xe5 \xb1\xb8\xb5\xbf \xbd\xc3\xc0\xdb");
    super_Threads.mWorkThreadNum = 1;
    for (int i = 0; i < super_Threads.mWorkThreadNum; i = i + 1)
    {
            super_Threads.threadWork_[i] = new WorkThread(i);
        super_Threads.threadWork_[i]->begin();
        printf("%d\xb9\xf8 workThread \xb1\xb8\xb5\xbf \xbc\xba\xb0\xf8\n", i);
    }
    TSystem<LinuxSystem>::sleep(2000);
    printf("NSLDBThread 0 \xb9\xf8 \xb1\xb8\xb5\xbf \xbd\xc3\xc0\xdb-%p\n",
           super_Threads.threadDB_[0]);
    super_Threads.threadDB_[0]->begin();
    G_TraceLog()->sysLog(0, "NSLDBThread 0 \xb9\xf8 \xb1\xb8\xb5\xbf \xbc\xba\xb0\xf8-%p",
                         super_Threads.threadDB_[0]);
    super_Threads.threadTCPSend_[0]->begin();
    G_TraceLog()->sysLog(0, "TCPSendThread 0 \xb9\xf8 \xb1\xb8\xb5\xbf \xbc\xba\xb0\xf8-%p",
                         super_Threads.threadTCPSend_[0]);
    super_Threads.threadTimer_->begin();
    printf("TimerThread \xb1\xb8\xb5\xbf \xbc\xba\xb0\xf8-%p\n", super_Threads.threadTimer_);
    {
        std::map<int, INetWorkHandler*>::iterator iter;
        iter = super_IHandlers.mNetWorkHandlers.begin();
        for (; iter != super_IHandlers.mNetWorkHandlers.end(); ++iter)
        {
            iter->second->init();
        }
    }
    {
        std::map<int, IDBHandler*>::iterator iter;
        iter = super_IHandlers.mDBHandlers.begin();
        iter->second->init();
    }
    {
        std::map<int, IInterHandler*>::iterator iter;
        if (!super_IHandlers.mInterHandlers.empty())
        {
            iter = super_IHandlers.mInterHandlers.begin();
            iter->second->init();
        }
    }
    for (int i = 0; i < super_IHandlers.mTimeHandlerNum; i = i + 1)
    {
        super_Threads.threadWork_[i]->loop((void*)0);
    }
    super_Threads.threadTCP_->begin();
    printf("TCPThread \xb1\xb8\xb5\xbf \xbc\xba\xb0\xf8-%p\n", super_Threads.threadTCP_);
    puts("------------------------------------------");
    puts("-\t\tServer Frame Start Ver1.0       -");
    puts("------------------------------------------");
    G_TraceLog()->sysLog(5, "----------------------------------------------------------");
    G_TraceLog()->sysLog(5, "-\t\tServer Frame Start Ver1.0 \t\t\t\t    -");
    G_TraceLog()->sysLog(5, "----------------------------------------------------------");
    if (G_TraceLog()->checkLogServer())
    {
        G_TraceLog()->sysLog(0, "checkLogServer(), write_to_logserver is true");
    }
    else
    {
        G_TraceLog()->sysLog(0, "checkLogServer(), write_to_logserver is false");
    }
    return ret;
}

int ServiceFactory::shutdown()
{
    if (this != NULL)
    {
        std::map<int, DBConnection*>::iterator dbConnIter;
        dbConnIter = super_DBConnections.mDbConnections.begin();
        for (; dbConnIter != super_DBConnections.mDbConnections.end(); ++dbConnIter)
        {
            dbConnIter->second->close();
        }
    }
    /* ORIG: no explicit mov $0 before leave; return value is last ne() result (0) */
}

} // namespace nsl
