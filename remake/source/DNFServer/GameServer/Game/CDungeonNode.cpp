// df_game_r CDungeonNode 族实现（CDungeonNode 批次独占，2026-08-28）。
// 报告：docs/class_func_reports/CDungeonNode/{InsertDB,InsertMonsterKey,InsertSkillKey,
// UpdateCharacter,UpdateMonster,UpdateSkill}.md（已完整读两段）+
// docs/class_func_reports/CDungeonJobNode/{InsertDB,InsertSkillKey,UpdateSkill}.md +
// docs/class_func_reports/CDungeonSuccessNode/{CDungeonSuccessNode,InsertDB,UpdateCharacter}.md +
// docs/class_func_reports/CSuccessJobNode/{CSuccessJobNode,InsertDB,UpdateCharacter}.md。
// ctor/dtor/copy 报告缺失 → §3.2 nm 定址 + objdump 转写（见头文件注释）。
// CUser.h 冻结：PlayInfo::operator+= 定义按 Main 授权落本 TU（ORIG W 0x084866e8）。

#include "CDungeonNode.h"
#include "CDungeonExpStatistic.h"  // CDungeonExpStatistic C1（ORIG W 0x082aa88a）
#include "SigTypes.h"
#include "MySQL.h"
#include "CEnvironment.h"
#include "GameTypes.h"   // cMyTrace
#include "STGrowthType.h"
#include "GlobalData.h"
#include "DBMgr.h"

MySQL* GetMySqLInstance()
{
    MySQL* sql = GlobalData::s_db_mgr->GetDBHandle(
        static_cast<ENUM_DB_HANDLE_IDX>(4),
        static_cast<ENUM_SERVER_GROUP>(0));
    return sql;
}

// ============================================================================
// PlayInfo::operator+=(PlayInfo const&) —— ORIG W 0x084866e8：
// +0x00..+0x34 逐 int 相加（跳过 0x2c/0x38 char 与 0x38 后 pad；m_f3c 不加）。
// 定义归本 TU（Main 授权；声明已在 CUser.h）。
// ============================================================================
void PlayInfo::operator+=(const PlayInfo& other)
{
    m_f00 += other.m_f00;
    m_f04 += other.m_f04;
    m_f08 += other.m_f08;
    m_f0c += other.m_f0c;
    m_f10 += other.m_f10;
    m_f14 += other.m_f14;
    m_f18 += other.m_f18;
    m_f1c += other.m_f1c;
    m_f20 += other.m_f20;
    m_f24 += other.m_f24;
    m_f28 += other.m_f28;
    m_f30 += other.m_f30;
    m_f34 += other.m_f34;
}

int CDungeonNode::InsertDB(const DungeonKey& key)
{
    for (std::map<int, Monster>::iterator it = m_monsterMap.begin(); it != m_monsterMap.end(); ++it)
    {
        Monster& m = it->second;
        if (m.m_count != 0)
        {
            MySQL* sql = GetMySqLInstance();
            sql->set_query(
                "inSert into log_dungeon_monster(channel_no,dungeon_index,dungeon_diff,"
                "monster_index,damage_total,damage_count,last_time) "
                "values(%d,%d,%d,%d,%d,%d,NOW())",
                G_CEnvironment()->get_channel_no(), key.m_dungeonIndex, key.m_dungeonDiff,
                it->first, m.m_damageTotal, m.m_damageCount);
            if (!sql->exec(true))
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0x13d, 5);
                trace("inSert log_dungeon_monster failed ");
                return false;
            }
        }
        m.Reset();
    }

    for (std::map<int, CDungeonJobNode>::iterator itJob = m_jobMap.begin(); itJob != m_jobMap.end(); ++itJob)
    {
        itJob->second.InsertDB(key, itJob->first);
    }

    for (int i = 0; i <= 1; ++i)
    {
        m_success[i].InsertDB(key, i != 0);
    }
    return true;
}

// ============================================================================
// CDungeonNode::InsertMonsterKey(int) —— ORIG T 0x084852de
// 已存在返回 false；否则构造 Monster 插入，返回 true。
// ============================================================================
bool CDungeonNode::InsertMonsterKey(int monsterIndex)
{
    std::map<int, Monster>::iterator e = m_monsterMap.end();
    std::map<int, Monster>::iterator it = m_monsterMap.find(monsterIndex);
    if (it != e)
        return false;
    Monster monster;
    m_monsterMap.insert(std::make_pair(monsterIndex, monster));
    return true;
}

