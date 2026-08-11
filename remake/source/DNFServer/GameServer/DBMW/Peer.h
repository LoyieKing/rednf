#ifndef DBMW_PEER_H_
#define DBMW_PEER_H_

#include "DBMWCommon.h"


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
    bool parsing(int len);
    void ConnSig();
    void DisConnSig();
    bool RecvPacket();
    char m_gap1c[0x1800];   // +0x1c（收包缓冲区：this+0x1c..this+0x181c，代码以指针算术访问，无成员）
    char* m_sendBuf;        // +0x181c
    int m_recvLen;          // +0x1820
    int m_sendLen;          // +0x1824
    TcpRecvQueue* m_recvQ;  // +0x1828
    CMutex* m_sendBLock;    // +0x182c
    CMutex* m_sendQLock;    // +0x1830
    int m_remainSendLen;    // +0x1834
    char* m_recvBuf;        // +0x1838
    char m_data183c[0x96004];  // +0x183c（sizeof(CPeer)=0x97840，与 MemPool 池大小一致）
};

#endif  // DBMW_PEER_H_
