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
        DNF_LOG_SCOPE_LINE(0x118, "./log/TcpServer", "tcpSock.open() Fail!");
        DeletePeer(peer);
        return 0;
    }
    else if (!sock->connect(ip, port))
    {
        // ORIG 实测：puts 文案 "tcpSock.connect Fail!"，日志文案
        // "tcpSock.connect(%s, %d) Fail!"。
        puts("tcpSock.connect Fail!");
        register int nPort = port;  // R10: register 局部变体（对齐 ORIG ctor 前 ebx 预装载）
        DNF_LOG_SCOPE_LINE(0x123, "./log/TcpServer", "tcpSock.connect(%s, %d) Fail!", ip, nPort);
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
        // ORIG 形态：临时对象宏（实参在 ctor 前从右到左求值入 edi/esi/ebx）
        DNF_LOG_SCOPE_LINE(0x91, "./log/TcpSend", "SEND PUSH(cnt:%d,id:%d,size:%d,ip:%d)",
            n, ((CTcpSendBuffer*)buf)->m_id, ((CTcpSendBuffer*)buf)->m_size,
            ((CTcpSendBuffer*)buf)->m_ip);
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
    // R14：ORIG 形态——queue 访问在 guard 内层作用域，flag(esi)/ret(ebx) 寄存器变量，
    // buf(-0x2c) 用于 PopDelete、b2(-0x28) 副本用于字段访问。
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
        {
            if (!buf)
            {
                ret = 0;
                goto done;
            }
            b2 = buf;
            std::map<unsigned int, CPeer*>::iterator it = m_peerMap.find(*(unsigned int*)((char*)b2 + 6));
            if (it == m_peerMap.end())
            {
                DNF_LOG_SCOPE_LINE(0xba, "./log/TcpSend", "SEND ERR:no peer(id:%d,size:%d,ip:%d)",
                    b2->m_id, b2->m_size, b2->m_ip);
                PopDeleteTcpSendPacketQ(buf);
                ret = 0;
            }
            else
            {
                peer = it->second;
                if (peer == 0 || b2->m_ip != peer->GetTcpSocket()->getHandle())
                {
                    DNF_LOG_SCOPE_LINE(0xc3, "./log/TcpSend", "SEND ERR:invalid peer(%x)(id:%d)(size:%d)(ip:%d)",
                        peer, b2->m_id, b2->m_size, b2->m_ip);
                    PopDeleteTcpSendPacketQ(buf);
                    ret = 0;
                }
                else
                {
                    result = peer->send_packet((char*)b2, b2->m_size);
                    if (result > 0)
                    {
                        PopDeleteTcpSendPacketQ(buf);
                    }
                    else
                    {
                        cnt = (int)m_sendQueue.size();
                        DNF_LOG_SCOPE_LINE(0xd5, "./log/TcpSend", "SEND(id:%d,size:%d,ip:%d, cnt:%d)",
                            b2->m_id, b2->m_size, b2->m_ip, cnt);
                    }
                    ret = result;
                }
            }
        }
    }
done:
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
