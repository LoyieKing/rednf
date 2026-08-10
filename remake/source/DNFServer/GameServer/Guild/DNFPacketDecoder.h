#ifndef DNFPACKETDECODER_H_
#define DNFPACKETDECODER_H_

#include <queue>
#include <string>
#include "PacketHeader.h"
#include "Thread.h"

#include "PacketHeader.h"

class CApplication;
class CPacketDecoder;
class CTcpRecvBuffer;
class CUdpRecvBuffer;

// from GuildPacket.h
class CPacketDecoder
{
public:
    CPacketDecoder();
    ~CPacketDecoder();
    void Attach(CApplication* app);
    void Process();
    void TcpProcess();
    void UdpProcess();
    int MsgDecode(PacketHeader* pkt);
    void SetTCPQueue(std::queue<CTcpRecvBuffer*>* q);
    void SetUdpQueue(std::queue<CUdpRecvBuffer*>* q);
    char m_data[0x3ec0];
};

CPacketDecoder* CPacketDecoderInstance();

#endif
