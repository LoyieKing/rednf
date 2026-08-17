//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_BROADCAST_MSG_H
#define PACKET_BROADCAST_MSG_H

#include "PacketHeader.h"

class Packet_Broadcast_Msg : public PacketHeader {
public:
    char m_padA[4];          // +0xa（第三轮穷举 2026-08-16：语料 0x2711 唯一发送点
                             //      _BroadcastRunaway@82eab7a 仅整字置 0（local_124=0），
                             //      0x2711 其余出现处为 oldGmRoutine 测试数据无关；
                             //      monitor OnBroadcastMsg 不读取，只读 m_len/m_text 并整包
                             //      转发；无消费端证据，保留占位名）
    unsigned char m_len;     // +0xe（_BroadcastRunaway：strlen 后写入；packetSize = len + 0xf）
    char m_text[0xff];       // +0xf（_BroadcastRunaway：strncpy 0xff）
    Packet_Broadcast_Msg() : PacketHeader(0x2711, 0x10e){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Broadcast_Msg, 0x10e);

#endif  // PACKET_BROADCAST_MSG_H
