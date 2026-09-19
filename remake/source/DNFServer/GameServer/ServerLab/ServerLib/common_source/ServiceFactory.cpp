// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ServiceFactory/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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

char configpath[256];
ServiceFactory* pApp = 0;
bool is_config_changed = false;
bool is_config_reloaded = false;

#include "ServiceFactory/Threads.inc"
{
    puts("Start Threads~");
    mMostLittleOrderQueue = 0;
    mMostLittleSendQueue = 0;
    mWorkThreadNum = 0;
}

#include "ServiceFactory/getScopedLittleWorkIdx.inc"
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

#include "ServiceFactory/setMostLittleQueue.inc"
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

#include "ServiceFactory/EncyptTools.inc"
{
    puts("Start EncryptTools~");
    encyptTool_ = NULL;
}

#include "ServiceFactory/getNetWorkHandler.inc"
{
    std::map<int, INetWorkHandler*>::iterator iter =
        mNetWorkHandlers.find(category);
    if (iter != mNetWorkHandlers.end())
    {
        return mNetWorkHandlers[category];
    }
    return NULL;
}

#include "ServiceFactory/getInterHandler.inc"
{
    std::map<int, IInterHandler*>::iterator iter =
        mInterHandlers.find(category);
    if (iter != mInterHandlers.end())
    {
        return mInterHandlers[category];
    }
    return NULL;
}

#include "ServiceFactory/getDBHandler.inc"
{
    std::map<int, IDBHandler*>::iterator iter =
        mDBHandlers.find(category);
    if (iter != mDBHandlers.end())
    {
        return mDBHandlers[category];
    }
    return NULL;
}

#include "ServiceFactory/ServiceFactory.inc"
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

#include "ServiceFactory/dtor_ServiceFactory.inc"
{
}

#include "ServiceFactory/setTick.inc"
{
    tick_ = nsl::get_ms_tick();
}

#include "ServiceFactory/getTick.inc"
{
    return tick_;
}

#include "ServiceFactory/startup.inc"
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
    // ORIG：plain `new`（ctor 体无调用不可抛 → 无 EH pads），
    // ebx→eax 传参、ebx→edx 存成员（v5 形态）。
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
    // ORIG：循环遍历 IHandlers::mTimeHandlers[]（@0x104），虚调用 vtable[2]
    // （init，无参）；不是 threadWork_（@0x24）。
    for (int i = 0; i < super_IHandlers.mTimeHandlerNum; i = i + 1)
    {
        super_IHandlers.mTimeHandlers[i]->init();
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

#include "ServiceFactory/shutdown.inc"
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
#if !remake_verify
    return 0;   // DNF-FIX: 非 void 函数落底 —— g++ 13 在此发 ud2（ORIG 的 GCC 4.x 只是返回残留 eax）
#endif
}

} // namespace nsl
