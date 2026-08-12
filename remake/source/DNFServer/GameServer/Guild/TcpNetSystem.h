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
    void Init();
    T* GetRecvQ();
    T* GetParseQ();
    void SwapQ();
    char m_data[0x58];
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
    IQueue();
    ~IQueue();
    static IQueue<T>* Get();
    void InitQueue(T* recv, T* parse);
    bool SwitchQueue();
    T* GetParseQueue();
    char m_data[0x30];
};

#endif
