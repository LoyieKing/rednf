// df_manager_r — CUdpNetworkThread（ORIG DNFNetworkThread.cpp）
#include "DNFNetworkThread.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>

#include "DNFApplication.h"
#include "DNFTableBase.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "DNFUdpHandler.h"
#include "Thread.h"

CUdpNetworkThread::CUdpNetworkThread()
{
    m_udpQueue = 0;
    m_udpHandler = 0;
    m_udpQLock = 0;
    m_udpBLock = 0;
}
CUdpNetworkThread::~CUdpNetworkThread()
{
    m_udpQueue = 0;
    m_udpHandler = 0;
    m_udpQLock = 0;
}

void CUdpNetworkThread::attach(CApplication* app)
{
    if (app)
    {
        m_udpQueue = app->Get_UdpPacketRecvQ();
        m_udpHandler = app->Get_UdpHandler();
        m_udpQLock = app->Get_UdpQLock();
        m_udpBLock = app->Get_UdpBLock();
    }
}

void* CUdpNetworkThread::dispatch(void* param)
{
    try
    {
        if (!m_udpQueue || !m_udpHandler || !m_udpQLock)
            throw CDNFException("NetworkThread is Not Ready!\n");
        DNFFLib::Sleep_Ext(5, 0);
        puts("Network Thread Start!");
        m_stop = 1;
        // ORIG：FD_SET/FD_ISSET 中 sock/32 为无符号 shr（sock 为 unsigned）。
        unsigned int sock = ((CUdpHandler*)m_udpHandler)->GetServerSocket();
        int flags = fcntl(sock, F_GETFL, 0);
        flags |= O_NONBLOCK;
        if (fcntl(sock, F_SETFL, flags) < 0)
        {
            puts("fcntl error!");
            return 0;
        }
        while (m_stop)
        {
            fd_set readfds;
            // ORIG：FD_ZERO 展开为 32 次循环清零（老 glibc 宏形态；
            // 4.4.7 头是 memset/rep stos，需显式循环复现）。
            for (int i = 0; i < 0x20; i++)
                readfds.fds_bits[i] = 0;
            FD_SET(sock, &readfds);
            struct timeval tv;
            tv.tv_sec = 1;
            tv.tv_usec = 0;
            if (select(sock + 1, &readfds, 0, 0, &tv) < 0)
                continue;
            if (!FD_ISSET(sock, &readfds))
                continue;
            CUdpRecvBuffer* buf;
            {
                CGuard<CMutex> guard(m_udpBLock);
                buf = new CUdpRecvBuffer;
            }
            int size = 0x1800;
            unsigned int addr = 0;
            unsigned short port = 0;
            if (!((CUdpHandler*)m_udpHandler)->RecvFromClient((char*)buf, &size, &addr, &port))
            {
                CGuard<CMutex> guard(m_udpBLock);
                delete buf;
                continue;
            }
            unsigned short code = *(unsigned short*)((char*)buf + 2);
            if (code != (unsigned short)size)
            {
                CMyFileLog log(__FUNCTION__, 0xb5);
                log("./log/recvErr",
                    "Packet Size is Incorrect! Packet Size( %d ), Recv Byte( %d ) Code( %d )\n",
                    *(unsigned short*)buf, size, code);
                {
                    CGuard<CMutex> guard(m_udpBLock);
                    delete buf;
                }
                continue;
            }
            if (code > 0x17ff)
            {
                CMyFileLog log(__FUNCTION__, 0xc0);
                log("./log/recvErr",
                    "Packet Size is Over! Packet Size( %d ), Recv Byte( %d ) Code( %d )\n",
                    *(unsigned short*)buf, size, code);
                {
                    CGuard<CMutex> guard(m_udpBLock);
                    delete buf;
                }
                continue;
            }
            if (size > 0x1800)
            {
                CMyFileLog log(__FUNCTION__, 0xcc);
                log("./log/recvErr",
                    "Recv Byte is Over! Packet Size( %d ), Recv Byte( %d ) Code( %d )\n",
                    *(unsigned short*)buf, size, code);
                {
                    CGuard<CMutex> guard(m_udpBLock);
                    delete buf;
                }
                continue;
            }
            *(unsigned int*)((char*)buf + 6) = addr;
            *(unsigned short*)((char*)buf + 4) = port;
            {
                CGuard<CMutex> guard(m_udpQLock);
                m_udpQueue->push(buf);
                if (m_udpQueue->size() > 0x64)
                {
                    CMyFileLog log(__FUNCTION__, 0xe0);
                    log("./log/recv",
                        "buffer(%d) ,id(%d), size(%d) \n",
                        m_udpQueue->size(), *(unsigned short*)buf, code);
                }
            }
        }
        return 0;
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

void CUdpNetworkThread::SetUDPQueue(UdpRecvQueue* q) { m_udpQueue = q; }
