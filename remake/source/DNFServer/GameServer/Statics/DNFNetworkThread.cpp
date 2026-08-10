// df_statics_r — DNFNetworkThread（ORIG DNFNetworkThread.o 拆分）
#include <queue>
#include <stdio.h>
#include <string.h>

#include "DNFNetworkThread.h"
#include "DNFThreadInterface.h"
#include "DNFApplication.h"
#include "DNFUdpHandler.h"
#include "DNFUdpRecvBuffer.h"
#include "DNFTableBase.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Thread.h"

CNetworkThread::CNetworkThread()
{
    m_queue = 0;
    m_udp = 0;
    m_lock = 0;
    m_bLock = 0;
}
CNetworkThread::~CNetworkThread()
{
    m_queue = 0;
    m_udp = 0;
    m_lock = 0;
}
void CNetworkThread::dispatch(void* param)
{
    try
    {
    if (m_queue != 0 && m_udp != 0 && m_lock != 0)
    {
        DNFFLib::Sleep_Ext(5, 0);
        puts("Network Thread Start!");
        m_running = 1;
        while (m_running != 0)
        {
            CUdpRecvBuffer* buf;
            {
                CGuard<CMutex> g((CMutex*)m_bLock);
                buf = (CUdpRecvBuffer*)CUdpRecvBuffer::operator new(0x1804);
            }
            int len = 0x1800;
            unsigned short port = 0;
            unsigned int ip = 0;
            if (((CUdpHandler*)m_udp)->RecvFromClient((char*)buf, &len, &ip, &port) == 1)
            {
                if (*(unsigned short*)((char*)buf + 2) == len)
                {
                    if (*(unsigned short*)((char*)buf + 2) < 0x1800)
                    {
                        if (len < 0x1801)
                        {
                            *(unsigned int*)((char*)buf + 6) = ip;
                            *(unsigned short*)((char*)buf + 4) = port;
                            {
                                CGuard<CMutex> g((CMutex*)m_lock);
                                ((std::queue<CUdpRecvBuffer*>*)m_queue)->push(buf);
                                unsigned int qsize =
                                    ((std::queue<CUdpRecvBuffer*>*)m_queue)->size();
                                if (100 < qsize)
                                {
                                    DNF_LOG_SCOPE_LINE(0xb2,"./log/recv", "cnt(%d)\n",
                                        ((std::queue<CUdpRecvBuffer*>*)m_queue)->size());
                                }
                            }
                        }
                        else
                        {
                            DNF_LOG_SCOPE_LINE(0x8d,"./log/recvErr",
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
                        DNF_LOG_SCOPE_LINE(0x81,"./log/recvErr",
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
                    DNF_LOG_SCOPE_LINE(0x76,"./log/recvErr",
                        "Packet Size is Incorrect! Packet Size( %d ), Recv Byte( %d ) Code( %d )\n",
                        *(unsigned short*)((char*)buf + 2), len, *(unsigned short*)buf);
                    {
                        CGuard<CMutex> g((CMutex*)m_bLock);
                        CUdpRecvBuffer::operator delete(buf);
                    }
                }
            }
            else
            {
                CGuard<CMutex> g((CMutex*)m_bLock);
                CUdpRecvBuffer::operator delete(buf);
            }
        }
        return;
    }
    throw CDNFException("NetworkThread is Not Ready!\n");
    }
    catch (std::exception& e)
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
void CNetworkThread::attach(CApplication* app)
{
    if (app != 0)
    {
        m_queue = app->Get_QPacket();
        m_udp = app->Get_UdpHandler();
        m_lock = app->Get_QLock();
        m_bLock = app->Get_BLock();
    }
}
