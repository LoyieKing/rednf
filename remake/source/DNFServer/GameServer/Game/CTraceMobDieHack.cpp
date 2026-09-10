// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CTraceMobDieHack/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r CTraceMobDieHack 还原（G2-5 批次）。
// CParty +0x210，总尺寸 0x94。符号（nm -C，W/T 全实现）：
//   C1 0x082a517c / D1 0x082a5236 / reset 0x082a50b0 /
//   traceBegin 0x085bf4cc / traceEnd 0x08599a88 /
//   addMonsterDamage 0x085bf4f6 / setMobKiller 0x085bf628 /
//   setDungeonDiff 0x085bf646 / reportHackInfo 0x08599cb2 /
//   _collect 0x08599f64；附带 stMonsterDamageLog_t C1 0x085bf4a2、
//   stHackLog_t::operator== 0x085bf472。
// 布局（由 ctor/reset/traceEnd/reportHackInfo 反汇编推导）：
//   +0x00 long m_traceBeginTime、+0x04 map<ushort,stMonsterDamageLog_t>[2]
//   （各 0x18）、+0x34 vector<int>（0xc）、+0x40 vector<stHackLog_t>（0xc）、
//   +0x4c char m_bTracing、+0x50 CUser* m_users[4]、+0x54 int m_damage[4]、
//   +0x58 int m_count[4]、+0x80 bool m_bInspectUser、+0x84 int m_dungeonDiff、
//   +0x88 保留。类/结构定义见 CTraceMobDieHack.h（真实 std::map/vector 成员，
// b28d-45 判词汇总（本 TU 1 件）：
//   traceEnd 0x08599a88 DIFF107 N82/N98：182:182 槽位集对齐；余 getCurSec
//     重定位显示（R_386_32）+ 帧槽差。
//   ctor/dtor 由编译器按成员序隐式构造/逆序析构，与 ORIG 一致）。
// 跨类调用一律 include 真实头，不再使用 extern asm 桥。

#include <algorithm>

#include "CTraceMobDieHack.h"
#include "CUser.h"              // CUser / CUserCharacInfo（基类）/ WongWork::CHackAnalyzer
#include "CDungeon.h"           // CDungeon::get_index / get_standard_level
#include "CParty.h"             // CParty::m_dungeon（+0xcac）
#include "CSystemTime.h"        // CSystemTime::getCurSec
#include "CVillageMonsterMgr.h" // village_attacked::CVillageMonsterMgr::OnEvent
#include "GlobalData.h"         // GlobalData::s_systemTime_ / s_villageMonsterMgr

// CParty::m_dungeon（+0xcac）在 CParty.h 为 public 数据成员，直接具名访问。

#include "CTraceMobDieHack/stMonsterDamageLog_t.inc"
{
    m_monsterId = 0;
    m_damage = 0;
    m_count = 0;
    m_monsterIndex = 0;
}

bool stHackLog_t::operator==(const stHackLog_t& other) const
{
    return m_accId == other.m_accId && m_characNo == other.m_characNo;
}

#include "CTraceMobDieHack/CTraceMobDieHack.inc"
{
    // m_map1/m_map2（map）与 m_mobKillers/m_hackLogs（vector）由编译器
    // 按成员声明序隐式构造，随后执行本函数体（ORIG 086c32e0 同序）。
    reset();
}

#include "CTraceMobDieHack/dtor_CTraceMobDieHack.inc"
{
    // 成员按声明逆序隐式析构（vector<stHackLog_t> → vector<int> → map → map）。
}

#include "CTraceMobDieHack/reset.inc"
{
    m_map1.clear();
    m_traceBeginTime = 0;
    m_mobKillers.clear();
    m_hackLogs.clear();
    m_bTracing = false;
    m_dungeonDiff = 5;
    for (int i = 0; i <= 3; ++i)
    {
        m_records[i].m_user = 0;
        m_records[i].m_damage = 0;
        m_records[i].m_count = 0;
    }
}

#include "CTraceMobDieHack/traceBegin.inc"
{
    m_traceBeginTime = t;
    m_bTracing = 1;
    m_bInspectUser = b;
}

