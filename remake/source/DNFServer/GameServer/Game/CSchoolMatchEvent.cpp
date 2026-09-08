// CSchoolMatchEvent —— 学院匹配 repeat 事件实现。
// 全部函数逐段对齐 docs/class_func_reports/CSchoolMatchEvent/ 下 7 份报告：
//   CSchoolMatchEvent.md(0x08273ecc ctor) / ~CSchoolMatchEvent.md(0x08273f1a) /
//   ~CSchoolMatchEvent_08273f7c.md(0x08273f7c) / StartEvent.md(0x08273f9a) /
//   StartEvent_08273fec.md(0x08273fec) / EndEvent.md(0x0827423c) /
//   AppendInfo.md(0x082742a8)。
#include "CSchoolMatchEvent.h"

#include <algorithm>

#include "CEnvironment.h"   // G_CEnvironment / get_channel_no
#include "GameWorld.h"      // G_GameWorld / GameWorld::GetChannelType
#include "CLeagueMatch.h"   // CLeagueMatch::SetMatchAble/SetPlayCountPerOneTeam/SetBattleMode
#include "OnlinePreliminary.h"  // PvPGlobalEnvironmentVariable::school_match_elo_k_/k2_
#include "GameTypes.h"      // cMyTrace
#include "PacketGuard.h"    // PacketGuard（AppendInfo 参数完整类型）

CSchoolMatchEvent::CSchoolMatchEvent()
{
    // ORIG 0x08273ecc：CEventBase 基类构造 + vptr + m_channels vector 构造（编译器生成）。
}

CSchoolMatchEvent::~CSchoolMatchEvent()
{
    // ORIG 0x08273f1a：写 vptr 后 m_channels 析构 + CEventBase 析构（编译器生成）。
}

void CSchoolMatchEvent::StartEvent()
{
    // ORIG 0x08273f9a：SetEventFlag(true) + 日志。
    SetEventFlag(true);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x15, 0);
    trace("CSchoolMatchEvent Start!");
}

void CSchoolMatchEvent::EndEvent()
{
    // ORIG 0x0827423c：SetEventFlag(false) + 联赛匹配关闭 + 日志 + 清频道表。
    SetEventFlag(false);
    CLeagueMatch::SetMatchAble(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x3f, 0);
    trace("CSchoolMatchEvent End!");
    m_channels.clear();
}

void CSchoolMatchEvent::AppendInfo(PacketGuard& packet)
{
    // ORIG 0x082742a8：put_byte(频道数) 后遍历 put_short 各频道号。
    packet.put_byte((int)m_channels.size());
    for (std::vector<int>::iterator it = m_channels.begin();
         it != m_channels.end(); ++it) {
        int value = *it;   // ORIG 经 -0xc 槽中转（Ghidra local_10）
        packet.put_short(value);
    }
}

void CSchoolMatchEvent::StartEvent(Word_Param param)
{
    // ORIG 0x08273fec。Word_Param 4 字节按字节视图读 4 个配置值：
    //   byte0 channel_no / byte1 player_count / byte2 school_rating / byte3 player_rating。
    // [b28c-27] N82/N93：两处 cMyTrace 全表达式临时链槽 ORIG -0x54/-0x44，4.4 恒
    // -0x50/-0x40（差 4；类内对齐选择跨版本差）；findValue 经 P212 提至函数头后
    // 已落 ORIG -0x30 独立槽。其余为 .o 重定位伪差（AE 口径）。
    SetEventFlag(true);

    struct Bytes { unsigned char b0, b1, b2, b3; };   // 组件视图折叠 ORIG 单 disp（P1）
    Bytes* p = (Bytes*)&param;
    int findValue;                                    // P212：ORIG -0x30 独立槽
                                                      // （块内声明会被 4.4 复用 trace 槽）
    cMyTrace(__PRETTY_FUNCTION__, 0x1e, 0)
        ("CSchoolMatchEvent Start! channel_no(%d), player_count(%d), school_rating(%d), player_rating(%d)",
         (unsigned int)p->b0, (unsigned int)p->b1,
         (unsigned int)p->b2, (unsigned int)p->b3);

    // 仅学院频道（channel type 9/10）启动联赛匹配。
    if (G_GameWorld()->GetChannelType() == 9 ||
        G_GameWorld()->GetChannelType() == 10) {
        if (G_CEnvironment()->get_channel_no() == (int)p->b0) {
            CLeagueMatch::SetMatchAble(true);
            CLeagueMatch::SetPlayCountPerOneTeam((unsigned int)p->b1);
            PvPGlobalEnvironmentVariable::school_match_elo_k2_ = (int)p->b2;
            PvPGlobalEnvironmentVariable::school_match_elo_k_ = (int)p->b3;
            if (G_CEnvironment()->get_channel_no() == 0x47) {
                CLeagueMatch::SetBattleMode(2);
            }
            cMyTrace(__PRETTY_FUNCTION__, 0x30, 0)
                ("This is School Matching Server!!");
        }
    }

    // 当前频道号不在表内才登记。
    findValue = (int)p->b0;
    std::vector<int>::const_iterator it =
        std::find(m_channels.begin(), m_channels.end(), findValue);
    if (it == m_channels.end()) {
        m_channels.push_back((int)p->b0);
    }
}
