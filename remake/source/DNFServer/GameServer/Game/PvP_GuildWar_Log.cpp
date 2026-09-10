// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 PvP_GuildWar_Log/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "LogManager.h"
// ============================================================================
// df_game_r PvP_GuildWar_Log（G2-4 公会战 PvP 日志，0x68 字节）
// 逐函数对照 docs/class_func_reports/PvP_GuildWar_Log.md 与 ORIG 反汇编实现；
// 布局：+0x00 char m_guildCnt、+0x04 int m_startTime、+0x08 STPvPGuild[8]（0xc 每项）。
// 注意：PvP_deps.h 桩签名 SetpvpStartTime(long long) 与 ORIG mangling El（long）
// 不符，本 TU 以 ORIG 为准（long）；STPvPGuild ctor 一并实现（ORIG 085df7d0 弱）。
// ============================================================================

#include <cstdio>
#include <cstring>

#include "CSystemTime.h"
#include "CUser.h"           // CUser / CUserCharacInfo（基类，offset 0）
#include "CEnvironment.h"    // G_CEnvironment() / get_file_name()

// ---- CMyFileLog / STPvPGuild / PvP_GuildWar_Log：唯一声明点为权威头 ----
//   CMyFileLog.h（ctor/operator() 由 ServerCommon/DNFFileLog.cpp、CMyFileLog.cpp 提供）
//   PvPTypes.h（STPvPGuild 0xc / PvP_GuildWar_Log 0x68 权威布局，本 TU 提供方法实现）
#include "CMyFileLog.h"
#include "PvPTypes.h"

// ORIG _ZN10STPvPGuildC1Ev 0x085df7d0 W：三字段清零
#include "PvP_GuildWar_Log/STPvPGuild.inc"
{
    m_guildKey = 0;
    m_point = 0;
    m_count = 0;
}

#include "PvP_GuildWar_Log/PvP_GuildWar_Log.inc"
    : m_startTime(0)
{
    // b28d-M2S3（推翻旧 N 判词）：ORIG 0x085dda4b 为 `sub $0x10`。
    // 帧-死局部穷举（4.4.7 / 446 / 444 三后端读数一致）：空体 ⇒ `sub $0x4`；
    // char[4] / [8] / [0xc] ⇒ 恒 `sub $0x14`。⇒ 0x10 不可达（4.4 帧只有
    // 0x4/0x14 两档）⇒ 保留最小死局部取最近帧（差 4B，N82 帧量化族）。
    // [N1B 2026-09-11 复核·帧量化规则] /tmp/frametest{,2}.cpp 扫 14 档（1/2/3/4/5/
    // 6/7/8/9/10/12/16/20 字节 + double + long double，同 TU 旗标）：空体 ⇒ 0x4；
    // 1..16 字节 ⇒ 0x14；20 字节 ⇒ 0x24 = 4 + round_up(locals,16) ⇒ 帧值集合为
    // {4} ∪ {4+16k}，**0x10 结构性不可达**（需 round_up=0xc）。ORIG 4.1 的 0x10 属
    // 跨版本帧量化差（N82），本件唯一残差（25/25 指令逐条同形、仅 sub/add 立即数
    // 0x10 vs 0x14）⇒ NEAR 封顶。
    char framePad[4];
    (void)framePad;
}

#include "PvP_GuildWar_Log/dtor_PvP_GuildWar_Log.inc"
{
    Clear();
}

#include "PvP_GuildWar_Log/Clear.inc"
{
    m_guildCnt = 0;
    m_startTime = 0;
    memset(m_guilds, 0, sizeof(m_guilds));
}

#include "PvP_GuildWar_Log/SetpvpStartTime.inc"
{
    m_startTime = t;
}

// ORIG _ZN16PvP_GuildWar_Log14InsertPvPGuildEji 0x085ddd80：
//   for(i=0;i<=7;++i){ key==0→跳到插入；key==guildKey→累加返回 1 } 未命中返回 0。
// [b28d-23 N·P223 → b28d-M2S3 推翻] ORIG 0x085ddd80：goto 岛形 → 自然
// for + if/else 直落后复测 108/108、归一化零差异（identical）。
#include "PvP_GuildWar_Log/InsertPvPGuild.inc"
{
    // [b28c-L18] 真语义 bug 修复：ORIG 0x085dddcc `cmp 0xc(%ebp),%eax; jne <++i 块>`
    // 实证 key 既非 0 亦非 guildKey 时**继续扫描下一槽**；旧源 mismatch 直落
    // found 插入块（首个非命中槽即被覆盖，槽 1..7 永不可达）。
    // b28d-M2S3（P164/P223 重试，推翻旧 N 判词）：ORIG 0x085ddd80 物理块序 =
    // body(+0x13) → insert 块(+0xb3) → ++i 块(+0x110) → 尾条件(+0x114)。
    // 旧 goto 岛形把 ++i 块插在 insert 之前，且入口 `jmp` 后多 1 条 label
    // 对齐 nop（109 vs ORIG 108）。自然 for + if/else 直落成 ORIG 块序。
    for (int i = 0; i <= 7; ++i)
    {
        if (m_guilds[i].m_guildKey != 0)
        {
            if (m_guilds[i].m_guildKey == guildKey)
            {
                m_guilds[i].m_point += point;
                ++m_guilds[i].m_count;
                return 1;
            }
        }
        else
        {
            m_guilds[i].m_guildKey = guildKey;
            m_guilds[i].m_point = point;
            m_guilds[i].m_count = 1;
            ++m_guildCnt;
            return 1;
        }
    }
    return 0;
}

