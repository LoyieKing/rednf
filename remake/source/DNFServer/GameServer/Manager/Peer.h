#ifndef PEER_H_
#define PEER_H_

#include "DNFPacketBuffer.h"
#include "DNFTcpSocket.h"

class CMutex;

// ---- CPeer：TCPSocket + 0x181c ----
class CPeer : public TCPSocket
{
public:
    CPeer();
    ~CPeer();
    static void* operator new(unsigned int size);
    static void operator delete(void* ptr);
    static void operator delete(void* ptr, unsigned int size);
    TCPSocket* GetTcpSocket();
    int get_remain_sendlen();
    int recv_packet();
    int send_packet();
    int send_packet(char* buf, int len);
    void InitPeer(TcpRecvQueue* recvQ, CMutex* qLock, CMutex* bLock);
    // ORIG 返回类型 bool（RecvPacket 中 !parsing(...) 编译为 xor $1）。
    bool parsing(int len);
    void ConnSig();
    void DisConnSig();
    bool RecvPacket();
    char m_sendData[0x1800];    // +0x1c（内联收发缓冲，m_sendBuf 指向此处）
    char* m_sendBuf;        // +0x181c
    int m_recvLen;          // +0x1820
    int m_sendLen;          // +0x1824
    TcpRecvQueue* m_recvQ;  // +0x1828
    CMutex* m_sendBLock;    // +0x182c
    CMutex* m_sendQLock;    // +0x1830
    unsigned int m_remainSendLen;    // +0x1834（ORIG 为 unsigned：+= int 时生成 edx=mem;eax=len;add 序列）
    char* m_recvBuf;        // +0x1838
    char m_data183c[0x96004];  // +0x183c（sizeof(CPeer)=0x97840，与 MemPool 池大小一致）
};

#endif  // PEER_H_
