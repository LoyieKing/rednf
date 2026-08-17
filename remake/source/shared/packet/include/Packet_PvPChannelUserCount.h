//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_PVPCHANNELUSERCOUNT_H
#define PACKET_PVPCHANNELUSERCOUNT_H

#include "PacketHeader.h"

class Packet_PvPChannelUserCount : public PacketHeader {
public:
    unsigned int m_charNo;          // +0xa  FindUser_CharNo
    unsigned int m_uid;             // +0xe（monitor 自 Packet_PvPChannelInfo::m_uid 转发；
                                    //        df_game_r Dispatcher_PvPChannelInfo: CUser::GetUID）
    unsigned int m_schoolNo;        // +0x12（monitor 自 Packet_PvPChannelInfo::m_schoolNo 转发；
                                    //        df_game_r: CUser::GetSchoolNo）
    int m_countA;                   // +0x16  SetChannelUserCount 第 1 参（频道号）
    int m_countB;                   // +0x1a  SetChannelUserCount 第 2 参
    int m_pad1e;                    // +0x1e（第三轮穷举 2026-08-16：语料无 0xfa3 发送/接收函数节
                                    //        （类名与 ID 均未出现，仅一条无关日志串）；
                                    //        monitor ORIG OnPvPChannelUserCount 反汇编读
                                    //        +0xa/+0xe/+0x12/+0x16/+0x1a/+0x22/+0x26，
                                    //        未读 +0x1e；无证据，保留）
    int m_countC;                   // +0x22  SetChannelUserCount 第 3 参
    int m_countD;                   // +0x26  SetChannelUserCount 第 4 参
    Packet_PvPChannelUserCount() : PacketHeader(0xfa3, 0x2a){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_PvPChannelUserCount, 0x2a);

#endif  // PACKET_PVPCHANNELUSERCOUNT_H
