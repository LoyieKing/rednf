#ifndef PEER_H_
#define PEER_H_

#include <map>
#include <queue>
#include <deque>
#include <list>
#include <algorithm>
#include <string>
#include <vector>
#include "Thread.h"
#include "PacketHeader.h"

#include "PacketHeader.h"
#include "DNFTcpSocket.h"
#include "MemPool.h"

class CPeer;
class CTcpRecvBuffer;
struct Packet_InnerPakcet_Login;
struct Packet_InnerPakcet_Logout;

// from GuildDomain.h
class CPeer : public TCPSocket
{
public:
    CPeer();
    ~CPeer();
    static void* operator new(unsigned int size);
    static void operator delete(void* p);
    static void operator delete(void* p, unsigned int size);
    TCPSocket* GetTcpSocket();
    int get_remain_sendlen();
    void InitPeer(std::queue<CTcpRecvBuffer*>* q, CMutex* lock1, CMutex* lock2);
    bool RecvPacket();
    int recv_packet();
    bool parsing(int len);
    int send_packet(char* buf, int len);
    int send_packet();
    void DisConnSig();
    void ConnSig();
    char m_data[0x1800];        // +0x1c..+0x181b
    char* m_buf;                // +0x181c
    int m_remainLen;            // +0x1820
    int m_alreadyRead;          // +0x1824
    void* m_recvQ;              // +0x1828
    void* m_bLock;              // +0x182c
    void* m_qLock;              // +0x1830
    int m_sendRemain;           // +0x1834
    char* m_sendPtr;            // +0x1838
    char m_sendBuf[0x96000];    // +0x183c
    unsigned int m_field9783c;  // +0x9783c
};

// from GuildPackets.h
class Packet_InnerPakcet_Login : public PacketHeader {
public:
    Packet_InnerPakcet_Login();
};

// from GuildPackets.h
class Packet_InnerPakcet_Logout : public PacketHeader {
public:
    Packet_InnerPakcet_Logout();
};

#endif
