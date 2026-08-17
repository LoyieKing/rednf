//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_HADES_UDP_HEARTBEAT_H
#define PACKET_HADES_UDP_HEARTBEAT_H

#include "PacketHeader.h"

class Packet_Hades_Udp_HeartBeat : public PacketHeader {
public:
    unsigned short m_udpPort;   // +0xa  SendHeartBeatPacket param_1：唯一调用点 Timer_HadesHeartBeat::
                                //       dispatch_sig @863a760（Hades TCP 断连后每 60s 的 UDP 心跳，
                                //       Inter_HadesDisConnect 注册），实参 = CEnvironment::get_udp_port()
                                //       （本游戏服 UDP 端口），经 SendHadesUdpPacket 发往 Hades 反外挂
                                //       服务器（配置 udp_ip/udp_port_of_hades）；外部协议无公开文档，
                                //       语义由内部实参确定（已穷举内部 + 外部）
    unsigned short m_channelNo; // +0xc  同调用点实参 = CEnvironment::get_channel_no()（本服频道号，
                                //       截断为 ushort）

    Packet_Hades_Udp_HeartBeat() : PacketHeader(0xfd8, 0xe),
        m_udpPort(0), m_channelNo(0)
    {

    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Hades_Udp_HeartBeat, 0xe);

#endif  // PACKET_HADES_UDP_HEARTBEAT_H
