// df_monitor_r — TcpNetSystem（从 MonitorTypes/App/Table 拆分）
#include <stdio.h>
#include "RawAccess.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <signal.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <algorithm>

#include "TcpNetSystem.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"

// P375：ORIG 反汇编实测 TcpSendBuffer 头部字段（id@0/size@2/pad@4/ip@6）——
// 与 guild 服务 identical 版本同源，仅本 TU 使用。
#pragma pack(push, 1)
struct TcpPacketFields
{
    unsigned short m_id;   // +0
    unsigned short m_size; // +2
    unsigned short m_pad;  // +4
    int m_ip;              // +6
};
#pragma pack(pop)
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"
#include "DNFPacketBuffer.h"
#include "DNFTcpAcceptThread.h"
#include "DNFTcpHandler.h"
#include "DNFTcpNetworkThread.h"
#include "DNFTcpSocket.h"
#include "Peer.h"

EpollHandler::EpollHandler()
{
    Init();
}

EpollHandler::~EpollHandler()
{
    Destroy();
}

void EpollHandler::Destroy()
{
    if (m_events != 0)
    {
        ::operator delete[](m_events);
    }
    m_events = 0;
}

int EpollHandler::Init()
{
    m_epollFd = epoll_create(1000);
    if (m_epollFd < 0)
    {
        puts("[Epoll::init] Can\'t init epoll create");
        return 0;
    }
    m_events = new epoll_event[1000];
    if (m_events == 0)
    {
        printf("[Epoll::init] Can\'t alloc event memory");
        return 0;
    }
    return 1;
}

int EpollHandler::SetEpoll(void* peer, int fd, bool flag)
{
    if (flag)
    {
        m_event.events = 0x8000001d;
    }
    else
    {
        m_event.events = 0x1d;
    }
    m_event.data.ptr = peer;
    CGuard<CMutex> guard(&m_mutex);
    register int r;
    if (epoll_ctl(m_epollFd, 1, fd, &m_event) < 0)
    {
        r = errno;
    }
    else
    {
        r = 0;
    }
    return r;
}

int EpollHandler::ResetEpoll(int fd)
{
    memset(&m_event, 0, 0xc);
    m_event.events = 1;
    CGuard<CMutex> guard(&m_mutex);
    register int r;
    if (epoll_ctl(m_epollFd, 2, fd, &m_event) < 0)
    {
        r = errno;
    }
    else
    {
        r = 0;
    }
    return r;
}

int EpollHandler::WaitForEvent()
{
    epoll_event* events = (epoll_event*)GetEpollEvents();
    int fd = GetEpollFD();
    return epoll_wait(fd, events, 1000, 100);
}

char EpollHandler::IsSetErrEvent(int idx)
{
    return (char)((m_events[idx].events & 0x18) != 0);
}

char EpollHandler::IsSetInEvent(int idx)
{
    return (char)((m_events[idx].events & 1) != 0);
}

char EpollHandler::IsSetOutEvent(int idx)
{
    return (char)((m_events[idx].events & 4) != 0);
}

void* EpollHandler::GetEventPtr(int idx)
{
    return m_events[idx].data.ptr;
}

CTcpNetSystem::CTcpNetSystem()
    : m_handler(0), m_networkThread(0), m_acceptThread(0), m_port(0)
{
}

