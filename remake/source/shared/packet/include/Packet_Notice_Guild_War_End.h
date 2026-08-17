//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_NOTICE_GUILD_WAR_END_H
#define PACKET_NOTICE_GUILD_WAR_END_H

#include "PacketHeader.h"

class Packet_Notice_Guild_War_End : public PacketHeader {
public:
    unsigned char m_channel;  // +0xa（2026-08-16 深挖定名：唯一调用点 CGuildWarEvent::EndEvent
                              //      （0x811d28c，game 侧）在 GetChannelType()==6（公会战频道）时传
                              //      CEnvironment::get_channel_no()（CEnvironment+0x1b0，0x814aaca），
                              //      即公会战结束事件的频道号；SendGuildWarStart 用同一字节，
                              //      CMonitorServerProxy::SendGuildWarEnd（0x84710a8）同构。
                              //      guild ORIG OnNoticeGuildWarEnd（0x8076258）movzbl 0xa →
                              //      SetGuildWarEvent(false, param) → CGuildWar::m_bParam(+0xf)：
                              //      df_guild_r 全二进制仅 ctor/Init/SetGuildWarEvent 写入，无读取点
                              //      （写而不读），故语义只能按发送端 get_channel_no() 定名）
    Packet_Notice_Guild_War_End() : PacketHeader(0x412, 0xb){};
} __attribute__((packed));

#endif  // PACKET_NOTICE_GUILD_WAR_END_H