// ============================================================================
// CDungeonNode::InsertSkillKey(int, int, int, int) —— ORIG T 0x08485404
// 参数 1 = skillIndex（转 CDungeonJobNode::InsertSkillKey）、参数 2 = job（m_jobMap 键）；
// 参数 3/4 未用。恒返回 1。
// ============================================================================
bool CDungeonNode::InsertSkillKey(int skillIndex, int job, int type, int flag)
{
    std::map<int, CDungeonJobNode>::iterator it = m_jobMap.find(job);
    std::map<int, CDungeonJobNode>::iterator e = m_jobMap.end();
    if (it != e)
    {
        it->second.InsertSkillKey(skillIndex);
        return true;
    }
    CDungeonJobNode node;
    node.InsertSkillKey(skillIndex);
    m_jobMap.insert(std::make_pair(job, node));
    return true;
}

// ============================================================================
// CDungeonNode::UpdateCharacter(SIG_DUNGEON_DATA_STATISTIC*) —— ORIG T 0x08485564
// m_success[stat->m_successType].UpdateCharacter(stat)；返回 1。
// ============================================================================
bool CDungeonNode::UpdateCharacter(SIG_DUNGEON_DATA_STATISTIC* stat)
{
    m_success[stat->m_successType].UpdateCharacter(stat);
    return true;
}

// ============================================================================
// CDungeonNode::UpdateMonster(SIG_DUNGEON_DATA_STATISTIC*) —— ORIG T 0x08485628
// find(stat->m_monsterKey)；命中则 it->second += stat->m_monster（Monster 聚合）。
// ============================================================================
bool CDungeonNode::UpdateMonster(SIG_DUNGEON_DATA_STATISTIC* stat)
{
    std::map<int, Monster>::iterator it = m_monsterMap.find(stat->m_monsterKey);
    if (it == m_monsterMap.end())
        return false;
    Monster& m = it->second;
    m += *reinterpret_cast<const Monster*>(stat->m_monster);
    return true;
}

// ============================================================================
// CDungeonNode::UpdateSkill(SIG_DUNGEON_DATA_STATISTIC*) —— ORIG T 0x0848559a
// find(stat->m_job)；命中则 CDungeonJobNode::UpdateSkill(stat)。
// ============================================================================
bool CDungeonNode::UpdateSkill(SIG_DUNGEON_DATA_STATISTIC* stat)
{
    int dummy;
    std::map<int, CDungeonJobNode>::iterator it = m_jobMap.find(stat->m_job);
    if (it == m_jobMap.end())
        return false;
    it->second.UpdateSkill(stat);
    return true;
}

// ============================================================================
// CDungeonJobNode::InsertSkillKey(int) —— ORIG T 0x08484a86
// 已存在返回 false；否则构造 Skill 插入，返回 true。
// ============================================================================
bool CDungeonJobNode::InsertSkillKey(int skillIndex)
{
    std::map<int, Skill>::iterator e = m_skillMap.end();
    std::map<int, Skill>::iterator it = m_skillMap.find(skillIndex);
    if (it != e)
        return false;
    Skill skill;
    m_skillMap.insert(std::make_pair(skillIndex, skill));
    return true;
}

// ============================================================================
// CDungeonJobNode::UpdateSkill(SIG_DUNGEON_DATA_STATISTIC*) —— ORIG T 0x08484bac
// find(stat->m_skillKey)；命中则 it->second += stat->m_charac[0x40]（Skill 聚合）。
// ============================================================================
bool CDungeonJobNode::UpdateSkill(SIG_DUNGEON_DATA_STATISTIC* stat)
{
    std::map<int, Skill>::iterator it = m_skillMap.find(stat->m_skillKey);
    std::map<int, Skill>::iterator e = m_skillMap.end();
    if (it == e)
        return false;
    Skill& s = it->second;
    s += *reinterpret_cast<const Skill*>(stat->m_charac + 0x40);
    return true;
}

// ============================================================================
// CDungeonJobNode::InsertDB(DungeonKey const&, int) —— ORIG T 0x08485e00
// 遍历 m_skillMap：m_useCount!=0 时写 log_dungeon_skill（失败即返 0），随后 Reset。
// ============================================================================
int CDungeonJobNode::InsertDB(const DungeonKey& key, int job)
{
    for (std::map<int, Skill>::iterator it = m_skillMap.begin(); it != m_skillMap.end(); ++it)
    {
        Skill& s = it->second;
        if (s.m_useCount != 0)
        {
            MySQL* sql = GetMySqLInstance();
            sql->set_query(
                "inSert into log_dungeon_skill(skill_cool_time,skill_use,skill_count,"
                "channel_no,dungeon_index,dungeon_diff,charac_job,charac_grow,"
                "skill_index,last_time) values(%d,%d,%d,%d,%d,%d,%d,%d,%d,NOW())",
                s.m_coolTime, s.m_skillUse, s.m_skillCount,
                G_CEnvironment()->get_channel_no(), key.m_dungeonIndex, key.m_dungeonDiff,
                job, 0, it->first);
            if (!sql->exec(true))
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0x177, 5);
                trace("inSert log_dungeon_skill failed");
                return false;
            }
        }
        s.Reset();
    }
    return true;
}

