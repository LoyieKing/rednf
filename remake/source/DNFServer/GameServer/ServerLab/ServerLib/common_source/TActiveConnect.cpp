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
    while (true)
    {
        TCPUser* pUser = conInfo->getTCPUser();
        if (!(pUser->isDisconnected() || pUser->isAboutToDisconnect()))
        {
            return;
        }
        pUser = RequestConnect(conInfo);
        conInfo->setTCPUser(pUser);
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
                TCPUserConnectMap::iterator iter;
                for (iter = mapConnectedUser_.begin(); iter != mapConnectedUser_.end(); ++iter)
                {
                    printf("rConInfo->getId-%d\n", iter->second->getId());
                    if (iter->second->getId() == conInfo->getId())
                    {
                        TCPUser* pUser = iter->second->getTCPUser();
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
    ConInterface* conInfo = NULL;
    bool bExist = false;
    {
        TScopedLock<TThreadLock<ThreadLock_linux> > slock(LockInCon);
        if (!queueRequestConnect.empty())
        {
            conInfo = queueRequestConnect.front();
            queueRequestConnect.pop();
            bExist = true;
        }
        else
        {
            bRequestInQueue = false;
        }
    }
    if (bExist)
    {
        DataPool* pPool = pApp->super_DataPools.getDataPool();
        TCPSocket* sTCP = pPool->createTCPSocket();
        if (!sTCP->open())
        {
            puts("failed to open UDP socket port");
            pPool->destroyTCPSocket(sTCP);
            return false;
        }
        printf("try to connect-%s, %d\n", conInfo->getIp(), conInfo->getPort());
        timeval tv;
        tv.tv_sec = 3;
        tv.tv_usec = 0;
        if (sTCP->connect_nonb(conInfo->getIp(), (unsigned short)conInfo->getPort(), tv))
        {
            conInfo->SetConnected(true);
            printf("connection success!!-%s, %d\n", conInfo->getIp(), conInfo->getPort());
            TCPUser* acUser = pPool->createTCPUser();
            if (acUser == NULL)
            {
                sTCP->close();
                pPool->destroyTCPSocket(sTCP);
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
            if (r->registHandle(acUser, 5))
            {
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
            puts("register handle fail");
            return false;
        }
        G_TraceLog()->sysLog(7, "failed to connect remote server-ip=%s, port=%d", conInfo->getIp(), conInfo->getPort());
        sTCP->close();
        pPool->destroyTCPSocket(sTCP);
        conInfo->mIsRejected = true;
        return false;
    }
    return false;
}

} // namespace nsl
