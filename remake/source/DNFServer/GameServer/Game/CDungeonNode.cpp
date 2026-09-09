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

bool CDungeonNode::InsertDB(const DungeonKey& key)   // SIGFIX: bool（ORIG 串）
{
    // [b28d-solo-CDungeonNode · P221 · AE] ORIG 0x08485ba4：循环内 cMyTrace 改全表达式
    // 临时链（4.1 迭代器槽 it -0x54 深于 trace -0x4c；具名 trace 时 4.4 反置）⇒ AE。
    // 补 `int monsterIndex = it->first;`、第二段 `CDungeonJobNode* node` / `int job`
    // 具名局部、两处 it++（p167 CLEAR）。指令数 171=171。
    for (std::map<int, Monster>::iterator it = m_monsterMap.begin(); it != m_monsterMap.end(); it++)
    {
        Monster& m = it->second;
        int monsterIndex = it->first;
        if (m.m_count != 0)
        {
            MySQL* sql = GetMySqLInstance();
            sql->set_query(
                "inSert into log_dungeon_monster(channel_no,dungeon_index,dungeon_diff,"
                "monster_index,damage_total,damage_count,last_time) "
                "values(%d,%d,%d,%d,%d,%d,NOW())",
                G_CEnvironment()->get_channel_no(), key.m_dungeonIndex, key.m_dungeonDiff,
                monsterIndex, m.m_damageTotal, m.m_damageCount);
            if (!sql->exec(true))
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x13d, 5)("inSert log_dungeon_monster failed ");
                return false;
            }
        }
        m.Reset();
    }

    for (std::map<int, CDungeonJobNode>::iterator itJob = m_jobMap.begin(); itJob != m_jobMap.end(); itJob++)
    {
        CDungeonJobNode* node = &itJob->second;
        int job = itJob->first;
        node->InsertDB(key, job);
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
    // [b28d-solo-CDungeonNode · P293] ORIG 0x084852de：end sret -0x44 先、find sret -0x40 后
    // ⇒ 原码是单表达式 `find(x) != end()`（4.4 单表达式形复现 ORIG end 先/find 后，
    // 具名 it/e 形恒反序）。make_pair 第二参为右值 `Monster&&` ⇒ 传 `Monster()` 临时。
    if (m_monsterMap.find(monsterIndex) != m_monsterMap.end())
        return false;
    m_monsterMap.insert(std::make_pair(monsterIndex, Monster()));
    return true;
}

// ============================================================================
// CDungeonNode::InsertSkillKey(int, int, int, int) —— ORIG T 0x08485404
// 参数 1 = skillIndex（转 CDungeonJobNode::InsertSkillKey）、参数 2 = job（m_jobMap 键）；
// 参数 3/4 未用。恒返回 1。
// ============================================================================
bool CDungeonNode::InsertSkillKey(int skillIndex, int job, int type, int flag)
{
    // [b28d-solo-CDungeonNode · 具名 it + 临时 end] ORIG 0x08485404：find sret -0x54（具名 it）
    // 先、end 临时 sret -0x50 后（ORIG 无具名 e 槽）⇒ 源码只具名 it，比较用 `m_jobMap.end()`。
    std::map<int, CDungeonJobNode>::iterator it = m_jobMap.find(job);
    if (it != m_jobMap.end())
    {
        CDungeonJobNode* node = &it->second;
        node->InsertSkillKey(skillIndex);
    }
    else
    {
        CDungeonJobNode node;
        node.InsertSkillKey(skillIndex);
        m_jobMap.insert(std::make_pair(job, node));
    }
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
    std::map<int, CDungeonJobNode>::iterator it = m_jobMap.find(stat->m_job);
    if (it == m_jobMap.end())
        return false;
    // ORIG 0x084855d4：operator-> 后 `add $0x4,%eax; mov %eax,-0xc(%ebp)` ——
    // 源码取具名节点指针再调用（直接 it->second.UpdateSkill 会折成 lea+直调）。
    CDungeonJobNode* node = &it->second;
    node->UpdateSkill(stat);
    return true;
}

