#ifndef DNFINNERMSGHANDLER_H_
#define DNFINNERMSGHANDLER_H_

#include <queue>
#include <string>
#include "PacketHeader.h"
#include "Thread.h"

#include "PacketHeader.h"

class CInnerMsgHandler;
struct Packet_Server_Queue_Load_Statistic;

// from GuildPacket.h
class CInnerMsgHandler
{
public:
    CInnerMsgHandler();
    ~CInnerMsgHandler();
    void SendStopNetworkThread();
};

// from GuildPackets.h
class Packet_Server_Queue_Load_Statistic : public PacketHeader {
public:
    Packet_Server_Queue_Load_Statistic();
    unsigned char m_flag;   // +0xa
    unsigned char m_param;  // +0xb
    unsigned short m_value; // +0xc
};

#endif
