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
int CTcpNetSystem::OpenTcpService(int& serverCount, const char* ip, unsigned short port)
{
    CPeer* peer = CreatePeer();
    TCPSocket* sock = peer->GetTcpSocket();
    if (!sock->open())
    {
        puts("Tcp Open Socket Err");
        CMyFileLog log(__FUNCTION__, 0x118);
        log("./log/TcpConnect", "Tcp Open Socket Err");
        DeletePeer(peer);
        return 0;
    }
    if (!sock->connect(ip, port))
    {
        puts("Tcp Connect Err");
        CMyFileLog log(__FUNCTION__, 0x123);
        log("./log/TcpConnect", "Tcp Connect Err(ip:%s, port:%d)", ip, port);
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
    m_acceptThread->attach(this);
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
    int fd = peer->GetTcpSocket()->getHandle();
    int ret = m_tcpHandler->SetPeer(peer, fd, 0);
    if (ret != 0)
    {
        printf("G_EpollHandler()->SetPeer(peer->get_socket(%d)) %d(%s)",
               peer->GetTcpSocket()->getHandle(), ret, strerror(ret));
    }
    m_peerMap.insert(std::make_pair(peer->GetTcpSocket()->getHandle(), peer));
}
void CTcpNetSystem::SetEpollAcceptedPeers()
{
    CGuard<CMutex> guard(&m_mutex60);
    while (!m_peerQueue.empty())
    {
        CPeer* peer = m_peerQueue.front();
        int ret = m_tcpHandler->SetPeer(peer, peer->GetTcpSocket()->getHandle(), false);
        if (ret != 0)
        {
            printf("G_EpollHandler()->SetPeer(peer->get_socket(%d)) %d(%s)",
                   peer->GetTcpSocket()->getHandle(), ret, strerror(ret));
        }
        int fd = peer->GetTcpSocket()->getHandle();
        m_peerMap.insert(std::make_pair(fd, peer));
        m_peerQueue.pop();
    }
}
int CTcpNetSystem::SendPacket()
{
    CTcpSendBuffer* buf;
    register int flag;
    register int result;
    {
        CGuard<CMutex> guard(&m_mutexE8);
        if (m_sendQueue.empty())
        {
            result = 0;
            flag = 0;
        }
        else
        {
            buf = m_sendQueue.front();
            flag = 1;
        }
    }
    if (flag && buf != NULL)
    {
        std::map<unsigned int, CPeer*>::iterator it =
            m_peerMap.find(*(unsigned int*)((char*)buf + 6));
        if (it == m_peerMap.end())
        {
            CMyFileLog log(__FUNCTION__, 0xba);
            log("./log/TcpSend", "SEND ERR:no peer(id:%d,size:%d,ip:%d)",
                ((unsigned short*)buf)[0], ((unsigned short*)buf)[1],
                *(unsigned int*)((char*)buf + 6));
            PopDeleteTcpSendPacketQ(buf);
            result = 0;
        }
        else
        {
            CPeer* peer = it->second;
            bool bad = true;
            if (peer != NULL &&
                *(unsigned int*)((char*)buf + 6) ==
                    peer->GetTcpSocket()->getHandle())
                bad = false;
            if (bad)
            {
                CMyFileLog log(__FUNCTION__, 0xc3);
                log("./log/TcpSend",
                    "SEND ERR:invalid peer(%x)(id:%d)(size:%d)(ip:%d)", peer,
                    ((unsigned short*)buf)[0], ((unsigned short*)buf)[1],
                    *(unsigned int*)((char*)buf + 6));
                PopDeleteTcpSendPacketQ(buf);
                result = 0;
            }
            else
            {
                result = peer->send_packet((char*)buf,
                                           ((unsigned short*)buf)[1]);
                if (result < 1)
                {
                    CMyFileLog log(__FUNCTION__, 0xd5);
                    log("./log/TcpSend", "SEND(id:%d,size:%d,ip:%d, cnt:%d)",
                        ((unsigned short*)buf)[0], ((unsigned short*)buf)[1],
                        *(unsigned int*)((char*)buf + 6),
                        m_sendQueue.size());
                }
                else
                {
                    PopDeleteTcpSendPacketQ(buf);
                }
            }
        }
    }
    return result;
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
void CTcpNetSystem::PushTcpSendPacketQ(char* buf)
{
    CGuard<CMutex> guard(&m_mutexE8);
    m_sendQueue.push((CTcpSendBuffer*)buf);
    int n = m_sendQueue.size();
    if (n > 0xa)
    {
        CMyFileLog log(__FUNCTION__, 0x91);
        log("./log/TcpSend", "SEND PUSH(cnt:%d,id:%d,size:%d,ip:%d)", n,
            (unsigned short)buf[0], (unsigned short)((unsigned short*)buf)[1],
            ((char*)buf)[6]);
    }
}
void CTcpNetSystem::CleanTcpSendPacketQ()
{
    CGuard<CMutex> guard(&m_mutexE8);
    while (!m_sendQueue.empty())
    {
        CTcpSendBuffer* p = m_sendQueue.front();
        m_sendQueue.pop();
        {
            CGuard<CMutex> guard(&m_mutex100);
            delete p;
        }
    }
    CMyFileLog log(__FUNCTION__, 0x16b);
    log("./log/TcpSend", "Clean Tcp Send Queue Complete !");
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