// ============================================================================
// CDungeonJobNode::InsertSkillKey(int) —— ORIG T 0x08484a86
// 已存在返回 false；否则构造 Skill 插入，返回 true。
// ============================================================================
bool CDungeonJobNode::InsertSkillKey(int skillIndex)
{
    // [b28d-solo-CDungeonNode · P293] ORIG 0x08484a86：end sret -0x50 先、find sret -0x4c 后
    // ⇒ 单表达式 `find(x) != end()`（具名 it/e 形 4.4 恒反序）。
    // make_pair 第二参为右值 `Skill&&` ⇒ 源形传 `Skill()` 临时。
    if (m_skillMap.find(skillIndex) != m_skillMap.end())
        return false;
    m_skillMap.insert(std::make_pair(skillIndex, Skill()));
    return true;
}

// ============================================================================
// CDungeonJobNode::UpdateSkill(SIG_DUNGEON_DATA_STATISTIC*) —— ORIG T 0x08484bac
// find(stat->m_skillKey)；命中则 it->second += stat->m_charac[0x40]（Skill 聚合）。
// ============================================================================
bool CDungeonJobNode::UpdateSkill(SIG_DUNGEON_DATA_STATISTIC* stat)
{
    // [b28d-39] ORIG 0x08484bac：find sret -0x14 / end sret -0x10 / s -0xc；
    // 聚合源地址 `add $0x64,%eax`（单条）⇒ 必须写 &m_charac[0x40]（数组下标折叠），
    // `m_charac + 0x40` 在 4.4 生 add $0x24 + add $0x40 两条。
    std::map<int, Skill>::iterator it = m_skillMap.find(stat->m_skillKey);
    if (it == m_skillMap.end())
        return false;
    Skill& s = it->second;
    s += *reinterpret_cast<const Skill*>(&stat->m_charac[0x40]);
    return true;
}

