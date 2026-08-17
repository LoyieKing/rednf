//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_CUTOFF_UDP_REPLY_USERINFO_H
#define PACKET_CUTOFF_UDP_REPLY_USERINFO_H

#include "PacketHeader.h"
#include <string.h>

class Packet_CutOff_UDP_Reply_UserInfo : public PacketHeader {
public:
    unsigned short m_count;  // +a（send_userinfos_to_cutoff_server：每桶 acc_id 数；packetSize = count*4 + 0xe；memcpy 长度 count*4）
    unsigned char m_channel;  // +c（send_userinfos_to_cutoff_server：CEnvironment::get_channel_no）
    unsigned char m_group;  // +d（send_userinfos_to_cutoff_server：CEnvironment+0x378 组号；同族 Packet_DoubleCheck_UDP_Login::m_group）
    unsigned int m_accIds[0x2bc];  // +e..+afd（acc_id 列表，memcpy count*4 字节；ctor memset 区域；注：首个 acc_id 亦写入 +0x6 m_connNo 槽）

    Packet_CutOff_UDP_Reply_UserInfo() : PacketHeader(0x3f9, 0xafe),
        m_count(0), m_channel(255), m_group(0)
    {
    memset(m_accIds, 0, sizeof(m_accIds));
    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_CutOff_UDP_Reply_UserInfo, 0xafe);


#endif //PACKET_CUTOFF_UDP_REPLY_USERINFO_H
