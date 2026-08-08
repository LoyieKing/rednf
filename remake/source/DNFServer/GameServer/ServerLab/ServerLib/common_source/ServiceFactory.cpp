#include <stdio.h>
#include <string.h>
#include <string>

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
    size_t most_little_queue_size = threadWork_[start]->orderQueue.size();
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
    size_t most_little_queue = threadWork_[0]->orderQueue.size();
    for (int i = 1; i < mWorkThreadNum; i = i + 1)
    {
        size_t sVar1 = threadWork_[i]->orderQueue.size();
        if ((int)sVar1 < (int)most_little_queue)
        {
            mMostLittleOrderQueue = i;
            most_little_queue = sVar1;
        }
    }
    most_little_queue = threadTCPSend_[0]->sendQueue.size();
    for (int i = 1; i < 1; i = i + 1)
    {
        size_t sVar1 = threadTCPSend_[i]->sendQueue.size();
        if ((int)sVar1 < (int)most_little_queue)
        {
            mMostLittleSendQueue = i;
            most_little_queue = sVar1;
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
    int iVar1 = (int)serviceConfigName.find_last_of("\\");
    int iVar2 = (int)serviceConfigName.find_last_of("/");
    int size = (iVar1 - iVar2) + -1;
    serviceConfigName.find_last_of("/");
    std::string sub = serviceConfigName.substr(iVar2 + 1);
    serviceConfigName = sub;
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
    ret = G_TraceLog()->init(ServiceFactoryName, ServiceConfigName);
    if (ret >= 0)
    {
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
        super_Dispatchers.dispatcherTCP = new TCPDispatcher;
        puts("TCPDispatcher has been created...");
        super_Dispatchers.mpInterDispatcher = new InterDispatcher;
        puts("InterDispatcher has been created...");
        super_Dispatchers.dispatcherDB = new DBDispatcher;
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
        std::map<int, INetWorkHandler*>::iterator network_iter;
        network_iter = super_IHandlers.mNetWorkHandlers.begin();
        for (; network_iter != super_IHandlers.mNetWorkHandlers.end(); ++network_iter)
        {
            network_iter->second->init();
        }
        std::map<int, IDBHandler*>::iterator db_iter;
        db_iter = super_IHandlers.mDBHandlers.begin();
        db_iter->second->init();
        if (!super_IHandlers.mInterHandlers.empty())
        {
            std::map<int, IInterHandler*>::iterator inter_iter;
            inter_iter = super_IHandlers.mInterHandlers.begin();
            inter_iter->second->init();
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
    return 0;
}

} // namespace nsl
