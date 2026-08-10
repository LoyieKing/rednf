// df_manager_r — CTcpNetworkThread（ORIG DNFTcpNetworkThread.cpp）
#include "DNFTcpNetworkThread.h"

#include <errno.h>
#include <stdio.h>

#include "DNFTableBase.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "DNFTcpHandler.h"
#include "Peer.h"
#include "TcpNetSystem.h"

CTcpNetworkThread::CTcpNetworkThread() {}
CTcpNetworkThread::~CTcpNetworkThread() {}

void CTcpNetworkThread::attach(CTcpNetSystem* net)
{
    if (!net)
        return;
    m_net = net;
    m_recvQ = net->Get_TcpSwapQPacket()->GetRecvQ();
    m_handler = net->Get_TcpHandler();
    m_recvQLock = net->Get_TcpRecvQLock();
    m_recvBLock = net->Get_TcpRecvBLock();
    m_sendQ = net->Get_TcpSendQPacket();
    m_sendQLock = net->Get_TcpSendQLock();
    m_sendBLock = net->Get_TcpSendBLock();
}

void* CTcpNetworkThread::dispatch(void* param)
{
    m_runningFlag = 1;
    DNFFLib::Sleep_Ext(5, 0);
    try
    {
        while (1)
        {
            if (!m_runningFlag)
            {
                CMyFileLog log("dispatch", 0xae);
                log("./log/TcpRecv", "RecvThread Terminate");
                break;
            }
            errno = 0;
            DNFFLib::Sleep_Ext(5, 0);
            if (!m_net)
                break;
            m_net->SetEpollAcceptedPeers();
            m_net->SendPacket();
            int nEvent = m_net->WaitForEvent();
            if (nEvent == 0)
                continue;
            if (nEvent < 0)
            {
                if (errno == 0x4)
                    continue;
                if (errno != 0)
                    break;
            }
            for (int i = 0; i < nEvent; i++)
            {
                CPeer* peer = (CPeer*)m_handler->GetEventPtr(i);
                if (peer && m_handler->IsSetInEvent(i))
                {
                    if (!peer->RecvPacket())
                    {
                        peer->DisConnSig();
                        m_net->DeletePeer(peer);
                        peer = 0;
                    }
                }
                if (peer && peer->get_remain_sendlen() != 0 &&
                    m_handler->IsSetOutEvent(i) && peer->get_remain_sendlen() <= 0x1800)
                {
                    peer->send_packet();
                }
                m_handler->IsSetErrEvent(i);
            }
        }
    }
    catch (CDNFException& e)
    {
        printf("CTcpNetworkThread::dispatch() \xbf\xb9\xbf\xdc \xb9\xdf\xbb\xfd : %s\n", e.what());
        throw CDNFException("CTcpNetworkThread::dispatch() Recv  Socket Exception Break!");
    }
    catch (...)
    {
        puts("CTcpNetworkThread::dispatch() \xbf\xb9\xbf\xdc \xb9\xdf\xbb\xfd");
        throw CDNFException("CTcpNetworkThread::dispatch() Recv  Socket Exception Break!");
    }
    return 0;
}
