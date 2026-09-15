// df_dbmw_r - TcpNetSystem (ORIG TcpNetSystem.cpp)
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

CTcpNetSystem::CTcpNetSystem() : m_tcpHandler(0), m_field4(0), m_acceptThread(0)
{
    m_serverPort = 0;
}
CTcpNetSystem::~CTcpNetSystem()
{
    CleanPeers();
    if (m_tcpHandler)
    {
        delete m_tcpHandler;
        m_tcpHandler = 0;
    }
    if (m_acceptThread)
    {
        m_acceptThread->stop();
        delete m_acceptThread;
        m_acceptThread = 0;
    }
    if (m_field4)
    {
        ((CTcpNetworkThread*)m_field4)->stop();
        delete (CTcpNetworkThread*)m_field4;
        m_field4 = 0;
    }
}
// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-DBM-DIFF-0076 已消除）
int CTcpNetSystem::OpenTcpService(int& serverCount, const char* ip, unsigned short port)
{
    CPeer* peer = CreatePeer();
    TCPSocket* sock = peer->GetTcpSocket();
    if (!sock->open())
    {
        puts("tcpSock.open() Fail!");
        CMyFileLog(__FUNCTION__, 0x118)("./log/TcpServer", "tcpSock.open() Fail!");
        DeletePeer(peer);
        return 0;
    }
    if (!sock->connect(ip, port))
    {
        puts("tcpSock.connect Fail!");
        CMyFileLog(__FUNCTION__, 0x123)("./log/TcpServer", "tcpSock.connect(%s, %d) Fail!", ip, port);
        DeletePeer(peer);
        return 0;
    }
    sock->setOptNonBlock();
    peer->InitPeer(Get_TcpSwapQPacket()->GetRecvQ(), Get_TcpRecvQLock(), Get_TcpRecvBLock());
    peer->ConnSig();
    SetEpollConnectedPeer(peer);
    serverCount = sock->getHandle();
    return 1;
}
void CTcpNetSystem::Init(unsigned short port)
{
    m_serverPort = port;
    m_tcpHandler = new CTcpHandler;
    m_acceptThread = new CTcpAcceptThread;
    ((CTcpAcceptThread*)m_acceptThread)->attach(this);
    if (!m_acceptThread->begin())
        throw;   // ORIG 失败路径 __cxa_rethrow（rethrow 当前异常），非 throw 1
    m_field4 = new CTcpNetworkThread;
    ((CTcpNetworkThread*)m_field4)->attach(this);
    if (!((CTcpNetworkThread*)m_field4)->begin())
        throw;   // ORIG 失败路径 __cxa_rethrow
}
void CTcpNetSystem::CleanPeers()
{
    for (std::map<unsigned int, CPeer*>::iterator it = m_peerMap.begin();
         it != m_peerMap.end(); )
    {
        CGuard<CMutex> guard(&m_mutex78);
        delete it->second;
        ++it;
    }
    m_peerMap.clear();
}
void CTcpNetSystem::DeletePeer(CPeer* peer)
{
    unsigned int fd = peer->GetTcpSocket()->getHandle();
    std::map<unsigned int, CPeer*>::iterator it = m_peerMap.find(fd);
    if (it != m_peerMap.end())
        m_peerMap.erase(it);
    CGuard<CMutex> guard(&m_mutex78);
    delete peer;
}
CPeer* CTcpNetSystem::GetPeer(unsigned int idx)
{
    std::map<unsigned int, CPeer*>::iterator it = m_peerMap.find(idx);
    if (it != m_peerMap.end())
        return it->second;
    return 0;
}
CPeer* CTcpNetSystem::CreatePeer()
{
    CGuard<CMutex> guard(&m_mutex78);
    return new CPeer;
}
void CTcpNetSystem::InsertAcceptedPeer(CPeer* peer)
{
    CGuard<CMutex> guard(&m_mutex60);
    m_peerQueue.push(peer);
}
void CTcpNetSystem::SetEpollConnectedPeer(CPeer* peer)
{
    CGuard<CMutex> guard(&m_mutex78);
    int ret = 0;
    if ((ret = m_tcpHandler->SetPeer(peer, peer->GetTcpSocket()->getHandle(), 0)) != 0)
    {
        printf("G_EpollHandler()->SetPeer(peer->get_socket(%d)) %d(%s)",
               peer->GetTcpSocket()->getHandle(), ret, strerror(ret));
    }
    m_peerMap.insert(std::make_pair(peer->GetTcpSocket()->getHandle(), peer));
}
// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-DBM-DIFF-0079 已消除）
void CTcpNetSystem::SetEpollAcceptedPeers()
{
    CGuard<CMutex> guard(&m_mutex60);
    if (m_peerQueue.empty())
        return;
    CPeer* peer = 0;
    while (!m_peerQueue.empty())
    {
        peer = m_peerQueue.front();
        int ret = 0;
        if ((ret = m_tcpHandler->SetPeer(peer, peer->GetTcpSocket()->getHandle(),
                                        false)) != 0)
        {
            printf("G_EpollHandler()->SetPeer(peer->get_socket(%d)) %d(%s)",
                   peer->GetTcpSocket()->getHandle(), ret, strerror(ret));
        }
        m_peerMap.insert(std::make_pair(peer->GetTcpSocket()->getHandle(), peer));
        m_peerQueue.pop();
    }
}
// [DNF-NONIDENTICAL] DNF-DBM-DIFF-0075 | dbmw | 与ORIG差异=DIFF | CTcpNetSystem::SendPacket | 详见 function_reports/dbmw/_ZN13CTcpNetSystem10SendPacketEv.md
// X2 移植自 guild identical 版：ORIG 反汇编实测 TcpSendBuffer 头部字段（id@0/size@2/pad@4/ip@6）
#pragma pack(push, 1)
struct TcpPacketFields
{
    unsigned short m_id;   // +0
    unsigned short m_size; // +2
    unsigned short m_pad;  // +4
    int m_ip;              // +6
};
#pragma pack(pop)

