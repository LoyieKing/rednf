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

// CParty::m_dungeon（+0xcac，CParty.h private）以本地偏移视图读取：
// ORIG traceEnd 直读 party+0xcac（_ZN5CUser8GetPartyEv 后 mov 0xcac(%eax)），
// 偏移与 CParty.h 建模一致（+0xcac CDungeon* m_dungeon）。
struct CPartyDungeonView
{
    char m_pad[0xcac];
    CDungeon* m_dungeon;
};

stMonsterDamageLog_t::stMonsterDamageLog_t()
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

CTraceMobDieHack::CTraceMobDieHack()
{
    // m_map1/m_map2（map）与 m_mobKillers/m_hackLogs（vector）由编译器
    // 按成员声明序隐式构造，随后执行本函数体（ORIG 086c32e0 同序）。
    reset();
}

CTraceMobDieHack::~CTraceMobDieHack()
{
    // 成员按声明逆序隐式析构（vector<stHackLog_t> → vector<int> → map → map）。
}

void CTraceMobDieHack::reset()
{
    m_map1.clear();
    m_traceBeginTime = 0;
    m_mobKillers.clear();
    m_hackLogs.clear();
    m_bTracing = 0;
    m_dungeonDiff = 5;
    for (int i = 0; i < 4; ++i)
    {
        m_users[i] = 0;
        m_damage[i] = 0;
        m_count[i] = 0;
    }
}

void CTraceMobDieHack::traceBegin(long t, bool b)
{
    m_traceBeginTime = t;
    m_bTracing = 1;
    m_bInspectUser = b;
}

void CTraceMobDieHack::traceEnd(CUser* killer, CUser** users)
{
    if (m_bTracing != 1)
    {
        return;
    }
    long curSec = GlobalData::s_systemTime_.getCurSec();
    m_bTracing = 0;
    if (killer == 0)
    {
        return;
    }
    CParty* party = (CParty*)killer->GetParty();
    if (party == 0 || ((CPartyDungeonView*)party)->m_dungeon == 0)
    {
        return;
    }
    CDungeon* dungeon = ((CPartyDungeonView*)party)->m_dungeon;
    int dungeonIndex = dungeon->get_index();
    int specialDungeons[4] = {1, 2, 0x29, 0x21};
    for (int i = 0; i < 4; ++i)
    {
        if (specialDungeons[i] == dungeonIndex)
        {
            return;
        }
    }
    for (int j = 0; j < 4; ++j)
    {
        CUser* u = users[j];
        if (u == 0 || u->getCurCharacR() == 0 ||
            (party = (CParty*)u->GetParty()) == 0)
        {
            continue;
        }
        int level = u->get_charac_level();
        int stdLevel = ((CPartyDungeonView*)party)->m_dungeon->get_standard_level();
        if (level > stdLevel + 10)
        {
            continue;
        }
        m_users[j] = u;
        m_damage[j] += (int)(curSec - m_traceBeginTime);
        m_count[j] += 1;
    }
}

int CTraceMobDieHack::addMonsterDamage(int nMonsterIndex, unsigned short monsterId, int damage)
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
        stMonsterDamageLog_t& entry = m_map1[monsterId];
        if (entry.m_damage == damage)
        {
            entry.m_count += 1;
        }
        else
        {
            entry = log;
        }
    }
    return m_map1[monsterId].m_count;
}

void CTraceMobDieHack::setMobKiller(int killer)
{
    m_mobKillers.push_back(killer);
}

void CTraceMobDieHack::setDungeonDiff(int diff)
{
    m_dungeonDiff = diff;
}

void CTraceMobDieHack::reportHackInfo()
{
    if (GlobalData::s_villageMonsterMgr->OnEvent())
    {
        reset();
        return;
    }
    for (int i = 0; i < 4; ++i)
    {
        if (m_users[i] == 0 || m_count[i] <= 1)
        {
            continue;
        }
        int avg = m_damage[i] / m_count[i];
        if (m_dungeonDiff != 5 && m_bInspectUser != 1)
        {
            int limit = 10;
            if (m_dungeonDiff == 0)
            {
                limit = 5;
            }
            else if (m_dungeonDiff == 1)
            {
                limit = 6;
            }
            else if (m_dungeonDiff == 2)
            {
                limit = 7;
            }
            else if (m_dungeonDiff == 3)
            {
                limit = 8;
            }
            if (avg <= limit)
            {
                WongWork::CHackAnalyzer* analyzer =
                    m_users[i]->getHackAnalyzer();
                analyzer->addServerHackCnt(
                    m_users[i], (WongWork::ENUM_HACKTYPE)0x19e,
                    (unsigned int)(limit - avg + 1), 0, 0);
            }
        }
        if (m_bInspectUser == 0)
        {
            if (avg < 0x14)
            {
                WongWork::CHackAnalyzer* analyzer =
                    m_users[i]->getHackAnalyzer();
                analyzer->addServerHackCnt(
                    m_users[i], (WongWork::ENUM_HACKTYPE)0x193, 1, 0, 0);
            }
        }
        else if (avg < 10)
        {
            WongWork::CHackAnalyzer* analyzer =
                m_users[i]->getHackAnalyzer();
            analyzer->addServerHackCnt(
                m_users[i], (WongWork::ENUM_HACKTYPE)0x194, 1, 0, 0);
        }
    }
    reset();
}

void CTraceMobDieHack::_collect(CUser** users, int param_2)
{
    for (int i = 0; i < 4; ++i)
    {
        CUser* u = users[i];
        if (u == 0)
        {
            continue;
        }
        if (u->IsGameMasterMode())
        {
            continue;
        }
        CParty* party = (CParty*)u->GetParty();
        if (party == 0 || ((CPartyDungeonView*)party)->m_dungeon == 0)
        {
            continue;
        }
        int level = u->get_charac_level();
        if (level <= 10)
        {
            continue;
        }
        int diff = level -
                   ((CPartyDungeonView*)u->GetParty())->m_dungeon->get_standard_level();
        if (diff > 5)
        {
            continue;
        }
        stHackLog_t log;
        log.m_accId = u->get_acc_id();
        log.m_characNo = u->getCurCharacNo();
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