// [b28d-23 N·N82 → b28d-M2S3 复测] ORIG 0x085ddad0：192/192，归一化后仅 8 处
// 「参数压栈立即数」差（.o 未链接值 vs ORIG .rodata 地址的显示差）⇒ identical 级。
#include "PvP_GuildWar_Log/WriteGuildWarPvPLog.inc"
{
    // [L14 round3] N78 突破口（P214③/P253）：ORIG 0x085ddad0 两处清零是**内联
    // `rep stos`**（0x85ddb5b 0x800 字节 / 0x85ddcbd 0x100 字节）而非 `call memset`；
    // 4.4 对 `memset` 恒发 call（p167 EXTRA +2 memset）。改「声明处 `= {'\0'}`」
    // 并把声明下移到原 memset 位置，令 4.4 在同一位置发常量 rep stos。
    int i;
    int j;
    int k;

    for (i = 0; i < 8; ++i)
    {
        if (users[i] != 0)
        {
            // b28d-M2S3（P0/P183）：ORIG 0x085ddb0b `call get_guildwar_point_
            // per_pvpplay; mov %eax,%ebx`（无 point 具名槽）+ 0x085ddb20
            // `call get_charac_guildkey`（无 guildKey 槽）；两具名局部多余。
            // cdecl 实参右→左求值 ⇒ ORIG 先算第 2 参 point 存 %ebx。
            InsertPvPGuild(
                ((CUserCharacInfo*)users[i])->get_charac_guildkey(),
                users[i]->get_guildwar_point_per_pvpplay());
        }
    }
    char logBuf[2048] = { '\0' };
    sprintf(logBuf, "Room_Idx:%d\tPvP_Mode:%d\tElasp_Time:%d\tG_Cnt:%d\n",
            roomIdx, (int)mode,
            GlobalData::s_systemTime_.getCurSec() - m_startTime,
            (unsigned char)m_guildCnt);
    // b28d-M2S3（P222/P164 块序）：ORIG 0x085ddbae `cmp $0x8,%al; jbe <then 块
    // +0x126>` ⇒ else(logFormat) 块**物理前置**、尾部 `jmp <epilogue>`（0x085ddbf1）
    // ⇒ 源码是早返回形（`>= 9` 打印后 return），then 块被跳到后面。
    // 旧源 `if (< 9) {...} else {...}` 令 then 直落、else 跳后（我方 `ja`）。
    if ((unsigned char)m_guildCnt >= 9)
    {
        LogManager::logFormat(
            1, "pvp.cpp",
            __PRETTY_FUNCTION__,
            0x12cc,
            "PvP_GuildWar_Log::WriteGuildWarPvPLog() m_guildCnt(%d) > MAX_PVP_PLAYER(8) ",
            (unsigned char)m_guildCnt);
        return;
    }
    // b28d-M2S3：ORIG 槽序 i(-0x24)/j(-0x20)/k(-0x1c) 连续 ⇒ j 用函数头
    // 已声明的外层 j（旧源 for 内再声明遮蔽，令 j 槽落到 k 之后）。
    for (j = 0; j < (unsigned char)m_guildCnt; ++j)
    {
        if (m_guilds[j].m_guildKey != 0)
        {
            sprintf(logBuf, "%s->\t\tG_Key:%d\tPoint:%d\tM_Cnt:%d\n", logBuf,
                    m_guilds[j].m_guildKey, m_guilds[j].m_point,
                    (unsigned char)m_guilds[j].m_count);
        }
    }
    char logPath[256] = { '\0' };
    sprintf(logPath, "./log/%s/GuildWarResult",
            G_CEnvironment()->get_file_name());
    // b28d-M2S3（P139/§6.5）：ORIG 0x085ddcf6 实参 =
    // `&_ZZN16PvP_GuildWar_Log19WriteGuildWarPvPLogEicPP5CUserE19__
    // PRETTY_FUNCTION__` 具名局部静态数组，非手写串。
    CMyFileLog fileLog(__PRETTY_FUNCTION__, 0x12dc);
    fileLog(logPath, "%s", logBuf);
    Clear();
    for (k = 0; k < 8; ++k)
    {
        if (users[k] != 0)
        {
            users[k]->set_guildwar_point_per_pvpplay(0);
        }
    }
}
