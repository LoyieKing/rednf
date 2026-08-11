// df_dbmw_r - DNFNetworkThread (ORIG DNFNetworkThread.cpp)
#include "DBMWCommon.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/resource.h>
#include <signal.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/times.h>

#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

int getErrno();

CNetworkThread::CNetworkThread()
{
    m_udpQueue = 0;
    m_udpHandler = 0;
    m_udpQLock = 0;
    m_udpBLock = 0;
}
CNetworkThread::~CNetworkThread()
{
    m_udpQueue = 0;
    m_udpHandler = 0;
    m_udpQLock = 0;
}
void CNetworkThread::attach(CApplication* app)
{
    if (!app)
        return;
    m_udpQueue = app->Get_UdpPacketRecvQ();
    m_udpHandler = app->Get_UdpHandler();
    m_udpQLock = app->Get_QLock();
    m_udpBLock = app->Get_BLock();
}
void CNetworkThread::dispatch(void* param)
{
    if (!m_udpQueue || !m_udpHandler || !m_udpQLock)
        throw CDNFException("NetworkThread is Not Ready!\n");
    DNFFLib::Sleep_Ext(5, 0);
    puts("Network Thread Start!");
    m_stop = 1;
    try
    {
        while (m_stop)
        {
            CUdpRecvBuffer* buf;
            {
                CGuard<CMutex> guard(m_udpBLock);
                buf = new CUdpRecvBuffer;
            }
            int size = 0x1800;
            unsigned int addr = 0;
            unsigned short port = 0;
            if (((CUdpHandler*)m_udpHandler)->RecvFromClient(
                    (char*)buf, &size, &addr, &port) != 1)
            {
                {
                    CGuard<CMutex> guard(m_udpBLock);
                    delete buf;
                }
                continue;
            }
            if (((PacketHeader*)buf)->packetSize != size)
            {
                CMyFileLog log(__FUNCTION__, 0x6c);
                log("./log/recvErr",
                    "Packet Size is Incorrect! Packet Size( %d ), Recv Byte( %d ) Code( %d )\n",
                    ((PacketHeader*)buf)->packetSize, size,
                    ((PacketHeader*)buf)->packetId);
                {
                    CGuard<CMutex> guard(m_udpBLock);
                    delete buf;
                }
                continue;
            }
            if (((PacketHeader*)buf)->packetSize > 0x17ff)
            {
                CMyFileLog log(__FUNCTION__, 0x77);
                log("./log/recvErr",
                    "Packet Size is Over! Packet Size( %d ), Recv Byte( %d ) Code( %d )\n",
                    ((PacketHeader*)buf)->packetSize, size,
                    ((PacketHeader*)buf)->packetId);
                {
                    CGuard<CMutex> guard(m_udpBLock);
                    delete buf;
                }
                continue;
            }
            if (size > 0x1800)
            {
                CMyFileLog log(__FUNCTION__, 0x83);
                log("./log/recvErr",
                    "Recv Byte is Over! Packet Size( %d ), Recv Byte( %d ) Code( %d )\n",
                    ((PacketHeader*)buf)->packetSize, size,
                    ((PacketHeader*)buf)->packetId);
                {
                    CGuard<CMutex> guard(m_udpBLock);
                    delete buf;
                }
                continue;
            }
            {
                CGuard<CMutex> guard(m_udpQLock);
                m_udpQueue->push(buf);
            }
        }
    }
    catch (CDNFException& e)
    {
        printf("CNetworkThread::dispatch() Exception Break : %s\n", e.what());
        throw CDNFException("CNetworkThread::dispatch() Recv  Socket Exception Break!");
    }
    catch (...)
    {
        puts("CNetworkThread::dispatch() Exception Break");
        throw CDNFException("CNetworkThread::dispatch() Recv  Socket Exception Break!");
    }
}
void CNetworkThread::SetUDPQueue(UdpRecvQueue* q) { m_udpQueue = q; }
