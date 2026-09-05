//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_PVPRESULT_H
#define PACKET_PVPRESULT_H

#include "PacketHeader.h"

// 载荷布局依据 ORIG 生产者 df_game_r 0x0855eb22
// WongWork::CLeagueMatch::OnReward 的逐指令偏移（docs/class_func_reports/
// CLeagueMatch/OnReward.md，汇编段）：
//   `movl %eax,0xa(%ebx)` 4B → m_eloK；`movb ...,0xe(%ebx)`/`0xf(%ebx)` 1B →
//   m_winType / m_playCount；每队块 stride 0x38（team0@+0x10、team1@+0x48，
//   asm 统一寻址 pkt + 0x10 + i + 3j + 56t）：块内 +0x00 每日徽章低字节
//   （`mov %bl,(%eax)`，i=0..2 来自 `movzwl 0x10(%ebx,%eax,2)`＝PlayerInfo
//   m_badges[i]）、+0x0c 新获旗标（`movb $0x0,0xc(%eax)`）、+0x18 schoolNo
//   int[4]（4B）、+0x28 accId uint[4]（4B）。
// 注意：DB 侧 pvp_result.play_count 列是 4B int（SIG_LOAD_ETC+0x3a4，
// GetUserPvp.md），与此处包内 1B m_playCount 是不同实体，不要互相“对齐”。
class Packet_PvPResult : public PacketHeader {
public:
    struct Team {                          // 单队结果块（0x38）
        unsigned char m_dailyBadge[4][3];  // +0x00 每队员 3 枚每日徽章计数
        unsigned char m_newFlag[4][3];     // +0x0c 每队员 3 字节新获旗标
        unsigned int m_schoolNo[4];        // +0x18 学院编号（写入源 PlayerInfo::m_schoolNo 为 int）
        unsigned int m_accId[4];           // +0x28 账号 ID
    } __attribute__((packed));             // sizeof 0x38

    Packet_PvPResult() : PacketHeader(0xfa5, 0x80){};

    int m_eloK;         // +0x0a CEloRating::calc 第 4 参（school_match_elo_k2_）
    char m_winType;     // +0x0e 0=负 1=平 2=胜
    char m_playCount;   // +0x0f nPlayerCountPerOneTeam_
    Team m_team[2];     // +0x10 / +0x48
} __attribute__((packed));

// 布局自检（C++98）：锁死 0x80 包尺寸与 0x38 队块尺寸。
typedef char Packet_PvPResult_size_0x80[
    (sizeof(Packet_PvPResult) == 0x80) ? 1 : -1];
typedef char Packet_PvPResult_Team_size_0x38[
    (sizeof(Packet_PvPResult::Team) == 0x38) ? 1 : -1];

#endif  // PACKET_PVPRESULT_H
