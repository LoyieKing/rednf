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
    // [b28d-23 N 封顶] ORIG 0x085dda4b `sub $0x10`（0xc 局部 + 4 outgoing）。
    // P232 补 char[0xc] 后 4.4 发 `sub $0x14`（实测）：4.4 带 call 帧恒 16k+4
    // 量化（P190 附注 b28 BL11：0x4/0x14/0x24…），ORIG 的 0x10 非量化值不可达；
    // 保留 0xc 死局部取最近帧（差 4），语义等价（framePad 不读写）。
    char framePad[0xc];
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
// [b28d-23 N·P223] 岛形/块序（108/109）⇒ N。ORIG 0x085ddd80
#include "PvP_GuildWar_Log/InsertPvPGuild.inc"
{
    // [b28c-L18] 真语义 bug 修复：ORIG 0x085dddcc `cmp 0xc(%ebp),%eax; jne <++i 块>`
    // 实证 key 既非 0 亦非 guildKey 时**继续扫描下一槽**；旧源 mismatch 直落
    // found 插入块（首个非命中槽即被覆盖，槽 1..7 永不可达）。
    // 块布局照 ORIG（P164 岛标签法）：入口 jmp 尾条件块；body/insert 物理前置、
    // `addl/cmpl/setle/test/jne` 回边块 + `mov $0` 直落 epilogue。
    int i = 0;
    goto cond;
body:
    if (m_guilds[i].m_guildKey == 0)
        goto found;
    if (m_guilds[i].m_guildKey == guildKey)
    {
        m_guilds[i].m_point += point;
        ++m_guilds[i].m_count;
        return 1;
    }
    ++i;
    goto insertblk;
found:
    m_guilds[i].m_guildKey = guildKey;
    m_guilds[i].m_point = point;
    m_guilds[i].m_count = 1;
    ++m_guildCnt;
    return 1;
insertblk:
cond:
    if (i <= 7)
        goto body;
    return 0;
}

// [b28d-23 N·N82] 192/191 ⇒ N。ORIG 0x085ddad0
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
            int point = users[i]->get_guildwar_point_per_pvpplay();
            unsigned int guildKey =
                ((CUserCharacInfo*)users[i])->get_charac_guildkey();
            InsertPvPGuild(guildKey, point);
        }
    }
    char logBuf[2048] = { '\0' };
    sprintf(logBuf, "Room_Idx:%d\tPvP_Mode:%d\tElasp_Time:%d\tG_Cnt:%d\n",
            roomIdx, (int)mode,
            GlobalData::s_systemTime_.getCurSec() - m_startTime,
            (unsigned char)m_guildCnt);
    if ((unsigned char)m_guildCnt < 9)
    {
        for (int j = 0; j < (unsigned char)m_guildCnt; ++j)
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
        CMyFileLog fileLog(
            "void PvP_GuildWar_Log::WriteGuildWarPvPLog(int, char, CUser**)",
            0x12dc);
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
    else
    {
        LogManager::logFormat(
            1, "pvp.cpp",
            __PRETTY_FUNCTION__,
            0x12cc,
            "PvP_GuildWar_Log::WriteGuildWarPvPLog() m_guildCnt(%d) > MAX_PVP_PLAYER(8) ",
            (unsigned char)m_guildCnt);
    }
}