int CTcpNetSystem::SendPacket()
{
    {
        register int ret;
        register int flag;
        CTcpSendBuffer* buf;
        CTcpSendBuffer* b2;
        CPeer* peer;
        int result;
        int cnt;
        {
            CGuard<CMutex> guard(&m_mutexE8);
            if (m_sendQueue.empty())
            {
                ret = 0;
                flag = 0;
            }
            else
            {
                buf = m_sendQueue.front();
                flag = 1;
            }
        }
        if (flag)
        {
            if (!buf)
            {
                ret = 0;
                goto done;
            }
            b2 = buf;
            std::map<unsigned int, CPeer*>::iterator it =
                m_peerMap.find(*(unsigned int*)((char*)b2 + 6));
            if (it == m_peerMap.end())
            {
                DNF_LOG_SCOPE_LINE(0xba, "./log/TcpSend", "SEND ERR:no peer(id:%d,size:%d,ip:%d)",
                    ((TcpPacketFields*)b2)->m_id, ((TcpPacketFields*)b2)->m_size,
                    ((TcpPacketFields*)b2)->m_ip);
                PopDeleteTcpSendPacketQ(buf);
                ret = 0;
            }
            else
            {
                peer = it->second;
                if (peer == 0 ||
                    ((TcpPacketFields*)b2)->m_ip !=
                        peer->GetTcpSocket()->getHandle())
                {
                    DNF_LOG_SCOPE_LINE(0xc3, "./log/TcpSend",
                        "SEND ERR:invalid peer(%x)(id:%d)(size:%d)(ip:%d)", peer,
                        ((TcpPacketFields*)b2)->m_id, ((TcpPacketFields*)b2)->m_size,
                        ((TcpPacketFields*)b2)->m_ip);
                    PopDeleteTcpSendPacketQ(buf);
                    ret = 0;
                }
                else
                {
                    result = peer->send_packet((char*)b2,
                                               ((TcpPacketFields*)b2)->m_size);
                    if (result > 0)
                    {
                        PopDeleteTcpSendPacketQ(buf);
                    }
                    else
                    {
                        cnt = (int)m_sendQueue.size();
                        DNF_LOG_SCOPE_LINE(0xd5, "./log/TcpSend", "SEND(id:%d,size:%d,ip:%d, cnt:%d)",
                            ((TcpPacketFields*)b2)->m_id, ((TcpPacketFields*)b2)->m_size,
                            ((TcpPacketFields*)b2)->m_ip, cnt);
                    }
                    ret = result;
                }
            }
        }
    done:
        return ret;
    }
}
void CTcpNetSystem::PopDeleteTcpSendPacketQ(CTcpSendBuffer* buf)
{
    {
        CGuard<CMutex> guard(&m_mutexE8);
        m_sendQueue.pop();
    }
    {
        CGuard<CMutex> guard(&m_mutex100);
        delete buf;
    }
}
CTcpSendBuffer* CTcpNetSystem::Acquire_TcpSendBuffer()
{
    CGuard<CMutex> guard(&m_mutex100);
    return new CTcpSendBuffer;
}
// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-DBM-DIFF-0077 已消除）
void CTcpNetSystem::PushTcpSendPacketQ(char* buf)
{
    CGuard<CMutex> guard(&m_mutexE8);
    m_sendQueue.push((CTcpSendBuffer*)buf);
    int n = m_sendQueue.size();
    if (n > 0xa)
    {
        CMyFileLog(__FUNCTION__, 0x91)("./log/TcpSend", "SEND PUSH(cnt:%d,id:%d,size:%d,ip:%d)", n,
            ((CTcpSendBuffer*)buf)->m_header.packetId,
            ((CTcpSendBuffer*)buf)->m_header.packetSize,
            ((CTcpSendBuffer*)buf)->m_header.m_connNo);
    }
}
// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-DBM-DIFF-0078 已消除）
void CTcpNetSystem::CleanTcpSendPacketQ()
{
    for (;;)
    {
        CTcpSendBuffer* p;
        register int keep;
        {
            CGuard<CMutex> guard(&m_mutexE8);
            if (m_sendQueue.empty())
            {
                keep = 0;
            }
            else
            {
                p = m_sendQueue.front();
                m_sendQueue.pop();
                keep = 1;
            }
        }
        if (!keep)
            break;
        {
            CGuard<CMutex> guard(&m_mutex100);
            delete p;
        }
    }
    CMyFileLog(__FUNCTION__, 0x16b)("./log/TcpSend", "Clean Tcp Send Queue Complete !");
}
int CTcpNetSystem::WaitForEvent()
{
    return m_tcpHandler->WaitForEvent();
}
unsigned short CTcpNetSystem::Get_TcpServerPort() { return m_serverPort; }
CTcpHandler* CTcpNetSystem::Get_TcpHandler() { return m_tcpHandler; }
void* CTcpNetSystem::Get_TcpRecvQPacket() { return &m_recvSwapQueue; }
CMutex* CTcpNetSystem::Get_TcpRecvQLock() { return &m_mutex90; }
CMutex* CTcpNetSystem::Get_TcpRecvBLock() { return &m_mutexA8; }
CMutex* CTcpNetSystem::Get_TcpSendQLock() { return &m_mutexE8; }
CMutex* CTcpNetSystem::Get_TcpSendBLock() { return &m_mutex100; }
CTcpSendQueue* CTcpNetSystem::Get_TcpSendQPacket() { return &m_sendQueue; }