// ============================================================================
// CDungeonSuccessNode::CDungeonSuccessNode() —— ORIG T 0x08484dda
// m_partyMap 预插 job 2..4 的 Party；m_jobMap 预插 job 0..0xa × sub 0..5 × grow 0..2
// 的 CSuccessJobNode（JobKey = {job, STGrowthType{sub, grow<<4}}）。
// ============================================================================
CDungeonSuccessNode::CDungeonSuccessNode()
    : m_partyMap(), m_jobMap()
{
    for (int job = 2; job <= 4; ++job)
    {
        Party p;
        m_partyMap.insert(std::make_pair(job, p));
    }

    JobKey key;
    STGrowthType grow;
    for (int job = 0; job <= 0xa; ++job)
    {
        key.m_job = job;
        for (int sub = 0; sub <= 5; ++sub)
        {
            grow.m_growthType = 0;
            grow.m_growthType = (unsigned char)(sub & 0xf);
            for (int g = 0; g <= 2; ++g)
            {
                grow.m_growthType = (unsigned char)((grow.m_growthType & 0x8f) | ((g & 7) << 4));
                key.m_sub = grow.m_growthType;
                CSuccessJobNode node;
                m_jobMap.insert(std::make_pair(key, node));
            }
        }
    }
}

// ============================================================================
// CDungeonSuccessNode::InsertDB(DungeonKey const&, bool) —— ORIG T 0x08485f8a
// 遍历 m_jobMap 转 CSuccessJobNode::InsertDB(key, flag, jobKey)；
// 再遍历 m_partyMap：m_f3c!=0 时写 log_dungeon_party（失败即返 0），随后 Reset。
// ============================================================================
int CDungeonSuccessNode::InsertDB(const DungeonKey& key, bool flag)
{
    for (std::map<JobKey, CSuccessJobNode>::iterator itJob = m_jobMap.begin(); itJob != m_jobMap.end(); ++itJob)
    {
        itJob->second.InsertDB(key, flag, itJob->first);
    }

    for (std::map<int, Party>::iterator it = m_partyMap.begin(); it != m_partyMap.end(); ++it)
    {
        Party& p = it->second;
        if (p.m_f3c != 0)
        {
            MySQL* sql = GetMySqLInstance();
            sql->set_query(
                "inSert into log_dungeon_party(clear_time,die_count,hp_consume,"
                "mp_consume,hit_count,hit_per_avg_damage,hp_recovery,mp_recovery,"
                "update_count,channel_no,dungeon_index,dungeon_diff,success,level,"
                "party_user_count,last_time) values(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,"
                "%d,%d,%d,NOW())",
                p.m_f00, p.m_f04, p.m_f08, p.m_f0c, p.m_f10, p.m_f14, p.m_f18, p.m_f1c,
                p.m_f3c, G_CEnvironment()->get_channel_no(), key.m_dungeonIndex,
                key.m_dungeonDiff, flag, p.m_f24, it->first);
            if (!sql->exec(true))
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0x1b9, 5);
                trace("inSert log_dungeon_party failed");
                                return 0;
            }
        }
        p.Reset();
    }
    return 1;
}

// ============================================================================
// CDungeonSuccessNode::UpdateCharacter(SIG_DUNGEON_DATA_STATISTIC*) —— ORIG T 0x084851de
// m_partyMap.find(stat->m_charac[0x20] 处 int)：命中 → PlayInfo::operator+= 聚合
// （ORIG 走 Character::operator+= 语义）+ m_f3c 自增；
// m_jobMap.find(*reinterpret_cast<JobKey*>(stat+0x10))：命中 → CSuccessJobNode::UpdateCharacter。
// ============================================================================
int CDungeonSuccessNode::UpdateCharacter(SIG_DUNGEON_DATA_STATISTIC* stat)
{
    std::map<int, Party>::iterator it =
        m_partyMap.find(*reinterpret_cast<const int*>(stat->m_charac + 0x20));
    std::map<int, Party>::iterator e = m_partyMap.end();
    if (it != e)
    {
        Party& p = it->second;
        p += *reinterpret_cast<const PlayInfo*>(stat->m_charac);
        p.m_f3c = p.m_f3c + 1;
    }

    std::map<JobKey, CSuccessJobNode>::iterator itJob =
        m_jobMap.find(*reinterpret_cast<const JobKey*>(&stat->m_job));
    std::map<JobKey, CSuccessJobNode>::iterator eJob = m_jobMap.end();
    if (itJob != eJob)
    {
        itJob->second.UpdateCharacter(stat);
    }
    return 1;
}

