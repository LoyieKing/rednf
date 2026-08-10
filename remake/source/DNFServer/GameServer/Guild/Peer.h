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
    int RecvPacket();
    int recv_packet();
    int parsing(int len);
    int send_packet(char* buf, int len);
    int send_packet();
    void DisConnSig();
    void ConnSig();
    char m_data[0x97824];   // +0x1c 起，sizeof(CPeer)=0x97840（MemPool chunk）
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