CTcpNetSystem::~CTcpNetSystem()
{
    CleanPeers();
    if (m_handler != 0)
    {
        delete m_handler;
        m_handler = 0;
    }
    if (m_acceptThread != 0)
    {
        m_acceptThread->stop();
        delete m_acceptThread;
        m_acceptThread = 0;
    }
    if (m_networkThread != 0)
    {
        m_networkThread->stop();
        delete m_networkThread;
        m_networkThread = 0;
    }
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-MON-DIFF-0021 已消除）
void CTcpNetSystem::Init(unsigned short port)
{
    m_port = port;
    m_handler = new CTcpHandler;
    m_acceptThread = new CTcpAcceptThread;
    ((CTcpAcceptThread*)m_acceptThread)->attach(this);
    if (!m_acceptThread->begin())
    {
        throw;
    }
    m_networkThread = new CTcpNetworkThread;
    ((CTcpNetworkThread*)m_networkThread)->attach(this);
    if (!m_networkThread->begin())
    {
        throw;
    }
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-MON-DIFF-0019 已消除）
bool CTcpNetSystem::OpenTcpService(int& sockRef, const char* ip, unsigned short port)
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
    if (!sock->connect(ip, port))
    {
        puts("tcpSock.connect Fail!");
        DNF_LOG_SCOPE_LINE(0x123, "./log/TcpServer", "tcpSock.connect(%s, %d) Fail!", ip, (unsigned int)port);
        DeletePeer(peer);
        return 0;
    }
    sock->setOptNonBlock();
    register CMutex* b = Get_TcpRecvBLock();
    register CMutex* q = Get_TcpRecvQLock();
    peer->InitPeer((std::queue<CTcpRecvBuffer*>*)Get_TcpSwapQPacket()->GetRecvQ(), q, b);
    peer->ConnSig();
    SetEpollConnectedPeer(peer);
    sockRef = sock->getHandle();
    return 1;
}

void CTcpNetSystem::CleanPeers()
{
    std::map<unsigned int, CPeer*>::iterator it = m_peers.begin();
    while (it != m_peers.end())
    {
        CGuard<CMutex> guard(&m_mutex78);
        register CPeer* p = it->second;
        delete p;
        ++it;
    }
    m_peers.clear();
}

void CTcpNetSystem::SetEpollAcceptedPeers()
{
    CGuard<CMutex> guard(&m_mutex60);
    if (m_peerQ.empty())
    {
        return;
    }
    CPeer* peer = 0;
    while (!m_peerQ.empty())
    {
        peer = m_peerQ.front();
        int result = 0;
        if ((result = m_handler->SetPeer(peer, peer->GetTcpSocket()->getHandle(), false)) != 0)
        {
            printf("G_EpollHandler()->SetPeer(peer->get_socket(%d)) %d(%s)",
                   peer->GetTcpSocket()->getHandle(), result, strerror(result));
        }
        m_peers.insert(std::make_pair((int)peer->GetTcpSocket()->getHandle(), peer));
        m_peerQ.pop();
    }
}

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0018 | monitor | 与ORIG差异=DIFF | CTcpNetSystem::SendPacket | 详见 function_reports/monitor/_ZN13CTcpNetSystem10SendPacketEv.md
int CTcpNetSystem::SendPacket()
{
    // [MonitorRest 本轮] DIFF 18→NEAR 6：已把 b2/buf 字段访问改为本地 packed TcpPacketFields
    //   （id@0/size@2/pad@4/ip@6，与 guild identical 版同源）⇒ 长度 208→207 与 ORIG 一致。
    //   剩余 6 条 = CGuard(guard) 与 map::find 返回的 iterator 两槽互换：ORIG guard=-0x50/iterator=-0x4c，
    //   我方 guard=-0x4c/iterator=-0x50。已试无效：iterator 声明拆成先声明后赋值（→帧 0x6c→0x7c 劣化）、
    //   删除包裹函数体的外层块（槽序不变）。登记为「BIND/临时对象分配顺序」族。
    // P375：ORIG 与 guild/dbmw 的 CTcpNetSystem::SendPacket 逐条同源（guild 已 identical）
    // ⇒ 结构照搬 guild 模板：全部局部在内层作用域、单出口 goto done、错误分支 ret=0。
    {
        register int ret;
        register int flag;
        CTcpSendBuffer* buf;
        CTcpSendBuffer* b2;
        CPeer* peer;
        int result;
        int cnt;
        {
            CGuard<CMutex> guard(&m_mutexe8);
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
            std::map<unsigned int, CPeer*>::iterator it =
                m_peers.find(*(unsigned int*)((char*)b2 + 6));
            if (it == m_peers.end())
            {
                DNF_LOG_SCOPE_LINE(0xba,"./log/TcpSend", "SEND ERR:no peer(id:%d,size:%d,ip:%d)",
                    ((TcpPacketFields*)b2)->m_id,
                    ((TcpPacketFields*)b2)->m_size,
                    ((TcpPacketFields*)b2)->m_ip);
                PopDeleteTcpSendPacketQ(buf);
                ret = 0;
            }
            else
            {
                peer = it->second;
                if (peer == 0 ||
                    ((TcpPacketFields*)b2)->m_ip != peer->GetTcpSocket()->getHandle())
                {
                    DNF_LOG_SCOPE_LINE(0xc3,"./log/TcpSend", "SEND ERR:invalid peer(%x)(id:%d)(size:%d)(ip:%d)",
                        peer, ((TcpPacketFields*)b2)->m_id,
                        ((TcpPacketFields*)b2)->m_size,
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
                        DNF_LOG_SCOPE_LINE(0xd5,"./log/TcpSend", "SEND(id:%d,size:%d,ip:%d, cnt:%d)",
                            ((TcpPacketFields*)b2)->m_id,
                            ((TcpPacketFields*)b2)->m_size,
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

int CTcpNetSystem::WaitForEvent()
{
    return m_handler->WaitForEvent();
}

void CTcpNetSystem::DeletePeer(CPeer* peer)
{
    unsigned int fd = peer->GetTcpSocket()->getHandle();
    std::map<unsigned int, CPeer*>::iterator it = m_peers.find(fd);
    if (it != m_peers.end())
    {
        m_peers.erase(it);
    }
    CGuard<CMutex> guard(&m_mutex78);
    delete peer;
}

CPeer* CTcpNetSystem::CreatePeer()
{
    CGuard<CMutex> guard(&m_mutex78);
    return new CPeer;
}

void CTcpNetSystem::InsertAcceptedPeer(CPeer* peer)
{
    CGuard<CMutex> guard(&m_mutex60);
    m_peerQ.push(peer);
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

void CTcpNetSystem::PopDeleteTcpSendPacketQ(CTcpSendBuffer* buf)
{
    {
        CGuard<CMutex> guard1(&m_mutexe8);
        m_sendQ.pop();
    }
    {
        CGuard<CMutex> guard2(&m_mutex100);
        delete buf;
    }
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-MON-DIFF-0020 已消除）
void CTcpNetSystem::CleanTcpSendPacketQ()
{
    while (true)
    {
        CTcpSendBuffer* buf;
        register int flag;
        {
            CGuard<CMutex> guard1(&m_mutexe8);
            if (m_sendQ.empty())
            {
                flag = 0;
            }
            else
            {
                buf = m_sendQ.front();
                m_sendQ.pop();
                flag = 1;
            }
        }
        if (flag == 0)
        {
            break;
        }
        {
            CGuard<CMutex> guard2(&m_mutex100);
            delete buf;
        }
    }
    DNF_LOG_SCOPE_LINE(0x16b, "./log/TcpSend", "Clean Tcp Send Queue Complete !");
}

void CTcpNetSystem::SetEpollConnectedPeer(CPeer* peer)
{
    {
        CGuard<CMutex> guard(&m_mutex78);
        int rc = 0;
        if ((rc = m_handler->SetPeer(peer, peer->GetTcpSocket()->getHandle(), false)) != 0)
        {
            printf("G_EpollHandler()->SetPeer(peer->get_socket(%d)) %d(%s)",
                   peer->GetTcpSocket()->getHandle(), rc, strerror(rc));
        }
        m_peers.insert(std::make_pair(peer->GetTcpSocket()->getHandle(), peer));
    }
}

unsigned short CTcpNetSystem::Get_TcpServerPort() { return m_port; }

void* CTcpNetSystem::Acquire_TcpSendBuffer()
{
    CGuard<CMutex> guard(&m_mutex100);
    return new CTcpSendBuffer;
}

void CTcpNetSystem::PushTcpSendPacketQ(char* buf)
{
    CGuard<CMutex> guard(&m_mutexe8);
    m_sendQ.push((CTcpSendBuffer*)buf);
    int size = (int)m_sendQ.size();
    if (10 < size)
    {
        DNF_LOG_SCOPE_LINE(0x91,"./log/TcpSend", "SEND PUSH(cnt:%d,id:%d,size:%d,ip:%d)", size,
            (unsigned int)((PacketHeader*)buf)->packetId,
            (unsigned int)((PacketHeader*)buf)->packetSize,
            ((PacketHeader*)buf)->m_connNo);
    }
}

CMutex* CTcpNetSystem::Get_TcpRecvBLock() { return &m_mutexa8; }

CMutex* CTcpNetSystem::Get_TcpRecvQLock() { return &m_mutex90; }

CTcpHandler* CTcpNetSystem::Get_TcpHandler() { return m_handler; }

CMutex* CTcpNetSystem::Get_TcpSendBLock() { return &m_mutex100; }

CMutex* CTcpNetSystem::Get_TcpSendQLock() { return &m_mutexe8; }

std::queue<CTcpSendBuffer*>* CTcpNetSystem::Get_TcpSendQPacket() { return &m_sendQ; }

CSwapQueue<std::queue<CTcpRecvBuffer*, std::deque<CTcpRecvBuffer*, std::allocator<CTcpRecvBuffer*> > >, 2>*
    CTcpNetSystem::Get_TcpSwapQPacket()
{
    return &m_recvSwapQ;
}
