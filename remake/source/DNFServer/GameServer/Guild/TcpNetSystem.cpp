// df_guild_r — TcpNetSystem (split from source/guild per ORIG layout)
#include <string.h>
#include <time.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>
#include "tinyxml.h"
#include "Packet_Monitor_Call_Guild_Members_ToChannel.h"
#include "Packet_Monitor_Call_Guild_Members_ToChannel_Next.h"
#include "Packet_Monitor_Call_Guild_All_Members_ToChannel.h"
#include "Packet_Monitor_Call_Guild_All_Members_ToChannel_Next.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include <stdio.h>
#include <sys/times.h>

#include "TcpNetSystem.h"
#include "BlackUser.h"
#include "CashObject.h"
#include "DNFAppConfig.h"
#include "DNFAppStartInit.h"
#include "DNFAppStopInit.h"
#include "DNFApplication.h"
#include "DNFDBServer.h"
#include "DNFFloatingPointExceptSig.h"
#include "DNFGameServer.h"
#include "DNFGuild.h"
#include "DNFGuildManager.h"
#include "DNFGuildServerMain.h"
#include "DNFGuildWar.h"
#include "DNFInnerMsgHandler.h"
#include "DNFKillUserConfig.h"
#include "DNFManagerServer.h"
#include "DNFMonitorServer.h"
#include "DNFNetworkThread.h"
#include "DNFPacketBuffer.h"
#include "DNFPacketDecoder.h"
#include "DNFPacketTracer.h"
#include "DNFPacketTranslater.h"
#include "DNFPowerWarConfig.h"
#include "DNFSegmentationFaultSig.h"
#include "DNFServerConfig.h"
#include "DNFServerHandler.h"
#include "DNFServerInterface.h"
#include "DNFSignal.h"
#include "DNFSignalTranslator.h"
#include "DNFSystemFailSig.h"
#include "DNFTableBase.h"
#include "DNFTcpAcceptThread.h"
#include "DNFTcpHandler.h"
#include "DNFTcpNetworkThread.h"
#include "DNFTcpSocket.h"
#include "DNFTerminateSig.h"
#include "DNFThreadInterface.h"
#include "DNFTickHandler.h"
#include "DNFUdpHandler.h"
#include "DNFUser.h"
#include "DNFUser1Sig.h"
#include "DNFUserManager.h"
#include "DNFVersion.h"
#include "GMAccounts.h"
#include "GuildBoard.h"
#include "GuildCargo.h"
#include "MemPool.h"
#include "MemoryCashManager.h"
#include "PacketCounter.h"
#include "Peer.h"
#include "Power.h"
#include "PowerManager.h"
#include "PowerWar.h"
#include "PowerWarCharacInfo.h"
#include "PowerWarGuildInfo.h"
#include "Scheduler.h"
#include "ServerLoadChecker.h"
#include "ServerXml.h"
#include "SystemTimeHandler.h"
#include "WebEvent.h"

void CTcpNetSystem::Process()
{
}

// ORIG 反汇编实测 TcpSendBuffer 头部字段（id@0/size@2/pad@4/ip@6），
// 与 Manager DNFPacketBuffer.h 的 CTcpSendBuffer 布局一致；仅本 TU 使用，
// 避免改动共享 DNFPacketBuffer.h 影响其它 TU 的 m_data 基址。
#pragma pack(push, 1)
struct TcpPacketFields
{
    unsigned short m_id;   // +0
    unsigned short m_size; // +2
    unsigned short m_pad;  // +4
    int m_ip;              // +6
};
#pragma pack(pop)

CSwapQueueBase::CSwapQueueBase()
{
    memset(m_data, 0, sizeof(m_data));
}

CSwapQueueBase::~CSwapQueueBase()
{
}

template<class T, int N>
CSwapQueue<T, N>::CSwapQueue()
{
    for (int i = 0; i < N; i++)
    {
        new (m_data + 8 + i * 0x28) T();
    }
    Init();
}

