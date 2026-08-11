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

void CTcpNetSystem::Init(unsigned short port)
{
    m_serverPort = port;
    m_tcpHandler = new CTcpHandler;
    m_acceptThread = new CTcpAcceptThread;  // R10: 成员为基类指针，隐式转换复现 ORIG 双 mov
    ((CTcpAcceptThread*)m_acceptThread)->attach(this);
    if (!((CTcpAcceptThread*)m_acceptThread)->CThreadInterface::begin())
        throw;
    m_field4 = new CTcpNetworkThread;  // R10: 成员为基类指针，隐式转换复现 ORIG 双 mov
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
        puts("tcpSock.open() Fail!");
        CMyFileLog log(__FUNCTION__, 0x118);
        log("./log/TcpServer", "tcpSock.open() Fail!");
        DeletePeer(peer);
        return 0;
    }
    else if (!sock->connect(ip, port))  // R10: else-if 结构变体
    {
        // ORIG 实测：puts 文案 "tcpSock.connect Fail!"，日志文案
        // "tcpSock.connect(%s, %d) Fail!"。
        puts("tcpSock.connect Fail!");
        register int nPort = port;  // R10: register 局部变体（对齐 ORIG ctor 前 ebx 预装载）
        CMyFileLog log(__FUNCTION__, 0x123);
        log("./log/TcpServer", "tcpSock.connect(%s, %d) Fail!", ip, nPort);
        DeletePeer(peer);
        return 0;
    }
    else
    {
        sock->setOptNonBlock();
        peer->InitPeer(Get_TcpSwapQPacket()->GetRecvQ(), Get_TcpRecvQLock(), Get_TcpRecvBLock());
        peer->ConnSig();
        SetEpollConnectedPeer(peer);
        serverCount = sock->getHandle();
        return 1;
    }
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
        // ORIG：三个字段在 CMyFileLog ctor 前预装载到 edi/esi/ebx（register 形态）
        register int ip = *(int*)((char*)buf + 6);
        register unsigned short size = *(unsigned short*)((char*)buf + 2);
        register unsigned short id = *(unsigned short*)((char*)buf);
        CMyFileLog log(__FUNCTION__, 0x91);
        log("./log/TcpSend", "SEND PUSH(cnt:%d,id:%d,size:%d,ip:%d)", n, id, size, ip);
    }
}

void CTcpNetSystem::CleanTcpSendPacketQ()
{
    while (true)
    {
        CTcpSendBuffer* p;
        {
            CGuard<CMutex> guard(&m_mutexE8);
            if (m_sendQueue.empty())
                break;
            p = m_sendQueue.front();
            m_sendQueue.pop();
        }
        {
            CGuard<CMutex> guard2(&m_mutex100);
            delete p;
        }
    }
    CMyFileLog log(__FUNCTION__, 0x16b);
    // ORIG 实测：字符串含空格 "Complete !"。
    log("./log/TcpSend", "Clean Tcp Send Queue Complete !");
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
    m_peerMap.insert(std::make_pair((int)peer->GetTcpSocket()->getHandle(), peer));
}
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
        if ((ret = m_tcpHandler->SetPeer(peer, peer->GetTcpSocket()->getHandle(), false)) != 0)
        {
            printf("G_EpollHandler()->SetPeer(peer->get_socket(%d)) %d(%s)",
                   peer->GetTcpSocket()->getHandle(), ret, strerror(ret));
        }
        m_peerMap.insert(std::make_pair((int)peer->GetTcpSocket()->getHandle(), peer));
        m_peerQueue.pop();
    }
}
int CTcpNetSystem::SendPacket()
{
    CGuard<CMutex> guard(&m_mutexE8);
    if (m_sendQueue.empty())
        return 0;
    CTcpSendBuffer* buf = m_sendQueue.front();
    if (!buf)
        return 0;
    std::map<unsigned int, CPeer*>::iterator it =
        m_peerMap.find(*(unsigned int*)((char*)buf + 6));
    if (it == m_peerMap.end())
    {
        register int ip = *(int*)((char*)buf + 6);
        register unsigned short size = *(unsigned short*)((char*)buf + 2);
        register unsigned short id = *(unsigned short*)((char*)buf);
        CMyFileLog log(__FUNCTION__, 0xba);
        log("./log/TcpSend", "SEND ERR:no peer(id:%d,size:%d,ip:%d)",
            id, size, ip);
        PopDeleteTcpSendPacketQ(buf);
        return 0;
    }
    CPeer* peer = it->second;
    if (peer == 0 || peer->GetTcpSocket()->getHandle() != *(int*)((char*)buf + 6))
    {
        register int ip = *(int*)((char*)buf + 6);
        register unsigned short size = *(unsigned short*)((char*)buf + 2);
        register unsigned short id = *(unsigned short*)((char*)buf);
        CMyFileLog log(__FUNCTION__, 0xc3);
        log("./log/TcpSend", "SEND ERR:invalid peer(%x)(id:%d)(size:%d)(ip:%d)",
            peer, id, size, ip);
        PopDeleteTcpSendPacketQ(buf);
        return 0;
    }
    int ret = peer->send_packet((char*)buf, *(unsigned short*)((char*)buf + 2));
    if (ret > 0)
    {
        PopDeleteTcpSendPacketQ(buf);
    }
    else
    {
        register int cnt = (int)m_sendQueue.size();
        register int ip = *(int*)((char*)buf + 6);
        register unsigned short size = *(unsigned short*)((char*)buf + 2);
        register unsigned short id = *(unsigned short*)((char*)buf);
        CMyFileLog log(__FUNCTION__, 0xd5);
        log("./log/TcpSend", "SEND(id:%d,size:%d,ip:%d, cnt:%d)",
            cnt, id, size, ip);
    }
    return ret;
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
