// df_coserver_r — CNetworkThread（ORIG DNFNetworkThread.cpp）
#include <stdio.h>
#include <queue>
#include <string.h>

#include "DNFNetworkThread.h"
#include "DNFApplication.h"
#include "DNFUdpHandler.h"
#include "DNFUdpRecvBuffer.h"
#include "DNFTableBase.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

CNetworkThread::CNetworkThread()
{
    m_udp = 0;
    m_bLock = 0;
    for (int i = 0; i < 10; i++)
    {
        m_queues[i] = 0;
        m_locks[i] = 0;
    }
}

CNetworkThread::~CNetworkThread()
{
    for (int i = 0; i < 10; i++)
    {
        m_queues[i] = 0;
        m_locks[i] = 0;
    }
    m_udp = 0;
}

void CNetworkThread::dispatch(void* param)
{
    try
    {
        if (m_queues == 0 || m_udp == 0 || m_locks == 0)
        {
            throw CDNFException("NetworkThread is Not Ready!\n");
        }
        DNFFLib::Sleep_Ext(5, 0);
        puts("Network Thread Start!");
        m_running = 1;
        while (m_running != 0)
        {
            CUdpRecvBuffer* buf;
            {
                CGuard<CMutex> g((CMutex*)m_bLock);
                buf = (CUdpRecvBuffer*)CUdpRecvBuffer::operator new(0x204);
            }
            int len = 0x200;
            unsigned short port = 0;
            unsigned int ip = 0;
            // ORIG 实测：`if (!RecvFromClient(...)) { error } else { success }` 形态
            // （xor $0x1; test; je 跳过错误块）；`== 1` 会生成 cmp/sete（仅差条件形态）。
            if (!((CUdpHandler*)m_udp)->RecvFromClient((char*)buf, &len, &ip, &port))
            {
                CGuard<CMutex> g((CMutex*)m_bLock);
                CUdpRecvBuffer::operator delete(buf);
            }
            else
            {
                {
                    if (*(unsigned short*)((char*)buf + 2) == len)
                    {
                        if (*(unsigned short*)((char*)buf + 2) < 0x200)
                        {
                            if (len < 0x201)
                            {
                                if (*(int*)((char*)buf + 6) == 0)
                                {
                                    printf("id(%d), m_id(%d)", *(unsigned short*)buf,
                                           *(int*)((char*)buf + 6));
                                }
                                unsigned int idx = *(unsigned int*)((char*)buf + 6) % 10;
                                {
                                    CGuard<CMutex> g((CMutex*)m_locks[idx]);
                                    ((std::queue<CUdpRecvBuffer*>*)m_queues[idx])->push(buf);
                                    unsigned int qsize =
                                        ((std::queue<CUdpRecvBuffer*>*)m_queues[idx])->size();
                                    if (100 < qsize)
                                    {
                                        DNF_LOG_SCOPE_LINE(0xa3,"./log/recv", "idx(%d) cnt(%d)", idx,
                                            ((std::queue<CUdpRecvBuffer*>*)m_queues[idx])->size());
                                    }
                                }
                            }
                            else
                            {
                                DNF_LOG_SCOPE_LINE(0x6f,"./log/recvErr",
                                    "Recv Byte is Over! Packet Size( %d ), Recv Byte( %d ) Code( %d )\n",
                                    *(unsigned short*)((char*)buf + 2), len,
                                    *(unsigned short*)buf);
                                {
                                    CGuard<CMutex> g((CMutex*)m_bLock);
                                    CUdpRecvBuffer::operator delete(buf);
                                }
                            }
                        }
                        else
                        {
                            DNF_LOG_SCOPE_LINE(0x7a,"./log/recvErr",
                                "Packet Size is Over! Packet Size( %d ), Recv Byte( %d ) Code( %d )\n",
                                *(unsigned short*)((char*)buf + 2), len, *(unsigned short*)buf);
                            {
                                CGuard<CMutex> g((CMutex*)m_bLock);
                                CUdpRecvBuffer::operator delete(buf);
                            }
                        }
                    }
                    else
                    {
                        DNF_LOG_SCOPE_LINE(0x86,"./log/recvErr",
                            "Packet Size is Incorrect! Packet Size( %d ), Recv Byte( %d ) Code( %d )\n",
                            *(unsigned short*)((char*)buf + 2), len, *(unsigned short*)buf);
                        {
                            CGuard<CMutex> g((CMutex*)m_bLock);
                            CUdpRecvBuffer::operator delete(buf);
                        }
                    }
                }
            }
        }
        return;
    }
    catch (CDNFException& e)
    {
        printf("CNetworkThread::dispatch() \xbf\xb9\xbf\xdc \xb9\xdf\xbb\xfd : %s\n", e.what());
        throw CDNFException("CNetworkThread::dispatch() Recv  Socket Exception Break!");
    }
    catch (...)
    {
        puts("CNetworkThread::dispatch() \xbf\xb9\xbf\xdc \xb9\xdf\xbb\xfd");
        throw CDNFException("CNetworkThread::dispatch() Recv  Socket Exception Break!");
    }
}

void CNetworkThread::attach(CApplication* app, int idx)
{
    if (app != 0)
    {
        m_udp = app->Get_UdpHandler(idx);
        for (int i = 0; i < 10; i++)
        {
            m_queues[i] = app->Get_QPacket() + i;
            m_locks[i] = app->Get_QLock() + i;
        }
        m_bLock = app->Get_BLock();
    }
}
