//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_CHAR_CHANGABLE_INFO_H
#define PACKET_MONITOR_CHAR_CHANGABLE_INFO_H

#include "PacketHeader.h"

class Packet_Monitor_Char_Changable_Info : public PacketHeader {
public:
    unsigned int m_dbid;            // +0xa  FindUser
    char m_padE;                    // +0xe（第三轮穷举 2026-08-16：SendCharLevelGrowType 写
                                    //        param_2 低字节；df_guild_r ORIG
                                    //        OnUpdateChangableCharInfo 反汇编仅读 +0xf/+0x11
                                    //        （movzwl 0xf / movzbl 0x11），+0xe 未读；
                                    //        Monitor OnUpdateChangableCharInfo 也不读，
                                    //        无字段语义证据，保留）
    union {
        short m_guildMemFlag;       // +0xf（第三轮穷举定名：df_guild_r ORIG
                                    //        OnUpdateChangableCharInfo 读 +0xf 作
                                    //        SetUserChangableInfo(type)，guild CUser 实现
                                    //        m_guildMemFlag = type）
        short m_field_f;            // +0xf（旧占位名，Guild 还原代码引用）
        short m_level;              // +0xf（旧猜测名，Monitor 还原代码引用，保留兼容）
    };
    union {
        char m_growthType;          // +0x11（第三轮穷举定名：ORIG OnUpdateChangableCharInfo 读
                                    //        +0x11 作 SetUserChangableInfo(value)，guild CUser
                                    //        实现 m_growthType = value）
        char m_field_11;            // +0x11（旧占位名）
        char m_flag;                // +0x11（旧猜测名，Monitor 还原代码引用，保留兼容）
    };
    Packet_Monitor_Char_Changable_Info(): PacketHeader(0x3f5,0x12) {};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Monitor_Char_Changable_Info, 0x12);

#endif //PACKET_MONITOR_CHAR_CHANGABLE_INFO_H
