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
        int sock = ((CUdpHandler*)m_udpHandler)->GetServerSocket();
        int ret;
        int flags = fcntl(sock, F_GETFL, 0);
        ret = fcntl(sock, F_SETFL, flags | O_NONBLOCK);
        if (ret < 0)
            puts("fcntl error!");
        else
        {
            while (m_stop)
            {
                fd_set readfds;
                CUdpRecvBuffer* buf;
                int size;
                unsigned short port;
                unsigned int addr;
                // ORIG：FD_ZERO 展开为 32 次循环清零（老 glibc 宏形态：
                // 指针局部 + 无符号索引；4.4.7 头是 rep stos，需显式复现）。
                {
                    unsigned int i;
                    fd_set* pfd = &readfds;
                    for (i = 0; i < 0x20; i++)
                        pfd->fds_bits[i] = 0;
                }
                struct timeval tv;
                tv.tv_sec = 1;
                tv.tv_usec = 0;
                FD_SET((unsigned int)sock, &readfds);
                int nfds = sock + 1;
                ret = select(nfds, &readfds, 0, 0, &tv);
                if (ret < 0)
                    continue;
                if (!FD_ISSET((unsigned int)sock, &readfds))
                    continue;
                {
                    CGuard<CMutex> guard(m_udpBLock);
                    buf = new CUdpRecvBuffer;
                }
                size = 0x1800;
                port = 0;
                addr = 0;
                if (!((CUdpHandler*)m_udpHandler)->RecvFromClient((char*)buf, &size, &addr, &port))
                {
                    CGuard<CMutex> guard(m_udpBLock);
                    delete buf;
                    continue;
                }
                CUdpRecvBuffer* pBuf = buf;
                if (pBuf->m_size != size)
                {
                    DNF_LOG_SCOPE_LINE(0xb5, "./log/recvErr",
                        "Packet Size is Incorrect! Packet Size( %d ), Recv Byte( %d ) Code( %d )\n",
                        pBuf->m_size, size, pBuf->m_id);
                    {
                        CGuard<CMutex> guard(m_udpBLock);
                        delete buf;
                    }
                    continue;
                }
                if (pBuf->m_size > 0x17ff)
                {
                    DNF_LOG_SCOPE_LINE(0xc0, "./log/recvErr",
                        "Packet Size is Over! Packet Size( %d ), Recv Byte( %d ) Code( %d )\n",
                        pBuf->m_size, size, pBuf->m_id);
                    {
                        CGuard<CMutex> guard(m_udpBLock);
                        delete buf;
                    }
                    continue;
                }
                if ((unsigned int)size > 0x1800)
                {
                    DNF_LOG_SCOPE_LINE(0xcc, "./log/recvErr",
                        "Recv Byte is Over! Packet Size( %d ), Recv Byte( %d ) Code( %d )\n",
                        pBuf->m_size, size, pBuf->m_id);
                    {
                        CGuard<CMutex> guard(m_udpBLock);
                        delete buf;
                    }
                    continue;
                }
                pBuf->m_addr = (int)addr;
                pBuf->m_port = port;
                {
                    CGuard<CMutex> guard(m_udpQLock);
                    m_udpQueue->push(buf);
                    if (m_udpQueue->size() > 0x64)
                    {
                        DNF_LOG_SCOPE_LINE(0xe0, "./log/recv",
                            "buffer(%d) ,id(%d), size(%d) \n",
                            m_udpQueue->size(), pBuf->m_id, pBuf->m_size);
                    }
                }
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

void CUdpNetworkThread::SetUDPQueue(UdpRecvQueue* q) { m_udpQueue = q; }
