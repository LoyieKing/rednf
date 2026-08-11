// nsl::TCPThread.cpp
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "TCPThread.h"
#include "ServiceFactory.h"
#include "Threads.h"
#include "Reactor.h"
#include "DataPool.h"
#include "TCPUser.h"
#include "TCPSocket.h"
#include "Message.h"
#include "TraceLog.h"
#include "Script.h"
#include "TActiveConnect.h"
#include "ActiveNetClose.h"
#include "TCPSendThread.h"
#include "CommonDataPool.h"
#include "PacketDesign.h"

namespace nsl {

extern char configpath[256];
extern bool is_config_changed;
extern bool is_config_reloaded;

TCPThread::TCPThread()
{
    nPort_ = 0;
}

TCPThread::~TCPThread()
{
}

void TCPThread::loop(void* temp)
{
    puts("Start up TCPThread");
    G_TraceLog()->sysLog(8, "Start up TCPThread");
    DataPool* pPool = pApp->super_DataPools.getDataPool();
    EpollReactor<TCPUser>* r = pApp->super_Reactor.getReactor();
    r->init(6000);
    r->startup();
    TCPSendThread* tcp_send_thread = pApp->super_Threads.getTCPSendThread();
    // ORIG declares a POD PACKET_HEADER (this TU's header view has no ctor);
    // our shared header's ctor would emit an extra call, so mirror the 18-byte
    // layout with a packed POD struct whose setters alias the real
    // PACKET_HEADER methods (asm labels) — no ctor call, same call targets.
    struct __attribute__((packed)) heart_beat_packet_t
    {
        char classification;
        char msg_no;
        unsigned int sLength;
        unsigned int check_sum;
        unsigned int sequence;
        unsigned int ack;
        void setCategory(int n) __asm__("_ZN3nsl13PACKET_HEADER11setCategoryEi");
        void setPacketID(int n) __asm__("_ZN3nsl13PACKET_HEADER11setPacketIDEi");
        void setSize(int n) __asm__("_ZN3nsl13PACKET_HEADER7setSizeEi");
    } heart_beat_packet;
    memset(&heart_beat_packet, 0, 0x12);
    heart_beat_packet.setCategory(-1);
    heart_beat_packet.setPacketID(0);
    heart_beat_packet.setSize(0x12);
    std::map<unsigned int, TCPUser*>::iterator client_iter_begin;
    std::map<unsigned int, TCPUser*>::iterator client_iter_end;
    std::map<unsigned int, TCPUser*>::iterator client_iter;
    long long HEART_BEAT_CHECK_INTERVAL = 30000;
    long long last_heartbeat_check_time = 30000;
    {
        long long last_heartbeat_check_time = pApp->getTick();
        unsigned short accport = (unsigned short)getPort();
        TCPUser* sUser = pApp->super_DataPools.getDataPool()->createTCPUser();
        TCPSocket listenSocket;
        long long now_time;
        if (!listenSocket.open())
        {
            G_TraceLog()->sysLog(0, "Fail to open socket");
            goto fail;
        }
        if (G_Script()->findIntValue(0, 4) != 0x5209)
        {
            if (!listenSocket.bind(accport, false))
            {
                G_TraceLog()->sysLog(0, "Fail to bind listen : reason=%s", strerror(errno));
                goto fail;
            }
            if (!listenSocket.listen(0x400))
            {
                G_TraceLog()->sysLog(0, "Fail to Create AcceptThread");
                goto fail;
            }
        }
        sUser->setSocket(&listenSocket);
        // Fail-first regist: if not 0x5209 mode, require registListenHandle.
        if (G_Script()->findIntValue(0, 4) != 0x5209)
        {
            if (!r->registListenHandle(sUser))
            {
                G_TraceLog()->sysLog(0, "Fail to regist socket");
                goto fail;
            }
        }
        while (true)
        {
            pApp->setTick();
            r->handleEvents(3, pPool);
            if (pApp->super_IActiveConManager.getActiveConManager()->isRequestInQueue())
            {
                TCPUser* active_connect_user = NULL;
                pApp->super_IActiveConManager.getActiveConManager()->PopRequestConnect(active_connect_user);
                if (active_connect_user != NULL)
                {
                    r->addConnectedUser(active_connect_user->getUserId(), active_connect_user, true);
                }
            }
            if (is_config_changed)
            {
                bool ret = G_Script()->load(configpath);
                if (!ret)
                {
                    printf("Can't open script file : %s\n", configpath);
                }
                else
                {
                    printf("Reloading script file : %s\n", configpath);
                }
                G_Script()->get_sections();
                is_config_changed = false;
                is_config_reloaded = true;
            }
            if (G_ActiveNetClose()->isInData())
            {
                unsigned int active_close_user_id;
                TCPUser* active_close_user = NULL;
                G_ActiveNetClose()->onActvieClose(active_close_user_id, active_close_user);
                if (active_close_user != NULL)
                {
                    G_TraceLog()->sysLog(5, "active close, tcpuserid(%d)", active_close_user_id);
                    r->delConnectedUser(active_close_user_id, active_close_user);
                }
            }
            now_time = pApp->getTick();
            long long delta = now_time - last_heartbeat_check_time;
            if (delta > 30000)
            {
                last_heartbeat_check_time = now_time;
                r->GetConnectedUsersIter(client_iter_begin, client_iter_end, true);
                for (client_iter = client_iter_begin; client_iter != client_iter_end; ++client_iter)
                {
                    if ((tcp_send_thread != NULL)
                            ? (!client_iter->second->isAboutToDisconnect()) ? true : false
                            : false)
                    {
                        Message* msg = pApp->super_DataPools.getCommonDataPool(tlsThreadId)->getSendMessage(client_iter->second);
                        CMsgCell* cell = msg->getCellFromMessage();
                        *cell << (PACKET_HEADER*)&heart_beat_packet;
                        cell->PAD();
                        tcp_send_thread->PushSendMsg(msg);
                        register int servercount = r->GetServerUserCount();
                        G_TraceLog()->sysLog(5, "heartbeat SEND, server(%d) reactor-servermapsize(%d)", client_iter->second->getUserId(), servercount);
                    }
                }
            }
        }
fail:
        r->shutdown();
        Thread::setTerminated();
        exit(1);
    }
}

} // namespace nsl
