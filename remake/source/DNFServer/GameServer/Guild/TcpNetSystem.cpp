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
{
    *(unsigned int*)m_data = 0;
    *(unsigned int*)(m_data + 4) = 0;
    typedef std::queue<CTcpRecvBuffer*, std::deque<CTcpRecvBuffer*> > TcpRecvQueue;
    new (m_data + 8) CSwapQueue<TcpRecvQueue, 2>();
    new (m_data + 0x60) CMutex();
    new (m_data + 0x78) CMutex();
    new (m_data + 0x90) CMutex();
    new (m_data + 0xa8) CMutex();
    new (m_data + 0xc0) std::queue<CTcpSendBuffer*, std::deque<CTcpSendBuffer*> >();
    new (m_data + 0xe8) CMutex();
    new (m_data + 0x100) CMutex();
    *(unsigned int*)(m_data + 0x118) = 0;
    new (m_data + 0x11c) std::queue<CPeer*, std::deque<CPeer*> >();
    new (m_data + 0x144) std::map<unsigned int, CPeer*>();
    *(unsigned short*)(m_data + 0x15c) = 0;
}

CTcpNetSystem::~CTcpNetSystem()
{
    CleanPeers();
    if (*(void**)m_data != 0)
    {
        delete (CTcpHandler*)*(void**)m_data;
    }
    *(void**)m_data = 0;
    if (*(CTcpAcceptThread**)(m_data + 0x118) != 0)
    {
        delete *(CTcpAcceptThread**)(m_data + 0x118);
    }
    *(CTcpAcceptThread**)(m_data + 0x118) = 0;
    if (*(CTcpNetworkThread**)(m_data + 4) != 0)
    {
        delete *(CTcpNetworkThread**)(m_data + 4);
    }
    *(CTcpNetworkThread**)(m_data + 4) = 0;
    ((std::map<unsigned int, CPeer*>*)(m_data + 0x144))->~map();
    ((std::queue<CPeer*, std::deque<CPeer*> >*)(m_data + 0x11c))->~queue();
    ((CMutex*)(m_data + 0x100))->~CMutex();
    ((CMutex*)(m_data + 0xe8))->~CMutex();
    ((std::queue<CTcpSendBuffer*, std::deque<CTcpSendBuffer*> >*)(m_data + 0xc0))->~queue();
    ((CMutex*)(m_data + 0xa8))->~CMutex();
    ((CMutex*)(m_data + 0x90))->~CMutex();
    ((CMutex*)(m_data + 0x78))->~CMutex();
    ((CMutex*)(m_data + 0x60))->~CMutex();
    ((CSwapQueue<std::queue<CTcpRecvBuffer*, std::deque<CTcpRecvBuffer*> >, 2>*)
        (m_data + 8))->~CSwapQueue();
}

void CTcpNetSystem::Init(unsigned short port)
{
    *(unsigned short*)(m_data + 0x15c) = port;
    CTcpHandler* h = new CTcpHandler;
    *(CTcpHandler**)(m_data + 0) = h;
    CTcpAcceptThread* at = new CTcpAcceptThread;
    *(CTcpAcceptThread**)(m_data + 0x118) = at;
    at->attach(this);
    if (!((CThreadInterface*)at)->begin())
    {
        throw;
    }
    CTcpNetworkThread* nt = new CTcpNetworkThread;
    *(CTcpNetworkThread**)(m_data + 4) = nt;
    nt->attach(this);
    if (!((CThreadInterface*)nt)->begin())
    {
        throw;
    }
}

void CTcpNetSystem::InsertAcceptedPeer(CPeer* peer)
{
    CGuard<CMutex> g((CMutex*)(m_data + 0x60));
    ((std::queue<CPeer*, std::deque<CPeer*> >*)(m_data + 0x11c))->push(peer);
}

void CTcpNetSystem::SetEpollAcceptedPeers()
{
    CGuard<CMutex> g((CMutex*)(m_data + 0x60));
    std::queue<CPeer*, std::deque<CPeer*> >* q =
        (std::queue<CPeer*, std::deque<CPeer*> >*)(m_data + 0x11c);
    std::map<unsigned int, CPeer*>* peers =
        (std::map<unsigned int, CPeer*>*)(m_data + 0x144);
    while (!q->empty())
    {
        CPeer* peer = q->front();
        int r = 0;
        TCPSocket* tcp = peer->GetTcpSocket();
        int fd = tcp->getHandle();
        CTcpHandler* h = (CTcpHandler*)*(void**)m_data;
        r = h->SetPeer(peer, fd, false);
        if (r != 0)
        {
            printf("G_EpollHandler()->SetPeer(peer->get_socket(%d)) %d(%s)", fd, r, strerror(r));
        }
        (*peers)[(unsigned int)fd] = peer;
        q->pop();
    }
}

