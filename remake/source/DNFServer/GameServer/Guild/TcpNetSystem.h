#ifndef TCPNETSYSTEM_H_
#define TCPNETSYSTEM_H_

#include <map>
#include <queue>
#include <deque>
#include <list>
#include <algorithm>
#include <string>
#include <vector>
#include "Thread.h"
#include <pthread.h>

#include "PacketHeader.h"

class CPeer;
class CTcpNetSystem;
class CTcpSendBuffer;
class CTcpHandler;
class CTcpAcceptThread;
class CTcpNetworkThread;
class CTcpRecvBuffer;
class CThreadInterface;

// from GuildThread.h
class CSwapQueueBase
{
public:
    CSwapQueueBase();
    ~CSwapQueueBase();
    void* m_data[0x58 / 4];
};

// from GuildThread.h
template<class T, int N>
class CSwapQueue
{
public:
    CSwapQueue();
    ~CSwapQueue();
    void Push(T* item);
    void Pop();
    // [2026-09-15 修复/P216] ORIG 0x08053dd8 `CSwapQueue<Tcp,2>::C1` 尾部 `call Init`
    //   **不布 EH landing**（无成员析构 cleanup 块）；4.4.7 对「同 TU 可见的弱成员调用」
    //   恒布 landing ⇒ Init 加 throw()（体非空，call 不被折叠）后 C1 与 ORIG 逐条一致。
    void Init() throw();
    T* GetRecvQ();
    T* GetParseQ();
    void SwapQ();
    unsigned int m_recv;    // +0x00（ORIG CSwapQueue::Init/GetRecvQ/SwapQ 实证）
    unsigned int m_parse;   // +0x04（ORIG CSwapQueue::Init/GetParseQ/SwapQ 实证）
    T m_queue[N];           // +0x08，元素 0x28B×N → 合计 0x58（ORIG C1/D1 循环步长实证）
};

// from GuildDomain.h
class CTcpNetSystem
{
public:
    CTcpNetSystem();
    ~CTcpNetSystem();
    void PushTcpSendPacketQ(PacketHeader* pkt);
    void* Acquire_TcpSendBuffer(unsigned int size);
    void Process();
    void* Get_TcpHandler();
    void* Get_TcpSwapQPacket();
    CMutex* Get_TcpRecvQLock();
    CMutex* Get_TcpRecvBLock();
    void* Get_TcpSendQPacket();
    CMutex* Get_TcpSendQLock();
    CMutex* Get_TcpSendBLock();
    unsigned short Get_TcpServerPort();
    void Init(unsigned short port);
    int WaitForEvent();
    CPeer* CreatePeer();
    void DeletePeer(CPeer* peer);
    void InsertAcceptedPeer(CPeer* peer);
    CPeer* GetPeer(unsigned int id);
    void CleanPeers();
    void PushTcpSendPacketQ(char* buf);
    void CleanTcpSendPacketQ();
    void* Acquire_TcpSendBuffer();
    void PopDeleteTcpSendPacketQ(CTcpSendBuffer* buf);
    int SendPacket();
    bool OpenTcpService(int& sock, const char* ip, unsigned short port);
    void SetEpollAcceptedPeers();
    void SetEpollConnectedPeer(CPeer* peer);
    CTcpHandler* m_tcpHandler;              // +0
    CThreadInterface* m_tcpNetworkThread;   // +4
    CSwapQueue<std::queue<CTcpRecvBuffer*, std::deque<CTcpRecvBuffer*> >, 2> m_recvQ; // +8 (0x58)
    CMutex m_mutex60;                       // +0x60
    CMutex m_mutex78;                       // +0x78
    CMutex m_mutex90;                       // +0x90
    CMutex m_mutexa8;                       // +0xa8
    std::queue<CTcpSendBuffer*, std::deque<CTcpSendBuffer*> > m_sendQ; // +0xc0 (0x28)
    CMutex m_mutexe8;                       // +0xe8
    CMutex m_mutex100;                      // +0x100
    CThreadInterface* m_acceptThread;       // +0x118
    std::queue<CPeer*, std::deque<CPeer*> > m_peerQ; // +0x11c (0x28)
    std::map<unsigned int, CPeer*> m_peers; // +0x144 (0x18)
    unsigned short m_tcpServerPort;         // +0x15c
};

// from GuildThread.h
class IQueueBase
{
public:
    IQueueBase();
    ~IQueueBase();
    char m_data[0x30];
};

// from GuildThread.h
template<class T>
class IQueue
{
public:
    // [2026-09-15 修复/P216] ORIG 0x0806435a `IQueue<Tcp>::C1` 为**空体外联定义**，
    //   且 ORIG `IQueue<Tcp>::Get`（0x08064116，0x63 字节）**无任何 EH 段**
    //   （无 `__cxa_guard_abort`/`_Unwind_Resume`）。4.4.7 对「同 TU 可见的弱 ctor 调用」
    //   恒布 landing pad ⇒ 声明+定义加 `throw()` 后与 ORIG 的 guard 段形状一致。
    IQueue() throw();
    ~IQueue();
    static IQueue<T>* Get();
    void InitQueue(T* recv, T* parse);
    bool SwitchQueue();
    T* GetParseQueue();
    T* m_recv;              // +0x00（ORIG InitQueue/GetParseQueue/SwitchQueue 实证）
    T* m_parse;             // +0x04（same）
    char m_pad[0x28];       // +0x08..+0x30
};

#endif
