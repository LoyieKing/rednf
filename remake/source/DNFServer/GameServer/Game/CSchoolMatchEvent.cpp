// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CSchoolMatchEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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

#include "CSchoolMatchEvent/CSchoolMatchEvent.inc"


#include "CSchoolMatchEvent/dtor_CSchoolMatchEvent.inc"


#include "CSchoolMatchEvent/StartEvent.inc"


#include "CSchoolMatchEvent/EndEvent.inc"


#include "CSchoolMatchEvent/AppendInfo.inc"


#include "CSchoolMatchEvent/StartEvent_1.inc"
{
    // ORIG 0x08273fec。Word_Param 4 字节按字节视图读 4 个配置值：
    //   byte0 channel_no / byte1 player_count / byte2 school_rating / byte3 player_rating。
    // [b28c-27] N82/N93：两处 cMyTrace 全表达式临时链槽 ORIG -0x54/-0x44，4.4 恒
    // -0x50/-0x40（差 4；类内对齐选择跨版本差）；findValue 经 P212 提至函数头后
    // 已落 ORIG -0x30 独立槽。其余为 .o 重定位伪差（AE 口径）。
    SetEventFlag(true);

    struct Bytes { unsigned char b0, b1, b2, b3; };   // 组件视图折叠 ORIG 单 disp（P1）
    Bytes* p = (Bytes*)&param;
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
    // b28d-35：find 的 key 用内联实参（临时槽），不设具名 findValue 局部——
    //   具名局部使 4.4 把 trace 对象整体浅移 4（-0x50/-0x40 vs ORIG -0x54/-0x44）。
    std::vector<int>::const_iterator it =
        std::find(m_channels.begin(), m_channels.end(), (int)p->b0);
    if (it == m_channels.end()) {
        m_channels.push_back((int)p->b0);
    }
}