template<class T, int N>
CSwapQueue<T, N>::~CSwapQueue()
{
}

template<class T, int N>
void CSwapQueue<T, N>::Push(T* item)
{
}

template<class T, int N>
void CSwapQueue<T, N>::Pop()
{
}

template<class T, int N>
void CSwapQueue<T, N>::Init()
{
    *(unsigned int*)(m_data + 0) = 0;
    *(unsigned int*)(m_data + 4) = 1;
}

template<class T, int N>
T* CSwapQueue<T, N>::GetRecvQ()
{
    return (T*)(m_data + 8 + *(unsigned int*)(m_data + 0) * 0x28);
}

template<class T, int N>
T* CSwapQueue<T, N>::GetParseQ()
{
    return (T*)(m_data + 8 + *(unsigned int*)(m_data + 4) * 0x28);
}

template<class T, int N>
void CSwapQueue<T, N>::SwapQ()
{
    unsigned int tmp = *(unsigned int*)(m_data + 0);
    *(unsigned int*)(m_data + 0) = *(unsigned int*)(m_data + 4);
    *(unsigned int*)(m_data + 4) = tmp;
}

template<class T>
IQueue<T>::IQueue()
{
    memset(m_data, 0, sizeof(m_data));
}

template<class T>
IQueue<T>::~IQueue()
{
}

template<class T>
IQueue<T>* IQueue<T>::Get()
{
    static IQueue<T> instance;
    return &instance;
}

template<class T>
void IQueue<T>::InitQueue(T* recv, T* parse)
{
    *(T**)(m_data + 0) = recv;
    *(T**)(m_data + 4) = parse;
}

template<class T>
bool IQueue<T>::SwitchQueue()
{
    T* recv = *(T**)(m_data + 0);
    if (recv->empty())
    {
        return false;
    }
    T* parse = *(T**)(m_data + 4);
    *(T**)(m_data + 0) = parse;
    *(T**)(m_data + 4) = recv;
    return true;
}

template<class T>
T* IQueue<T>::GetParseQueue()
{
    return *(T**)(m_data + 4);
}

typedef std::queue<CTcpRecvBuffer*> TcpRecvQueue;

typedef std::queue<CUdpRecvBuffer*> UdpRecvQueue;

template class CSwapQueue<TcpRecvQueue, 2>;

template class CSwapQueue<UdpRecvQueue, 2>;

template class IQueue<TcpRecvQueue>;

CTcpNetSystem::CTcpNetSystem()
    : m_tcpHandler(0), m_tcpNetworkThread(0), m_acceptThread(0), m_tcpServerPort(0)
{
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
    if (m_tcpNetworkThread)
    {
        m_tcpNetworkThread->stop();
        delete m_tcpNetworkThread;
        m_tcpNetworkThread = 0;
    }
}

void CTcpNetSystem::Init(unsigned short port)
{
    m_tcpServerPort = port;
    m_tcpHandler = new CTcpHandler;
    m_acceptThread = new CTcpAcceptThread;  // 成员为基类指针，隐式转换复现 ORIG 双 mov
    ((CTcpAcceptThread*)m_acceptThread)->attach(this);
    if (!((CTcpAcceptThread*)m_acceptThread)->CThreadInterface::begin())
    {
        throw;
    }
    m_tcpNetworkThread = new CTcpNetworkThread;  // 双 mov
    ((CTcpNetworkThread*)m_tcpNetworkThread)->attach(this);
    if (!((CTcpNetworkThread*)m_tcpNetworkThread)->CThreadInterface::begin())
    {
        throw;
    }
}

void CTcpNetSystem::InsertAcceptedPeer(CPeer* peer)
{
    CGuard<CMutex> g(&m_mutex60);
    m_peerQ.push(peer);
}