int CTcpNetSystem::WaitForEvent()
{
    CTcpHandler* h = (CTcpHandler*)*(void**)m_data;
    if (h != 0)
    {
        return h->WaitForEvent();
    }
    return -1;
}

void* CTcpNetSystem::Acquire_TcpSendBuffer(unsigned int size)
{
    return 0;
}

void* CTcpNetSystem::Acquire_TcpSendBuffer()
{
    CGuard<CMutex> g((CMutex*)(m_data + 0x100));
    return CTcpSendBuffer::operator new(0x1804);
}

void CTcpNetSystem::PushTcpSendPacketQ(PacketHeader* pkt)
{
}

void CTcpNetSystem::PushTcpSendPacketQ(char* buf)
{
    CGuard<CMutex> g((CMutex*)(m_data + 0xe8));
    ((std::queue<CTcpSendBuffer*, std::deque<CTcpSendBuffer*> >*)(m_data + 0xc0))
        ->push((CTcpSendBuffer*)buf);
    int cnt = (int)((std::queue<CTcpSendBuffer*, std::deque<CTcpSendBuffer*> >*)
        (m_data + 0xc0))->size();
    if (cnt > 10)
    {
        DNF_LOG_SCOPE_LINE(0x91, "./log/TcpSend", "SEND PUSH(cnt:%d,id:%d,size:%d,ip:%d)",
            cnt, (int)*(unsigned short*)buf, (int)*(unsigned short*)((char*)buf + 2),
            *(unsigned int*)((char*)buf + 6));
    }
}

void CTcpNetSystem::SendPacket()
{
    std::queue<CTcpSendBuffer*, std::deque<CTcpSendBuffer*> >* q =
        (std::queue<CTcpSendBuffer*, std::deque<CTcpSendBuffer*> >*)(m_data + 0xc0);
    CTcpSendBuffer* buf = 0;
    bool empty;
    {
        CGuard<CMutex> g((CMutex*)(m_data + 0xe8));
        empty = q->empty();
        if (!empty)
        {
            buf = q->front();
        }
    }
    if (empty || buf == 0)
    {
        return;
    }
    std::map<unsigned int, CPeer*>* peers =
        (std::map<unsigned int, CPeer*>*)(m_data + 0x144);
    std::map<unsigned int, CPeer*>::iterator it =
        peers->find(*(unsigned int*)((char*)buf + 6));
    if (it == peers->end())
    {
        DNF_LOG_SCOPE_LINE(0xba,"./log/TcpSend", "SEND ERR:no peer(id:%d,size:%d,ip:%d)",
            (unsigned int)*(unsigned short*)buf, (unsigned int)*(unsigned short*)((char*)buf + 2),
            *(unsigned int*)((char*)buf + 6));
        PopDeleteTcpSendPacketQ(buf);
        return;
    }
    CPeer* peer = it->second;
    bool invalid = true;
    if (peer != 0 &&
        *(int*)((char*)buf + 6) == peer->GetTcpSocket()->getHandle())
    {
        invalid = false;
    }
    if (invalid)
    {
        DNF_LOG_SCOPE_LINE(0xc3,"./log/TcpSend", "SEND ERR:invalid peer(%x)(id:%d)(size:%d)(ip:%d)", peer,
            (unsigned int)*(unsigned short*)buf, (unsigned int)*(unsigned short*)((char*)buf + 2),
            *(unsigned int*)((char*)buf + 6));
        PopDeleteTcpSendPacketQ(buf);
        return;
    }
    int r = peer->send_packet((char*)buf, (int)*(unsigned short*)((char*)buf + 2));
    if (r < 1)
    {
        DNF_LOG_SCOPE_LINE(0xd5,"./log/TcpSend", "SEND(id:%d,size:%d,ip:%d, cnt:%d)",
            (unsigned int)*(unsigned short*)buf, (unsigned int)*(unsigned short*)((char*)buf + 2),
            *(unsigned int*)((char*)buf + 6), (unsigned int)q->size());
    }
    else
    {
        PopDeleteTcpSendPacketQ(buf);
    }
}

void CTcpNetSystem::PopDeleteTcpSendPacketQ(CTcpSendBuffer* buf)
{
    std::queue<CTcpSendBuffer*, std::deque<CTcpSendBuffer*> >* q =
        (std::queue<CTcpSendBuffer*, std::deque<CTcpSendBuffer*> >*)(m_data + 0xc0);
    {
        CGuard<CMutex> g((CMutex*)(m_data + 0xe8));
        q->pop();
    }
    {
        CGuard<CMutex> g2((CMutex*)(m_data + 0x100));
        delete buf;
    }
}

