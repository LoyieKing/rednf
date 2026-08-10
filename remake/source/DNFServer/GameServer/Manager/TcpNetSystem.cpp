// df_manager_r — CTcpNetSystem（ORIG TcpNetSystem.cpp）
#include "TcpNetSystem.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "DNFTcpAcceptThread.h"
#include "DNFTcpHandler.h"
#include "DNFTcpNetworkThread.h"
#include "Peer.h"

CTcpNetSystem::CTcpNetSystem()
{
    m_tcpHandler = 0;
    m_field4 = 0;
    m_acceptThread = 0;
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
        delete m_acceptThread;
        m_acceptThread = 0;
    }
    if (m_field4)
    {
        delete (CTcpNetworkThread*)m_field4;
        m_field4 = 0;
    }
}

void CTcpNetSystem::Init(unsigned short port)
{
    m_serverPort = port;
    m_tcpHandler = new CTcpHandler;
    m_acceptThread = new CTcpAcceptThread;
    m_acceptThread->attach(this);
    if (!m_acceptThread->CThreadInterface::begin())
        throw;
    m_field4 = new CTcpNetworkThread;
    ((CTcpNetworkThread*)m_field4)->attach(this);
    if (!((CTcpNetworkThread*)m_field4)->CThreadInterface::begin())
        throw;
}

int CTcpNetSystem::OpenTcpService(int& serverCount, const char* ip, unsigned short port)
{
    CPeer* peer = CreatePeer();
    TCPSocket* sock = peer->GetTcpSocket();
    if (!sock->open())
    {
        puts("Tcp Open Socket Err");
        CMyFileLog log("OpenTcpService", 0x118);
        log("./log/TcpConnect", "Tcp Open Socket Err");
        DeletePeer(peer);
        return 0;
    }
    if (!sock->connect(ip, port))
    {
        puts("Tcp Connect Err");
        CMyFileLog log("OpenTcpService", 0x123);
        log("./log/TcpConnect", "Tcp Connect Err(ip:%s, port:%d)", ip, port);
        DeletePeer(peer);
        return 0;
    }
    sock->setOptNonBlock();
    peer->InitPeer(m_recvSwapQueue.GetRecvQ(), Get_TcpRecvQLock(), Get_TcpRecvBLock());
    peer->ConnSig();
    SetEpollConnectedPeer(peer);
    serverCount = sock->getHandle();
    return 1;
}

int CTcpNetSystem::WaitForEvent()
{
    return m_tcpHandler->WaitForEvent();
}

void CTcpNetSystem::PushTcpSendPacketQ(char* buf)
{
    CGuard<CMutex> guard(&m_mutexE8);
    m_sendQueue.push((CTcpSendBuffer*)buf);
    int n = m_sendQueue.size();
    if (n > 0xa)
    {
        CMyFileLog log("PushTcpSendPacketQ", 0x91);
        log("./log/TcpSend", "SEND PUSH(cnt:%d,id:%d,size:%d,ip:%d)", n,
            (unsigned short)buf[0], (unsigned short)((unsigned short*)buf)[1],
            ((char*)buf)[6]);
    }
}

void CTcpNetSystem::CleanTcpSendPacketQ()
{
    while (true)
    {
        CGuard<CMutex> guard(&m_mutexE8);
        if (m_sendQueue.empty())
            break;
        CTcpSendBuffer* p = m_sendQueue.front();
        m_sendQueue.pop();
        CGuard<CMutex> guard2(&m_mutex100);
        delete p;
    }
    CMyFileLog log("CleanTcpSendPacketQ", 0x16b);
    log("./log/TcpSend", "Clean Tcp Send Queue Complete!");
}

void CTcpNetSystem::CleanPeers()
{
    for (std::map<unsigned int, CPeer*>::iterator it = m_peerMap.begin();
         it != m_peerMap.end(); ++it)
    {
        CGuard<CMutex> guard(&m_mutex78);
        CPeer* peer = it->second;
        if (peer)
            delete peer;
    }
    m_peerMap.clear();
}

