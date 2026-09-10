// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 TActiveConnect/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// nsl::TActiveConnect.cpp
#include <stdio.h>
#include <string.h>

#include "TActiveConnect.h"
#include "ServiceFactory.h"
#include "Reactor.h"
#include "DataPool.h"
#include "TCPUser.h"
#include "TCPSocket.h"
#include "TraceLog.h"
#include "ThreadLock.h"

namespace nsl {

#include "TActiveConnect/ActiveConManager.inc"
    : queueRequestConnect(std::deque<ConInterface*>())
{
    bRequestInQueue = false;
    bConnectedInQueue = false;
}

#include "TActiveConnect/setInfo.inc"
                           TCPUser::ENUM_DATA_TYPE send_data_type,
                           TCPUser::ENUM_DATA_TYPE recv_data_type, bool need_to_recon)
{
    this->con_id = con_id;
    this->remote_port = remote_port;
    this->send_data_type = send_data_type;
    this->recv_data_type = recv_data_type;
    this->need_to_recon = need_to_recon;
    strcpy(this->remote_ip, remote_ip);
    this->mConnected = false;
    this->mIsRejected = false;
}

#include "TActiveConnect/CheckTheConnection.inc"
{
    // ORIG: while (getTCPUser()->isDisconnected() || getTCPUser()->isAboutToDisconnect())
    // dual getTCPUser, condition-at-top (jmp to check), no local bool
    while (conInfo->getTCPUser()->isDisconnected() ||
           conInfo->getTCPUser()->isAboutToDisconnect())
    {
        conInfo->setTCPUser(RequestConnect(conInfo));
    }
}

#include "TActiveConnect/RequestConnect.inc"
{
    {
        TScopedLock<TThreadLock<ThreadLock_linux> > slock(LockInCon);
        queueRequestConnect.push(conInfo);
        bRequestInQueue = true;
    }
    while (true)
    {
        ConInterface* rConInfo;
        if (bConnectedInQueue != true)
        {
            if (conInfo->mIsRejected != false)
            {
                break;
            }
            TSystem<LinuxSystem>::sleep(10);
            continue;
        }
        {
            TScopedLock<TThreadLock<ThreadLock_linux> > slock(LockOutCon);
            if (mapConnectedUser_.empty())
            {
                bConnectedInQueue = false;
                TSystem<LinuxSystem>::sleep(10);
                continue;
            }
            for (TCPUserConnectMap::iterator iter = mapConnectedUser_.begin();
                 iter != mapConnectedUser_.end(); iter++)
            {
                rConInfo = iter->second;
                printf("rConInfo->getId-%d\n", rConInfo->getId());
                if (conInfo->getId() == rConInfo->getId())
                {
                    conInfo->setTCPUser(rConInfo->getTCPUser());
                    mapConnectedUser_.erase(iter);
                    return conInfo->getTCPUser();
                }
            }
        }
        TSystem<LinuxSystem>::sleep(100);
    }
    return NULL;
}

#include "TActiveConnect/PopRequestConnect.inc"
{
    ConInterface* conInfo;
    timeval tv;
    register bool bRet;
    register int bExist;
    {
        TScopedLock<TThreadLock<ThreadLock_linux> > slock(LockInCon);
        if (queueRequestConnect.empty())
        {
            bRequestInQueue = false;
            bRet = false;
            bExist = 0;
        }
        else
        {
            conInfo = queueRequestConnect.front();
            queueRequestConnect.pop();
            bExist = 1;
        }
    }
    if (bExist)
    {
        TCPSocket* sTCP = pApp->super_DataPools.getDataPool()->createTCPSocket();
        if (sTCP->open() == false)
        {
            puts("failed to open UDP socket port");
            pApp->super_DataPools.getDataPool()->destroyTCPSocket(sTCP);
            bRet = false;
            goto END;
        }
        printf("try to connect-%s, %d\n", conInfo->getIp(), conInfo->getPort());
        tv.tv_sec = 3;
        tv.tv_usec = 0;
        // ORIG: fail branch first via xor $1 on connect result
        if (sTCP->connect_nonb(conInfo->getIp(), (unsigned short)conInfo->getPort(), tv) == false)
        {
            G_TraceLog()->sysLog(7, "failed to connect remote server-ip=%s, port=%d", conInfo->getIp(), conInfo->getPort());
            sTCP->close();
            pApp->super_DataPools.getDataPool()->destroyTCPSocket(sTCP);
            conInfo->mIsRejected = true;
            bRet = false;
            goto END;
        }
        conInfo->SetConnected(true);
        printf("connection success!!-%s, %d\n", conInfo->getIp(), conInfo->getPort());
        TCPUser* acUser = pApp->super_DataPools.getDataPool()->createTCPUser();
        if (acUser == NULL)
        {
            sTCP->close();
            pApp->super_DataPools.getDataPool()->destroyTCPSocket(sTCP);
            puts("cannot create TCP USER");
            bRet = false;
            goto END;
        }
        acUser->initialize();
        acUser->startupAfterSetSocket();
        acUser->setSocket(sTCP);
        acUser->setLastAccessTime();
        acUser->setNeedReconnect(conInfo->getNeedRecon());
        acUser->setSendDataType(conInfo->getSendType());
        acUser->setRecvDataType(conInfo->getRecvType());
        G_TraceLog()->sysLog(8, "Active Connected TCPUser id=[%d]", acUser->mUserId);
        TCPReactor* r = pApp->super_Reactor.getReactor();
        if (r->registHandle(acUser, 5) == false)
        {
            puts("register handle fail");
            bRet = false;
            goto END;
        }
        conInfo->setTCPUser(acUser);
        {
            TScopedLock<TThreadLock<ThreadLock_linux> > slock(LockOutCon);
            mapConnectedUser_[conInfo->getId()] = conInfo;
            bConnectedInQueue = true;
            printf("con map size-%d\n", (int)mapConnectedUser_.size());
        }
        outConnectedUser = acUser;
        bRet = true;
    }
END:
    return bRet;
}

} // namespace nsl
