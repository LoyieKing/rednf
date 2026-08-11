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
#include "TcpNetSystem.h"

class CPeer;
class CTcpNetSystem;
class CTcpSendBuffer;

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
    void SendPacket();
    bool OpenTcpService(int& sock, const char* ip, unsigned short port);
    void SetEpollAcceptedPeers();
    void SetEpollConnectedPeer(CPeer* peer);
    char m_data[0x15c];
    unsigned short m_tcpServerPort;   // +0x15c
};

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
