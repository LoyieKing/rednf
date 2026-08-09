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

ActiveConManager::ActiveConManager()
    : queueRequestConnect(std::deque<ConInterface*>())
{
    bRequestInQueue = false;
    bConnectedInQueue = false;
}

void ConInterface::setInfo(int con_id, const char* remote_ip, int remote_port,
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

void ActiveConManager::CheckTheConnection(ConInterface* conInfo)
{
    // ORIG: while (getTCPUser()->isDisconnected() || getTCPUser()->isAboutToDisconnect())
    // dual getTCPUser, condition-at-top (jmp to check), no local bool
    while (conInfo->getTCPUser()->isDisconnected() ||
           conInfo->getTCPUser()->isAboutToDisconnect())
    {
        conInfo->setTCPUser(RequestConnect(conInfo));
    }
}

TCPUser* ActiveConManager::RequestConnect(ConInterface* conInfo)
{
    {
        TScopedLock<TThreadLock<ThreadLock_linux> > slock(LockInCon);
        queueRequestConnect.push(conInfo);
        bRequestInQueue = true;
    }
    while (true)
    {
        while (bConnectedInQueue != true)
        {
            if (conInfo->mIsRejected != false)
            {
                return NULL;
            }
            TSystem<LinuxSystem>::sleep(10);
        }
        int iVar4;
        TCPUser* pUserRet = NULL;
        {
            TScopedLock<TThreadLock<ThreadLock_linux> > slock(LockOutCon);
            if (mapConnectedUser_.empty())
            {
                bConnectedInQueue = false;
                TSystem<LinuxSystem>::sleep(10);
                iVar4 = 0;
            }
            else
            {
                for (TCPUserConnectMap::iterator iter = mapConnectedUser_.begin();
                     iter != mapConnectedUser_.end(); iter++)
                {
                    ConInterface* rConInfo = iter->second;
                    printf("rConInfo->getId-%d\n", rConInfo->getId());
                    if (rConInfo->getId() == conInfo->getId())
                    {
                        TCPUser* pUser = rConInfo->getTCPUser();
                        conInfo->setTCPUser(pUser);
                        mapConnectedUser_.erase(iter);
                        pUserRet = conInfo->getTCPUser();
                        iVar4 = 1;
                        goto done;
                    }
                }
                iVar4 = 2;
            }
        }
    done:
        if (iVar4 != 0)
        {
            if (iVar4 == 1)
            {
                return pUserRet;
            }
            TSystem<LinuxSystem>::sleep(100);
        }
    }
}

bool ActiveConManager::PopRequestConnect(TCPUser*& outConnectedUser)
{
    ConInterface* conInfo;
    int bExist;
    {
        TScopedLock<TThreadLock<ThreadLock_linux> > slock(LockInCon);
        // ORIG: if (empty()) { bRequestInQueue=false; esi=0 } else { pop; esi=1 }
        if (queueRequestConnect.empty())
        {
            bRequestInQueue = false;
            bExist = 0;
        }
        else
        {
            conInfo = queueRequestConnect.front();
            queueRequestConnect.pop();
            bExist = 1;
        }
    }
    if (bExist == 0)
    {
        return false;
    }

    TCPSocket* sTCP = pApp->super_DataPools.getDataPool()->createTCPSocket();
    if (sTCP->open() == false)
    {
        puts("failed to open UDP socket port");
        pApp->super_DataPools.getDataPool()->destroyTCPSocket(sTCP);
        return false;
    }
    printf("try to connect-%s, %d\n", conInfo->getIp(), conInfo->getPort());
    timeval tv;
    tv.tv_sec = 3;
    tv.tv_usec = 0;
    // ORIG: fail branch first via xor $1 on connect result
    if (sTCP->connect_nonb(conInfo->getIp(), (unsigned short)conInfo->getPort(), tv) == false)
    {
        G_TraceLog()->sysLog(7, "failed to connect remote server-ip=%s, port=%d", conInfo->getIp(), conInfo->getPort());
        sTCP->close();
        pApp->super_DataPools.getDataPool()->destroyTCPSocket(sTCP);
        conInfo->mIsRejected = true;
        return false;
    }
    conInfo->SetConnected(true);
    printf("connection success!!-%s, %d\n", conInfo->getIp(), conInfo->getPort());
    TCPUser* acUser = pApp->super_DataPools.getDataPool()->createTCPUser();
    if (acUser == NULL)
    {
        sTCP->close();
        pApp->super_DataPools.getDataPool()->destroyTCPSocket(sTCP);
        puts("cannot create TCP USER");
        return false;
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
        return false;
    }
    conInfo->setTCPUser(acUser);
    {
        TScopedLock<TThreadLock<ThreadLock_linux> > slock(LockOutCon);
        mapConnectedUser_[conInfo->getId()] = conInfo;
        bConnectedInQueue = true;
        printf("con map size-%d\n", (int)mapConnectedUser_.size());
    }
    outConnectedUser = acUser;
    return true;
}

} // namespace nsl