// ============================================================================
// CSuccessJobNode::CSuccessJobNode() —— ORIG T 0x08484c30
// m_jobPartyMap 预插 job 2..4 的 JobParty。
// ============================================================================
CSuccessJobNode::CSuccessJobNode()
    : m_jobPartyMap(), m_charac()
{
    for (int job = 2; job <= 4; ++job)
    {
        JobParty p;
        m_jobPartyMap.insert(std::make_pair(job, p));
    }
}

// ============================================================================
// CSuccessJobNode::InsertDB(DungeonKey const&, bool, JobKey const&) —— ORIG T 0x08486234
// m_charac.m_f3c!=0 时写 log_dungeon_charac（失败即返 0）；
// 再遍历 m_jobPartyMap：m_count!=0 时写 log_dungeon_party_job（失败即返 0），随后 Reset。
// ============================================================================
int CSuccessJobNode::InsertDB(const DungeonKey& key, bool flag, const JobKey& jobKey)
{
    if (m_charac.m_f3c != 0)
    {
        MySQL* sql = GetMySqLInstance();
        sql->set_query(
            "inSert into log_dungeon_charac(clear_time,die_count,hp_consume,mp_consume,"
            "hit_count,hit_per_avg_damage,hp_recovery,mp_recovery,update_count,channel_no,"
            "dungeon_index,dungeon_diff,success,charac_job,level,charac_grow,last_time,"
            "exp_avg) values(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,NOW(),%d)",
            m_charac.m_f00, m_charac.m_f04, m_charac.m_f08, m_charac.m_f0c,
            m_charac.m_f10, m_charac.m_f14, m_charac.m_f18, m_charac.m_f1c,
            m_charac.m_f3c, G_CEnvironment()->get_channel_no(), key.m_dungeonIndex,
            key.m_dungeonDiff, flag, jobKey.m_job, m_charac.m_f24,
            (int)(char)jobKey.m_sub, m_charac.m_f28);
        if (!sql->exec(true))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x1f2, 5);
            trace("inSert log_dungeon_charac failed");
            return 0;
        }
    }

    for (std::map<int, JobParty>::iterator it = m_jobPartyMap.begin(); it != m_jobPartyMap.end(); ++it)
    {
        JobParty& jp = it->second;
        if (jp.m_count != 0)
        {
            MySQL* sql = GetMySqLInstance();
            sql->set_query(
                "inSert into log_dungeon_party_job(channel_no,dungeon_index,dungeon_diff,"
                "charac_job,charac_grow,party_user_count,success,job_count,last_time) "
                "values(%d,%d,%d,%d,%d,%d,%d,%d,NOW())",
                G_CEnvironment()->get_channel_no(), key.m_dungeonIndex, key.m_dungeonDiff,
                jobKey.m_job, (int)(char)jobKey.m_sub, it->first, flag, jp.m_count);
            if (!sql->exec(true))
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0x215, 5);
                trace("inSert log_dungeon_party_job failed");
                return 0;
            }
        }
        jp.Reset();
    }
    return 1;
}

// ============================================================================
// CSuccessJobNode::UpdateCharacter(SIG_DUNGEON_DATA_STATISTIC*) —— ORIG T 0x08484d4c
// m_jobPartyMap.find(stat->m_charac[0x20] 处 int)：命中 → m_count 自增；
// m_charac += stat->m_charac（Character 聚合；ORIG 为 Character::operator+= 单调用，
// m_charac 成员现为 Character，与 ORIG 一致）。
// ============================================================================
int CSuccessJobNode::UpdateCharacter(SIG_DUNGEON_DATA_STATISTIC* stat)
{
    std::map<int, JobParty>::iterator it =
        m_jobPartyMap.find(*reinterpret_cast<const int*>(stat->m_charac + 0x20));
    std::map<int, JobParty>::iterator e = m_jobPartyMap.end();
    if (it != e)
    {
        it->second.m_count = it->second.m_count + 1;
    }
    m_charac += *reinterpret_cast<const Character*>(stat->m_charac);
    m_charac.m_f3c = m_charac.m_f3c + 1;
    return 1;
}