CPeer* CTcpNetSystem::CreatePeer()
{
    CGuard<CMutex> g((CMutex*)(m_data + 0x78));
    return new CPeer;
}

void CTcpNetSystem::DeletePeer(CPeer* peer)
{
    TCPSocket* tcp = peer->GetTcpSocket();
    int handle = tcp->getHandle();
    std::map<unsigned int, CPeer*>* peers =
        (std::map<unsigned int, CPeer*>*)(m_data + 0x144);
    std::map<unsigned int, CPeer*>::iterator it = peers->find((unsigned int)handle);
    if (it != peers->end())
    {
        peers->erase(it);
    }
    CGuard<CMutex> g((CMutex*)(m_data + 0x78));
    if (peer != 0)
    {
        delete peer;
    }
}

bool CTcpNetSystem::OpenTcpService(int& sock, const char* ip, unsigned short port)
{
    CPeer* peer = CreatePeer();
    TCPSocket* tcp = peer->GetTcpSocket();
    if (tcp->open())
    {
        if (tcp->connect(ip, port))
        {
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
        puts("tcpSock.connect Fail!");
        DNF_LOG_SCOPE_LINE(0x123, "./log/TcpServer", "tcpSock.connect(%s, %d) Fail!", ip, (unsigned int)port);
        DeletePeer(peer);
        return false;
    }
    else
    {
        puts("tcpSock.open() Fail!");
        DNF_LOG_SCOPE_LINE(0x118, "./log/TcpServer", "tcpSock.open() Fail!");
        DeletePeer(peer);
        return false;
    }
}

void CTcpNetSystem::SetEpollConnectedPeer(CPeer* peer)
{
    CGuard<CMutex> g((CMutex*)(m_data + 0x78));
    TCPSocket* tcp = peer->GetTcpSocket();
    int fd = tcp->getHandle();
    int r = 0;
    CTcpHandler* h = (CTcpHandler*)*(void**)m_data;
    if (h != 0)
    {
        r = h->SetPeer(peer, fd, false);
    }
    if (r != 0)
    {
        printf("G_EpollHandler()->SetPeer(peer->get_socket(%d)) %d(%s)", fd, r, strerror(r));
    }
    (*(std::map<unsigned int, CPeer*>*)(m_data + 0x144))[(unsigned int)fd] = peer;
}

void CTcpNetSystem::CleanPeers()
{
    std::map<unsigned int, CPeer*>* peers =
        (std::map<unsigned int, CPeer*>*)(m_data + 0x144);
    for (std::map<unsigned int, CPeer*>::iterator it = peers->begin();
         it != peers->end(); ++it)
    {
        CGuard<CMutex> g((CMutex*)(m_data + 0x78));
        delete it->second;
    }
    peers->clear();
}

void CTcpNetSystem::CleanTcpSendPacketQ()
{
    std::queue<CTcpSendBuffer*, std::deque<CTcpSendBuffer*> >* q =
        (std::queue<CTcpSendBuffer*, std::deque<CTcpSendBuffer*> >*)(m_data + 0xc0);
    for (;;)
    {
        CGuard<CMutex> g((CMutex*)(m_data + 0xe8));
        if (q->empty())
        {
            break;
        }
        CTcpSendBuffer* buf = q->front();
        q->pop();
        {
            CGuard<CMutex> g2((CMutex*)(m_data + 0x100));
            delete buf;
        }
    }
    DNF_LOG_SCOPE_LINE(0x16b, "./log/TcpSend", "Clean Tcp Send Queue Complete !");
}

CPeer* CTcpNetSystem::GetPeer(unsigned int id)
{
    std::map<unsigned int, CPeer*>* peers =
        (std::map<unsigned int, CPeer*>*)(m_data + 0x144);
    std::map<unsigned int, CPeer*>::iterator it = peers->find(id);
    return it == peers->end() ? 0 : it->second;
}

void* CTcpNetSystem::Get_TcpSwapQPacket()
{
    return m_data + 8;
}

CMutex* CTcpNetSystem::Get_TcpRecvQLock()
{
    return (CMutex*)(m_data + 0x90);
}

CMutex* CTcpNetSystem::Get_TcpRecvBLock()
{
    return (CMutex*)(m_data + 0xa8);
}

unsigned short CTcpNetSystem::Get_TcpServerPort()
{
    return *(unsigned short*)(m_data + 0x15c);
}

void* CTcpNetSystem::Get_TcpHandler()
{
    return *(void**)m_data;
}

void* CTcpNetSystem::Get_TcpSendQPacket()
{
    return m_data + 0xc0;
}

CMutex* CTcpNetSystem::Get_TcpSendQLock()
{
    return (CMutex*)(m_data + 0xe8);
}

CMutex* CTcpNetSystem::Get_TcpSendBLock()
{
    return (CMutex*)(m_data + 0x100);
}