#include "CTraceMobDieHack/traceEnd.inc"
{
    if (!m_bTracing)
    {
        return;
    }
    long curSec = GlobalData::s_systemTime_.getCurSec();
    m_bTracing = false;
    if (killer == 0 || killer->GetParty() == 0 ||
        ((CParty*)killer->GetParty())->m_battleField.m_pDungeon == 0)
    {
        return;
    }
    int dungeonIndex = ((CParty*)killer->GetParty())->m_battleField.m_pDungeon->get_index();
    int specialDungeons[4];
    specialDungeons[0] = 1;
    specialDungeons[1] = 2;
    specialDungeons[2] = 0x29;
    specialDungeons[3] = 0x21;
    for (unsigned int i = 0; i <= 3; ++i)
    {
        // [N6S5ZeroA] ORIG 0x08599b4b(+0xc3) 条件是 `mov -0x10(%ebp),%eax;
        //   cmp $0x3,%eax; setbe %al`，我方为 `cmpl $0x3,-0x10(%ebp); setbe`
        //   （gap=+1，ORIG 多一条装载）。穷举 30 形 × 三后端（4.4.7/4.4.6/4.4.4 +
        //   4.1 族 c5）全部生成 `cmpl $3,mem`；唯一生成 `mov;cmp $3,%eax` 的
        //   volatile 形与 ORIG 的 `addl $1,-0x10(%ebp)` 自增形互斥
        //   （volatile 自增为 load/add/store 三条）⇒ 源码不可控，判 N 封顶
        //   （编译器 lowering 差异；体/符号性/边界 setbe(unsigned) 均与 ORIG 一致）。
        if (specialDungeons[i] == dungeonIndex)
        {
            return;
        }
    }
    for (int j = 0; j <= 3; ++j)
    {
        if (users[j] == 0 || users[j]->getCurCharacR() == 0 ||
            users[j]->GetParty() == 0)
        {
            continue;
        }
        if (users[j]->get_charac_level() >
            ((CParty*)users[j]->GetParty())->m_battleField.m_pDungeon->get_standard_level() + 10)
        {
            continue;
        }
        m_records[j].m_user = users[j];
        m_records[j].m_damage += (int)(curSec - m_traceBeginTime);
        // [N6S5ZeroA] ORIG 0x08599c5b(+0x1d3) 块首为 `mov -0xc(%ebp),%edx;
        //   mov 0x8(%ebp),%ecx; mov %edx,%eax`（edx 版，this→ecx）；`m_count += 1`
        //   曾生成 `mov j,%edx; mov j,%ecx; mov this,%ebx; mov %ecx,%eax`（多一条
        //   P231 纯寄存器复本 + this→ebx）。改后置/前置自增形为 `++` 后，
        //   我方指令形与 ORIG 逐条一致（gap -1→+1，余仅 i 循环那一条）。
        ++m_records[j].m_count;
    }
}

#include "CTraceMobDieHack/addMonsterDamage.inc"
{
    stMonsterDamageLog_t log;
    log.m_monsterId = monsterId;
    log.m_damage = (unsigned short)damage;
    log.m_count = 1;
    log.m_monsterIndex = nMonsterIndex;

    // map 的 value_type：pair<const unsigned short, stMonsterDamageLog_t>
    std::pair<std::map<unsigned short, stMonsterDamageLog_t>::iterator, bool>
        result = m_map1.insert(std::make_pair(monsterId, log));
    if (!result.second)
    {
        if (m_map1[monsterId].m_damage == damage)
        {
            m_map1[monsterId].m_count = m_map1[monsterId].m_count + 1;
        }
        else
        {
            m_map1[monsterId] = log;
        }
    }
    return m_map1[monsterId].m_count;
}

#include "CTraceMobDieHack/setMobKiller.inc"
{
    m_mobKillers.push_back(killer);
}

#include "CTraceMobDieHack/setDungeonDiff.inc"
{
    m_dungeonDiff = diff;
}

#include "CTraceMobDieHack/reportHackInfo.inc"
{
    if (GlobalData::s_villageMonsterMgr->OnEvent())
    {
        reset();
        return;
    }
    for (int i = 0; i <= 3; ++i)
    {
        if (m_records[i].m_user == 0 || m_records[i].m_count <= 1)
        {
            continue;
        }
        int avg = m_records[i].m_damage / m_records[i].m_count;
        if (m_dungeonDiff != 5 && !m_bInspectUser)
        {
            int limit = 10;
            switch (m_dungeonDiff)
            {
            case 0:
                limit = 5;
                break;
            case 1:
                limit = 6;
                break;
            case 2:
                limit = 7;
                break;
            case 3:
                limit = 8;
                break;
            default:
                break;
            }
            if (avg <= limit)
            {
                unsigned int extra = (unsigned int)(limit - avg);
                m_records[i].m_user->getHackAnalyzer()->addServerHackCnt(
                    m_records[i].m_user, (WongWork::ENUM_HACKTYPE)0x19e,
                    extra + 1, 0, 0);
            }
        }
        if (m_bInspectUser)
        {
            if (avg < 10)
            {
                m_records[i].m_user->getHackAnalyzer()->addServerHackCnt(
                    m_records[i].m_user, (WongWork::ENUM_HACKTYPE)0x194, 1, 0, 0);
            }
        }
        else
        {
            if (avg < 0x14)
            {
                m_records[i].m_user->getHackAnalyzer()->addServerHackCnt(
                    m_records[i].m_user, (WongWork::ENUM_HACKTYPE)0x193, 1, 0, 0);
            }
        }
    }
    reset();
}

#include "CTraceMobDieHack/_collect.inc"
{
    for (int i = 0; i <= 3; ++i)
    {
        if (users[i] == 0)
        {
            continue;
        }
        if (users[i]->IsGameMasterMode())
        {
            continue;
        }
        if (users[i]->GetParty() == 0 ||
            ((CParty*)users[i]->GetParty())->m_battleField.m_pDungeon == 0)
        {
            continue;
        }
        if (users[i]->get_charac_level() <= 10)
        {
            continue;
        }
        int diff = users[i]->get_charac_level() -
                   ((CParty*)users[i]->GetParty())->m_battleField.m_pDungeon->get_standard_level();
        if (diff > 5)
        {
            continue;
        }
        stHackLog_t log;
        log.m_accId = users[i]->get_acc_id();
        log.m_characNo = users[i]->getCurCharacNo();
        log.m_hackType = 0x193;
        log.m_value = param_2;
        log.m_count = 1;
        std::vector<stHackLog_t>::iterator found =
            std::find(m_hackLogs.begin(), m_hackLogs.end(), log);
        if (found == m_hackLogs.end())
        {
            m_hackLogs.push_back(log);
        }
        else
        {
            found->m_count += 1;
        }
    }
}
