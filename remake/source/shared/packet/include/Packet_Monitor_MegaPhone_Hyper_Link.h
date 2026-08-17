//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_MEGAPHONE_HYPER_LINK_H
#define PACKET_MONITOR_MEGAPHONE_HYPER_LINK_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Monitor_MegaPhone_Hyper_Link : public PacketHeader {
public:
    char m_sg;  // +a（df_monitor_r OnMonitorMegaPhoneMsgHyperLink 转发前写入 Get_ServerGroup()，MonitorMegaPhonePkt.m_sg）
    char m_channel;  // +b（send_chat_msg_hyper_link case 0xb/c/e/0x23：set() 参数1 = CEnvironment::get_channel_no；同族 Packet_Monitor_MegaPhone +b m_channel）
    unsigned short m_uniqueId;  // +c（send_chat_msg_hyper_link：set() 参数2 = CUser::get_unique_id；同族 +c m_uniqueId）
    char m_nameLength;  // +e（send_chat_msg_hyper_link：set() 参数3 = getCurCharacName；同族 +e m_nameLength）
    char m_name[0x1e];  // +f..+2c（send_chat_msg_hyper_link：set() 参数3 名字内容；同族 +f m_name[0x1e]）
    unsigned char m_itemCount;  // +2d（send_chat_msg_hyper_link：set() 参数7 = MSG+0x13b 超链接道具个数；家族包 m_itemCount 均为 items 前 1 字节）
    char m_items[0x68];  // +2e..+95（ctor memset 0x68=hyperlink_item_info 大小；set() 传入 hyperlink_item_info*）
    char m_pad96[0xd0];  // +96..+165（0xd0 == 2×0x68：家族包 m_items[3][0x68] 的额外 2 槽位；
                         //        send_chat_msg_hyper_link case 0xb/c/e/0x23 经 set() 传入
                         //        itemCount(MSG+0x13b)+items(MSG+0x13c)，set 本体不在语料；
                         //        ORIG ctor 仅 memset 0x68；monitor OnMonitorMegaPhoneMsgHyperLink
                         //        只写 m_sg 后整体转发、语料亦无 game 侧接收端 →
                         //        此区无成员级读写证据，保留 pad）
    char m_msgLen;  // +166（send_chat_msg_hyper_link: SendPacket 大小 = msgLen + 0x167，msg 起点 +0x167）
    char m_msg[0x100];  // +167..+266（同上；Guild/Member/Other_Channel 家族包同型：msgLen + msg[0x100]）

    Packet_Monitor_MegaPhone_Hyper_Link() : PacketHeader(0x271b, 0x267),
        m_sg(0), m_channel(0), m_uniqueId(0), m_itemCount(0), m_msgLen(0)
    {
    memset(m_items, 0, sizeof(m_items));
    m_msg[0] = 0;
    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Monitor_MegaPhone_Hyper_Link, 0x267);


#endif //PACKET_MONITOR_MEGAPHONE_HYPER_LINK_H
