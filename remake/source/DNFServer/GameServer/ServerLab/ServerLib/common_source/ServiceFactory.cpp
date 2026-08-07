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
#include "ActiveConManager.h"
#include "EncyptTools.h"
#include "Message.h"

namespace nsl {

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
        size_t size = threadWork_[i]->orderQueue.size();
        if (size < most_little_queue_size)
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
        size_t size = threadWork_[i]->orderQueue.size();
        if ((int)size < (int)most_little_queue)
        {
            mMostLittleOrderQueue = i;
            most_little_queue = size;
        }
    }
    most_little_queue = threadTCPSend_[0]->sendQueue.size();
    for (int i = 1; i < 1; i = i + 1)
    {
        size_t size = threadTCPSend_[i]->sendQueue.size();
        if ((int)size < (int)most_little_queue)
        {
            mMostLittleSendQueue = i;
            most_little_queue = size;
        }
    }
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
    puts("\xB1\xE2\xBF\xB5\xC3\xBC \BD\xC3\xC0\xDB");
    printf("%d\n", (int)super_IHandlers.mInterHandlers.size());
    if (G_TraceLog()->init(ServiceFactoryName, ServiceConfigName) >= 0)
    {
        super_DataPools.dataPool_ = new DataPool;
        PortInfo::setUDPPort(&super_PortInfo, G_Script()->findIntValue(0, 5));
        PortInfo::setTCPPort(&super_PortInfo, G_Script()->findIntValue(0, 4));
        TCPSocket::SetRecvBufSize(G_Script()->findIntValue(0, 10));
        TCPSocket::SetSendBufSize(G_Script()->findIntValue(0, 0xb));
        printf("%d\n", (int)super_IHandlers.mInterHandlers.size());
        super_Threads.threadTCPSend_[0] = new TCPSendThread;
        puts("\xB1\xE2\xBF\xB5\xC3\xBC \C6\xF7\xB9\xD6\xC0\xCC\xBC\xAD\xB9\xF6 \C6\xC7\xB8\xAE");
        super_IActiveConManager.activeConManager_ = new ActiveConManager;
        puts("\xB1\xE2\xBF\xB5\xC3\xBC \C6\xF7\xB9\xD6\xC0\xCC\xBC\xAD\xB9\xF6 \C6\xC7\xB8\xAE");
        super_Threads.threadTimer_ = new TimerThread;
        puts("\xB1\xE2\xBF\xB5\xC3\xBC \C6\xF7\xB9\xD6\xC0\xCC\xBC\xAD\xB9\xF6 \C6\xC7\xB8\xAE");
        G_TraceLog()->setLogSendThread(super_Threads.logSendThread_);
        puts("\xB1\xE2\xBF\xB5\xC3\xBC \C6\xF7\xB9\xD6\xC0\xCC\xBC\xAD\xB9\xF6 \C6\xC7\xB8\xAE");
        super_Threads.threadTCP_ = new TCPThread;
        super_Threads.threadTCP_->setPort(super_PortInfo.getTCPPort());
        puts("\xB1\xE2\xBF\xB5\xC3\xBC \C6\xF7\xB9\xD6\xC0\xCC\xBC\xAD\xB9\xF6 \C6\xC7\xB8\xAE");
        puts("\xB1\xE2\xBF\xB5\xC3\xBC \C6\xF7\xB9\xD6\xC0\xCC\xBC\xAD\xB9\xF6 \C6\xC7\xB8\xAE");
        super_Threads.threadDB_[0] = new NSLDBThread;
        puts("\xB1\xE2\xBF\xB5\xC3\xBC \C6\xF7\xB9\xD6\xC0\xCC\xBC\xAD\xB9\xF6 \C6\xC7\xB8\xAE");
        super_Dispatchers.dispatcherTCP = new TCPDispatcher;
        puts("TCPDispatcher has been created...");
        super_Dispatchers.mpInterDispatcher = new InterDispatcher;
        puts("InterDispatcher has been created...");
        super_Dispatchers.dispatcherDB = new DBDispatcher;
        puts("\xB1\xE2\xBF\xB5\xC3\xBC \C6\xF7\xB9\xD6\xC0\xCC\xBC\xAD\xB9\xF6 \C6\xC7\xB8\xAE");
        puts("\xB1\xE2\xBF\xB5\xC3\xBC \C6\xF7\xB9\xD6\xC0\xCC\xBC\xAD\xB9\xF6 \C6\xC7\xB8\xAE");
        G_TraceLog()->sysLog(0, "\xB1\xE2\xBF\xB5\xC3\xBC \C6\xF7\xB9\xD6\xC0\xCC\xBC\xAD\xB9\xF6 \C6\xC7\xB8\xAE");
        super_Threads.mWorkThreadNum = 1;
        for (int i = 0; i < super_Threads.mWorkThreadNum; i = i + 1)
        {
            super_Threads.threadWork_[i] = new WorkThread(i);
            super_Threads.threadWork_[i]->begin();
            printf("\xB1\xE2\xBF\xB5\xC3\xBC \B0\xA1\xB5\xBF %d\n", i);
        }
        TSystem<LinuxSystem>::sleep(2000);
        printf("\xBD\xBA\xC5\xA9\xB7\xB0\xC6\xC7\xB4\xE2 %x\n", super_Threads.threadDB_[0]);
        super_Threads.threadDB_[0]->begin();
        G_TraceLog()->sysLog(0, "\xBD\xBA\xC5\xA9\xB7\xB0\xC6\xC7\xB4\xE2 %x\n", super_Threads.threadDB_[0]);
        super_Threads.threadTCPSend_[0]->begin();
        G_TraceLog()->sysLog(0, "\xC0\xCC\xBA\xCE\xC6\xBC\xB8\xAE %x\n", super_Threads.threadTCPSend_[0]);
        super_Threads.threadTimer_->begin();
        printf("\xC0\xCC\xBA\xCE\xC6\xBC\xB8\xAE %x\n", super_Threads.threadTimer_);
        // ... handler setup continues
    }
    return 0;
}

int ServiceFactory::shutdown()
{
    return 0;
}

} // namespace nsl
