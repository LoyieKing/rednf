//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_MEGAPHONE_H
#define PACKET_MONITOR_MEGAPHONE_H

#include "PacketHeader.h"

class Packet_Monitor_MegaPhone : public PacketHeader {
public:
    char m_sg;  // +a（df_monitor_r OnMonitorMegaPhoneMsg 转发前写入 Get_ServerGroup()，同族 Hyper_Link 已同名；game 侧 ctor 清零、set() 不写）
    char m_channel;  // +b（send_chat_msg case 0xb/c/e：set() 参数1 = CEnvironment::get_channel_no）
    unsigned short m_uniqueId;  // +c（send_chat_msg：set() 参数2 = CUser::get_unique_id）
    char m_nameLength;  // +e（名字长度，set() 参数3 为 getCurCharacName；ctor 未触碰）
    char m_name[0x1e];  // +f..+2c（名字区 30B，与家族 0x1e 缓冲一致；ctor 仅清 m_name[0]）
    char m_msgLength;  // +2d（set() 参数4 = 消息长度；SendPacket 大小 = msgLen + 0x2e）
    char m_msg[0x100];  // +2e..+12d（消息体；msg 起点 +0x2e 由 SendPacket 大小 msgLen+0x2e 反推；ctor 仅清 m_msg[0]）

    Packet_Monitor_MegaPhone() : PacketHeader(0x546, 0x12e),
        m_sg(0), m_channel(0), m_uniqueId(0), m_msgLength(0)
    {
        m_name[0] = 0;
        m_msg[0] = 0;
    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Monitor_MegaPhone, 0x12e);

#endif  // PACKET_MONITOR_MEGAPHONE_H