void CTcpNetSystem::SetEpollAcceptedPeers()
{
    CGuard<CMutex> g(&m_mutex60);
    if (m_peerQ.empty())
        return;
    CPeer* peer = 0;
    while (!m_peerQ.empty())
    {
        peer = m_peerQ.front();
        int r = 0;
        if ((r = m_tcpHandler->SetPeer(peer, peer->GetTcpSocket()->getHandle(), false)) != 0)
        {
            printf("G_EpollHandler()->SetPeer(peer->get_socket(%d)) %d(%s)",
                   peer->GetTcpSocket()->getHandle(), r, strerror(r));
        }
        m_peers.insert(std::make_pair((int)peer->GetTcpSocket()->getHandle(), peer));
        m_peerQ.pop();
    }
}

int CTcpNetSystem::WaitForEvent()
{
    return m_tcpHandler->WaitForEvent();
}

void* CTcpNetSystem::Acquire_TcpSendBuffer(unsigned int size)
{
    return 0;
}

void* CTcpNetSystem::Acquire_TcpSendBuffer()
{
    CGuard<CMutex> g(&m_mutex100);
    return CTcpSendBuffer::operator new(0x1804);
}

void CTcpNetSystem::PushTcpSendPacketQ(PacketHeader* pkt)
{
}

void CTcpNetSystem::PushTcpSendPacketQ(char* buf)
{
    CGuard<CMutex> g(&m_mutexe8);
    m_sendQ.push((CTcpSendBuffer*)buf);
    int n = (int)m_sendQ.size();
    if (n > 0xa)
    {
        DNF_LOG_SCOPE_LINE(0x91, "./log/TcpSend", "SEND PUSH(cnt:%d,id:%d,size:%d,ip:%d)",
            n, ((TcpPacketFields*)buf)->m_id, ((TcpPacketFields*)buf)->m_size,
            ((TcpPacketFields*)buf)->m_ip);
    }
}

