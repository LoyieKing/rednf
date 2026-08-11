#ifndef TCP_NET_SYSTEM_H_
#define TCP_NET_SYSTEM_H_

#include <map>
#include <queue>

#include "DNFPacketBuffer.h"
#include "Thread.h"

class CPeer;
class CTcpAcceptThread;
class CTcpHandler;
class CTcpNetworkThread;
class CThreadInterface;  // R10: m_acceptThread/m_field4 的 ORIG 基类指针类型

// ---- CTcpNetSystem：0x160 ----
class CTcpNetSystem
{
public:
    CTcpNetSystem();
    ~CTcpNetSystem();
    void Init(unsigned short port);
    int OpenTcpService(int& serverCount, const char* ip, unsigned short port);
    int WaitForEvent();
    int SendPacket();
    void CleanPeers();
    void CleanTcpSendPacketQ();
    void DeletePeer(CPeer* peer);
    CPeer* GetPeer(unsigned int idx);
    CPeer* CreatePeer();
    void InsertAcceptedPeer(CPeer* peer);
    void SetEpollConnectedPeer(CPeer* peer);
    void SetEpollAcceptedPeers();
    void PopDeleteTcpSendPacketQ(CTcpSendBuffer* buf);
    void PushTcpSendPacketQ(char* buf);
    CTcpSendBuffer* Acquire_TcpSendBuffer();
    unsigned short Get_TcpServerPort();
    CTcpHandler* Get_TcpHandler();
    CSwapQueue<TcpRecvQueue, 2>* Get_TcpSwapQPacket();
    void* Get_TcpRecvQPacket();
    CMutex* Get_TcpRecvQLock();
    CMutex* Get_TcpRecvBLock();
    CMutex* Get_TcpSendQLock();
    CMutex* Get_TcpSendBLock();
    CTcpSendQueue* Get_TcpSendQPacket();
    CTcpHandler* m_tcpHandler;      // +0
    CThreadInterface* m_field4;     // +4  // R10: ORIG 为基类指针（同 m_acceptThread）
    CSwapQueue<TcpRecvQueue, 2> m_recvSwapQueue;  // +8
    CMutex m_mutex60;               // +0x60
    CMutex m_mutex78;               // +0x78
    CMutex m_mutex90;               // +0x90
    CMutex m_mutexA8;               // +0xa8
    CTcpSendQueue m_sendQueue;      // +0xc0
    CMutex m_mutexE8;               // +0xe8
    CMutex m_mutex100;              // +0x100
    // R10: ORIG 成员为基类指针（派生类赋值产生 ebx->eax->edx 转换临时形态）
    CThreadInterface* m_acceptThread;  // +0x118
    std::queue<CPeer*> m_peerQueue;    // +0x11c
    std::map<unsigned int, CPeer*> m_peerMap;  // +0x144
    unsigned short m_serverPort;    // +0x15c
    char m_pad[2];
};

#endif  // TCP_NET_SYSTEM_H_