// ============================================================================
// CDungeonJobNode::InsertDB(DungeonKey const&, int) —— ORIG T 0x08485e00
// 遍历 m_skillMap：m_useCount!=0 时写 log_dungeon_skill（失败即返 0），随后 Reset。
// ============================================================================
bool CDungeonJobNode::InsertDB(const DungeonKey& key, int job)   // SIGFIX: bool
{
    // [b28d-solo-CDungeonNode · P221 · AE] ORIG 0x08485e00：cMyTrace 改全表达式临时链
    // （迭代器槽 it -0x40 深于 trace -0x38）⇒ AE。`Skill& s = it->second;`
    // + `int skillIndex = it->first;` 两个具名局部（operator-> 各调一次，槽 -0x24/-0x20）；
    // 日志尾参用 skillIndex 而非重读 it->first；it++（ppEi）。110=110。
    for (std::map<int, Skill>::iterator it = m_skillMap.begin(); it != m_skillMap.end(); it++)
    {
        Skill& s = it->second;
        int skillIndex = it->first;
        if (s.m_useCount != 0)
        {
            MySQL* sql = GetMySqLInstance();
            sql->set_query(
                "inSert into log_dungeon_skill(skill_cool_time,skill_use,skill_count,"
                "channel_no,dungeon_index,dungeon_diff,charac_job,charac_grow,"
                "skill_index,last_time) values(%d,%d,%d,%d,%d,%d,%d,%d,%d,NOW())",
                s.m_coolTime, s.m_skillUse, s.m_skillCount,
                G_CEnvironment()->get_channel_no(), key.m_dungeonIndex, key.m_dungeonDiff,
                job, 0, skillIndex);
            if (!sql->exec(true))
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x177, 5)("inSert log_dungeon_skill failed");
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
    // [b28d-solo-CDungeonNode · N104 封顶 + 位域已修] ORIG 0x08484dda：循环变量 job/g/sub
    // 声明于函数头且先置 0（P191；ORIG -0x214/-0x10/-0xc 三条 movl $0，g/sub 声明序按 ORIG
    // 槽深排定）；两段循环各带原码死比较 `find != end()`（P171 照抄）；make_pair 第二参右值。
    // STGrowthType 位域（m_sub:4/m_grow:3/m_rsv:1）复现 ORIG RMW 掩码 0xfffffff0/0xffffff8f；
    // 底层类型改 signed char 复现 ORIG `shl$4; sar$4,%al` / `shl$5; sar$5,%al` 有符号窄化
    // （STGrowthType.h；探针 /tmp/b28d_s218_bf.cpp w2/w6 实证）⇒ 位域段 4 处 DIFF 已归零。
    // 残差 = N104（两循环 EH landing 相对序：4.1 集中置尾 vs 4.4 就地内联，且 4.4 多发
    //        块间 jmp；块内容逐条全等、槽位集全等、p167 33/33 CLEAR）；262 vs 263。
    int job = 0;
    int g = 0;
    int sub = 0;
    for (job = 2; job <= 4; job++)
    {
        if (m_partyMap.find(job) != m_partyMap.end())
        {
        }
        m_partyMap.insert(std::make_pair(job, Party()));
    }

    JobKey key;
    STGrowthType grow;
    for (job = 0; job <= 0xa; job++)
    {
        key.m_job = (ENUM_CHARACTERJOB)job;   // b28c-36: JobKey::m_job enum 化连带
        for (sub = 0; sub <= 5; sub++)
        {
            grow.m_growthType = 0;
            grow.m_sub = sub;
            for (g = 0; g <= 2; g++)
            {
                grow.m_grow = g;
                key.m_sub = grow.m_growthType;
                if (m_jobMap.find(key) != m_jobMap.end())
                {
                }
                m_jobMap.insert(std::make_pair(key, CSuccessJobNode()));
            }
        }
    }
}

// ============================================================================
// CDungeonSuccessNode::InsertDB(DungeonKey const&, bool) —— ORIG T 0x08485f8a
// 遍历 m_jobMap 转 CSuccessJobNode::InsertDB(key, flag, jobKey)；
// 再遍历 m_partyMap：m_f3c!=0 时写 log_dungeon_party（失败即返 0），随后 Reset。
// ============================================================================
bool CDungeonSuccessNode::InsertDB(const DungeonKey& key, bool flag)   // SIGFIX: bool
{
    // [b28d-solo-CDungeonNode · P221 · AE] ORIG 0x08485f8a：cMyTrace 改全表达式临时链
    // ⇒ AE。两循环各补具名引用 `const JobKey& jobKey` / `const int& partyKey`
    // + `CSuccessJobNode* node`、it++。194=194、p167 CLEAR。
    for (std::map<JobKey, CSuccessJobNode>::iterator itJob = m_jobMap.begin(); itJob != m_jobMap.end(); itJob++)
    {
        const JobKey& jobKey = itJob->first;
        CSuccessJobNode* node = &itJob->second;
        node->InsertDB(key, flag, jobKey);
    }

    for (std::map<int, Party>::iterator it = m_partyMap.begin(); it != m_partyMap.end(); it++)
    {
        const int& partyKey = it->first;
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
                key.m_dungeonDiff, flag, p.m_f24, partyKey);
            if (!sql->exec(true))
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x1b9, 5)("inSert log_dungeon_party failed");
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
    // [b28d-solo-CDungeonNode · N107 封顶] ORIG 0x084851de：两段均 find sret 具名
    // （-0x1c/-0x20）、end 临时（-0x18/-0x14，具名 e/eJob 时 4.4 反置，已删）；第二段补具名
    // `CSuccessJobNode* node = &itJob->second;`。残差 = N107（`&stat->m_charac[0x20]`/
    // `stat->m_charac`/`&stat->m_job` 三处取址 4.4 恒 add+mov vs ORIG `lea K(%eax),%reg`；
    // 本轮 20 形探针复证不可造：/tmp/b28d_s218_probe{,2,3}.cpp 覆盖 reinterpret_cast/
    // char* 算术/整型加法回 cast/union 视图/真实具名成员/pack(1)/数组引用/指针形参等，全产 add）。
    // 78 vs 81（+3 = 三处 N107 各多 1 条）；p167 10/10 CLEAR。
    std::map<int, Party>::iterator it =
        m_partyMap.find(*reinterpret_cast<const int*>(&stat->m_charac[0x20]));
    if (it != m_partyMap.end())
    {
        Party& p = it->second;
        p += *reinterpret_cast<const PlayInfo*>(stat->m_charac);
        p.m_f3c = p.m_f3c + 1;
    }

    std::map<JobKey, CSuccessJobNode>::iterator itJob =
        m_jobMap.find(*reinterpret_cast<const JobKey*>(&stat->m_job));
    if (itJob != m_jobMap.end())
    {
        CSuccessJobNode* node = &itJob->second;
        node->UpdateCharacter(stat);
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
    // [b28d-solo-CDungeonNode · N104 封顶] ORIG 0x08484c30：循环体内保留原码的死比较
    // `find(job) != end()`（operator!= 结果被丢弃，无 test/je——P171 照抄）；
    // make_pair 第二参为右值 `JobParty&&`（mangled `IRi7JobPartyE...OS7_`）。
    // 残差 = N104：4.4 在 cleanup1（CharacterD1Ev 段）尾多发 1 条 `jmp <cleanup2>`，
    // ORIG 两 landing 块物理相邻、fall-through 直达（无 jmp）；块内容逐条全等、
    // 槽位集全等、p167 调用点差集 12/12 CLEAR。89 vs 90。
    // N104 为 4.4 的 EH landing 布局固有产物（throw() 法不适用：两侧 landing 均在）。
    for (int job = 2; job <= 4; ++job)
    {
        if (m_jobPartyMap.find(job) != m_jobPartyMap.end())
        {
        }
        m_jobPartyMap.insert(std::make_pair(job, JobParty()));
    }
}

// ============================================================================
// CSuccessJobNode::InsertDB(DungeonKey const&, bool, JobKey const&) —— ORIG T 0x08486234
// m_charac.m_f3c!=0 时写 log_dungeon_charac（失败即返 0）；
// 再遍历 m_jobPartyMap：m_count!=0 时写 log_dungeon_party_job（失败即返 0），随后 Reset。
// ============================================================================
bool CSuccessJobNode::InsertDB(const DungeonKey& key, bool flag, const JobKey& jobKey)   // SIGFIX: bool
{
    // [b28d-solo-CDungeonNode · P221 · AE] ORIG 0x08486234：两处 cMyTrace 改全表达式
    // 临时链（4.1 迭代器槽 -0x54 深于 trace，具名 trace 时 4.4 反置）⇒ AE。
    // 补缺失的 `m_charac.Reset()`（真语义缺口，p167 归零）+ 具名引用
    // `const int& jobIndex = it->first;` + it++。226=226、p167 CLEAR。
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
            cMyTrace(__PRETTY_FUNCTION__, 0x1f2, 5)("inSert log_dungeon_charac failed");
            return 0;
        }
    }

    m_charac.Reset();

    for (std::map<int, JobParty>::iterator it = m_jobPartyMap.begin(); it != m_jobPartyMap.end(); it++)
    {
        const int& jobIndex = it->first;
        JobParty& jp = it->second;
        if (jp.m_count != 0)
        {
            MySQL* sql = GetMySqLInstance();
            sql->set_query(
                "inSert into log_dungeon_party_job(channel_no,dungeon_index,dungeon_diff,"
                "charac_job,charac_grow,party_user_count,success,job_count,last_time) "
                "values(%d,%d,%d,%d,%d,%d,%d,%d,NOW())",
                G_CEnvironment()->get_channel_no(), key.m_dungeonIndex, key.m_dungeonDiff,
                jobKey.m_job, (int)(char)jobKey.m_sub, jobIndex, flag, jp.m_count);
            if (!sql->exec(true))
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x215, 5)("inSert log_dungeon_party_job failed");
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
    // [b28d-solo-CDungeonNode · N107 封顶] ORIG 0x08484d4c：find sret -0x14 具名、
    // end 临时 -0x10（具名 e 时 4.4 反置，已删）；operator-> 只调一次并把
    // `&it->second.m_count` 存具名引用槽 -0xc（P0 槽往返⇒原码有具名局部）；
    // 尾段仅一次 Character::operator+=（无 m_f3c 自增，ORIG 无该指令，已删）。
    // 残差 = N107（`&stat->m_charac[0x20]`/`&stat->m_charac[0]` 取址：4.4 恒
    // `add $K,%reg; mov %reg,%reg2` vs ORIG `lea K(%eax),%ecx`；本轮 20 形探针
    // /tmp/b28d_s218_probe{,2,3}.cpp 复证不可造；第 3 处 `this+0x18` 的 eax/edx
    // 角色互换系前两处连锁）。
    // 46 vs 45（+1 = N107 的 add+mov 比 lea 多 1 条）；p167 5/5 CLEAR。
    std::map<int, JobParty>::iterator it =
        m_jobPartyMap.find(*reinterpret_cast<const int*>(&stat->m_charac[0x20]));
    if (it != m_jobPartyMap.end())
    {
        JobParty& jp = it->second;
        jp.m_count = jp.m_count + 1;
    }
    m_charac += *reinterpret_cast<const Character*>(&stat->m_charac[0]);
    return 1;
}