void CTcpNetSystem::DeletePeer(CPeer* peer)
{
    int fd = peer->GetTcpSocket()->getHandle();
    std::map<unsigned int, CPeer*>::iterator it = m_peerMap.find(fd);
    if (it != m_peerMap.end())
        m_peerMap.erase(it);
    CGuard<CMutex> guard(&m_mutex78);
    delete peer;
}
CPeer* CTcpNetSystem::GetPeer(unsigned int idx)
{
    std::map<unsigned int, CPeer*>::iterator it = m_peerMap.find(idx);
    if (it == m_peerMap.end())
        return 0;
    return it->second;
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
        printf("Epoll SetPeer fail(fd:%d, error:%d, %s)", fd, ret, strerror(ret));
        return;
    }
    m_peerMap.insert(std::make_pair((int)fd, peer));
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
        m_peerMap.insert(std::make_pair((int)fd, peer));
        m_peerQueue.pop();
    }
}
void CTcpNetSystem::SendPacket()
{
    CGuard<CMutex> guard(&m_mutexE8);
    if (m_sendQueue.empty())
        return;
    CTcpSendBuffer* buf = m_sendQueue.front();
    if (!buf)
        return;
    int port = *(int*)((char*)buf + 6);
    std::map<unsigned int, CPeer*>::iterator it = m_peerMap.find(port);
    if (it == m_peerMap.end())
    {
        CMyFileLog log("CTcpNetSystem::SendPacket", 0xba);
        log("./log/TcpSend", "SEND FAIL(port:%d,id:%d,size:%d)",
            port, *(unsigned short*)((char*)buf),
            *(unsigned short*)((char*)buf + 2));
        PopDeleteTcpSendPacketQ(buf);
        return;
    }
    CPeer* peer = it->second;
    if (!peer)
    {
        CMyFileLog log("CTcpNetSystem::SendPacket", 0xba);
        log("./log/TcpSend", "SEND FAIL(port:%d,id:%d,size:%d)",
            port, *(unsigned short*)((char*)buf),
            *(unsigned short*)((char*)buf + 2));
        PopDeleteTcpSendPacketQ(buf);
        return;
    }
    if (peer->GetTcpSocket()->getHandle() == port)
    {
        CMyFileLog log("CTcpNetSystem::SendPacket", 0xc3);
        log("./log/TcpSend", "SEND FAIL(peer:%p, id:%d, size:%d, port:%d)",
            peer, *(unsigned short*)((char*)buf),
            *(unsigned short*)((char*)buf + 2), port);
        PopDeleteTcpSendPacketQ(buf);
        return;
    }
    int ret = peer->send_packet((char*)buf, *(unsigned short*)((char*)buf + 2));
    if (ret > 0)
    {
        PopDeleteTcpSendPacketQ(buf);
    }
    else
    {
        CMyFileLog log("CTcpNetSystem::SendPacket", 0xd5);
        log("./log/TcpSend", "SEND QUEUE(%d, id:%d, size:%d, port:%d)",
            (int)m_sendQueue.size(), *(unsigned short*)((char*)buf),
            *(unsigned short*)((char*)buf + 2), port);
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

unsigned short CTcpNetSystem::Get_TcpServerPort() { return m_serverPort; }
CTcpHandler* CTcpNetSystem::Get_TcpHandler() { return m_tcpHandler; }
CSwapQueue<TcpRecvQueue, 2>* CTcpNetSystem::Get_TcpSwapQPacket() { return &m_recvSwapQueue; }
void* CTcpNetSystem::Get_TcpRecvQPacket() { return &m_recvSwapQueue; }
CMutex* CTcpNetSystem::Get_TcpRecvQLock() { return &m_mutex90; }
CMutex* CTcpNetSystem::Get_TcpRecvBLock() { return &m_mutexA8; }
CMutex* CTcpNetSystem::Get_TcpSendQLock() { return &m_mutexE8; }
CMutex* CTcpNetSystem::Get_TcpSendBLock() { return &m_mutex100; }
CTcpSendQueue* CTcpNetSystem::Get_TcpSendQPacket() { return &m_sendQueue; }