int CTcpNetSystem::SendPacket()
{
    // R14：ORIG 形态——queue 访问在 guard 内层作用域，flag(esi)/ret(ebx) 寄存器变量，
    // buf(-0x2c) 用于 PopDelete、b2(-0x28) 副本用于字段访问。
    {
        register int ret;
        register int flag;
        CTcpSendBuffer* buf;
        CTcpSendBuffer* b2;
        CPeer* peer;
        int result;
        int cnt;
        {
            CGuard<CMutex> g(&m_mutexe8);
            if (m_sendQ.empty())
            {
                ret = 0;
                flag = 0;
            }
            else
            {
                buf = m_sendQ.front();
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
            std::map<unsigned int, CPeer*>::iterator it = m_peers.find(*(unsigned int*)((char*)b2 + 6));
            if (it == m_peers.end())
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
                if (peer == 0 || ((TcpPacketFields*)b2)->m_ip != peer->GetTcpSocket()->getHandle())
                {
                    DNF_LOG_SCOPE_LINE(0xc3, "./log/TcpSend", "SEND ERR:invalid peer(%x)(id:%d)(size:%d)(ip:%d)",
                        peer, ((TcpPacketFields*)b2)->m_id, ((TcpPacketFields*)b2)->m_size,
                        ((TcpPacketFields*)b2)->m_ip);
                    PopDeleteTcpSendPacketQ(buf);
                    ret = 0;
                }
                else
                {
                    result = peer->send_packet((char*)b2, ((TcpPacketFields*)b2)->m_size);
                    if (result > 0)
                    {
                        PopDeleteTcpSendPacketQ(buf);
                    }
                    else
                    {
                        cnt = (int)m_sendQ.size();
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
        CGuard<CMutex> g(&m_mutexe8);
        m_sendQ.pop();
    }
    {
        CGuard<CMutex> g2(&m_mutex100);
        delete buf;
    }
}

CPeer* CTcpNetSystem::CreatePeer()
{
    CGuard<CMutex> g(&m_mutex78);
    return new CPeer;
}

void CTcpNetSystem::DeletePeer(CPeer* peer)
{
    unsigned int handle = peer->GetTcpSocket()->getHandle();
    std::map<unsigned int, CPeer*>::iterator it = m_peers.find(handle);
    if (it != m_peers.end())
    {
        m_peers.erase(it);
    }
    CGuard<CMutex> g(&m_mutex78);
    delete peer;
}

bool CTcpNetSystem::OpenTcpService(int& sock, const char* ip, unsigned short port)
{
    CPeer* peer = CreatePeer();
    TCPSocket* tcp = peer->GetTcpSocket();
    if (!tcp->open())
    {
        puts("tcpSock.open() Fail!");
        DNF_LOG_SCOPE_LINE(0x118, "./log/TcpServer", "tcpSock.open() Fail!");
        DeletePeer(peer);
        return false;
    }
    else if (!tcp->connect(ip, port))
    {
        puts("tcpSock.connect Fail!");
        register int nPort = port;  // R10: register 局部变体（对齐 ORIG ctor 前 ebx 预装载）
        DNF_LOG_SCOPE_LINE(0x123, "./log/TcpServer", "tcpSock.connect(%s, %d) Fail!", ip, nPort);
        DeletePeer(peer);
        return false;
    }
    tcp->setOptNonBlock();
    peer->InitPeer(
        ((CSwapQueue<std::queue<CTcpRecvBuffer*, std::deque<CTcpRecvBuffer*> >, 2>*)
            Get_TcpSwapQPacket())->GetRecvQ(),
        Get_TcpRecvQLock(), Get_TcpRecvBLock());
    peer->ConnSig();
    SetEpollConnectedPeer(peer);
    sock = tcp->getHandle();
    return true;
}

void CTcpNetSystem::SetEpollConnectedPeer(CPeer* peer)
{
    CGuard<CMutex> g(&m_mutex78);
    int r = 0;
    if ((r = m_tcpHandler->SetPeer(peer, peer->GetTcpSocket()->getHandle(), 0)) != 0)
    {
        printf("G_EpollHandler()->SetPeer(peer->get_socket(%d)) %d(%s)",
               peer->GetTcpSocket()->getHandle(), r, strerror(r));
    }
    m_peers.insert(std::make_pair((int)peer->GetTcpSocket()->getHandle(), peer));
}

void CTcpNetSystem::CleanTcpSendPacketQ()
{
    while (true)
    {
        CTcpSendBuffer* p;
        {
            CGuard<CMutex> g(&m_mutexe8);
            if (m_sendQ.empty())
                break;
            p = m_sendQ.front();
            m_sendQ.pop();
        }
        {
            CGuard<CMutex> g2(&m_mutex100);
            delete p;
        }
    }
    CMyFileLog log(__FUNCTION__, 0x16b);
    log("./log/TcpSend", "Clean Tcp Send Queue Complete !");
}

void CTcpNetSystem::CleanPeers()
{
    std::map<unsigned int, CPeer*>::iterator it = m_peers.begin();
    while (it != m_peers.end())
    {
        CGuard<CMutex> g(&m_mutex78);
        delete it->second;
        ++it;
    }
    m_peers.clear();
}

CPeer* CTcpNetSystem::GetPeer(unsigned int id)
{
    std::map<unsigned int, CPeer*>::iterator it = m_peers.find(id);
    if (it != m_peers.end())
    {
        return it->second;
    }
    return 0;
}

void* CTcpNetSystem::Get_TcpSwapQPacket()
{
    return &m_recvQ;
}

CMutex* CTcpNetSystem::Get_TcpRecvQLock()
{
    return &m_mutex90;
}

CMutex* CTcpNetSystem::Get_TcpRecvBLock()
{
    return &m_mutexa8;
}

unsigned short CTcpNetSystem::Get_TcpServerPort()
{
    return m_tcpServerPort;
}

void* CTcpNetSystem::Get_TcpHandler()
{
    return m_tcpHandler;
}

void* CTcpNetSystem::Get_TcpSendQPacket()
{
    return &m_sendQ;
}

CMutex* CTcpNetSystem::Get_TcpSendQLock()
{
    return &m_mutexe8;
}

CMutex* CTcpNetSystem::Get_TcpSendBLock()
{
    return &m_mutex100;
}
