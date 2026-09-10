// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CDungeonNode/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
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

#include "CDungeonNode/InsertDB.inc"
}

// ============================================================================
// CDungeonNode::InsertMonsterKey(int) —— ORIG T 0x084852de
// 已存在返回 false；否则构造 Monster 插入，返回 true。
// ============================================================================
#include "CDungeonNode/InsertMonsterKey.inc"
}

// ============================================================================
// CDungeonNode::InsertSkillKey(int, int, int, int) —— ORIG T 0x08485404
// 参数 1 = skillIndex（转 CDungeonJobNode::InsertSkillKey）、参数 2 = job（m_jobMap 键）；
// 参数 3/4 未用。恒返回 1。
// ============================================================================
#include "CDungeonNode/InsertSkillKey.inc"
}

// ============================================================================
// CDungeonNode::UpdateCharacter(SIG_DUNGEON_DATA_STATISTIC*) —— ORIG T 0x08485564
// m_success[stat->m_successType].UpdateCharacter(stat)；返回 1。
// ============================================================================
#include "CDungeonNode/UpdateCharacter.inc"
}

// ============================================================================
// CDungeonNode::UpdateMonster(SIG_DUNGEON_DATA_STATISTIC*) —— ORIG T 0x08485628
// find(stat->m_monsterKey)；命中则 it->second += stat->m_monster（Monster 聚合）。
// ============================================================================
#include "CDungeonNode/UpdateMonster.inc"
}

// ============================================================================
// CDungeonNode::UpdateSkill(SIG_DUNGEON_DATA_STATISTIC*) —— ORIG T 0x0848559a
// find(stat->m_job)；命中则 CDungeonJobNode::UpdateSkill(stat)。
// ============================================================================
#include "CDungeonNode/UpdateSkill.inc"
}

// ============================================================================
// CDungeonJobNode::InsertSkillKey(int) —— ORIG T 0x08484a86
// 已存在返回 false；否则构造 Skill 插入，返回 true。
// ============================================================================
#include "CDungeonNode/InsertSkillKey_1.inc"
}

// ============================================================================
// CDungeonJobNode::UpdateSkill(SIG_DUNGEON_DATA_STATISTIC*) —— ORIG T 0x08484bac
// find(stat->m_skillKey)；命中则 it->second += stat->m_charac[0x40]（Skill 聚合）。
// ============================================================================
#include "CDungeonNode/UpdateSkill_1.inc"
}

// ============================================================================
// CDungeonJobNode::InsertDB(DungeonKey const&, int) —— ORIG T 0x08485e00
// 遍历 m_skillMap：m_useCount!=0 时写 log_dungeon_skill（失败即返 0），随后 Reset。
// ============================================================================
#include "CDungeonNode/InsertDB_1.inc"
}

// ============================================================================
// CDungeonSuccessNode::CDungeonSuccessNode() —— ORIG T 0x08484dda
// m_partyMap 预插 job 2..4 的 Party；m_jobMap 预插 job 0..0xa × sub 0..5 × grow 0..2
// 的 CSuccessJobNode（JobKey = {job, STGrowthType{sub, grow<<4}}）。
// ============================================================================
#include "CDungeonNode/CDungeonSuccessNode.inc"
}

// ============================================================================
// CDungeonSuccessNode::InsertDB(DungeonKey const&, bool) —— ORIG T 0x08485f8a
// 遍历 m_jobMap 转 CSuccessJobNode::InsertDB(key, flag, jobKey)；
// 再遍历 m_partyMap：m_f3c!=0 时写 log_dungeon_party（失败即返 0），随后 Reset。
// ============================================================================
#include "CDungeonNode/InsertDB_2.inc"
}

// ============================================================================
// CDungeonSuccessNode::UpdateCharacter(SIG_DUNGEON_DATA_STATISTIC*) —— ORIG T 0x084851de
// m_partyMap.find(stat->m_charac[0x20] 处 int)：命中 → PlayInfo::operator+= 聚合
// （ORIG 走 Character::operator+= 语义）+ m_f3c 自增；
// m_jobMap.find(*reinterpret_cast<JobKey*>(stat+0x10))：命中 → CSuccessJobNode::UpdateCharacter。
// ============================================================================
#include "CDungeonNode/UpdateCharacter_1.inc"
}

// ============================================================================
// CSuccessJobNode::CSuccessJobNode() —— ORIG T 0x08484c30
// m_jobPartyMap 预插 job 2..4 的 JobParty。
// ============================================================================
#include "CDungeonNode/CSuccessJobNode.inc"
}

// ============================================================================
// CSuccessJobNode::InsertDB(DungeonKey const&, bool, JobKey const&) —— ORIG T 0x08486234
// m_charac.m_f3c!=0 时写 log_dungeon_charac（失败即返 0）；
// 再遍历 m_jobPartyMap：m_count!=0 时写 log_dungeon_party_job（失败即返 0），随后 Reset。
// ============================================================================
#include "CDungeonNode/InsertDB_3.inc"
}

// ============================================================================
// CSuccessJobNode::UpdateCharacter(SIG_DUNGEON_DATA_STATISTIC*) —— ORIG T 0x08484d4c
// m_jobPartyMap.find(stat->m_charac[0x20] 处 int)：命中 → m_count 自增；
// m_charac += stat->m_charac（Character 聚合；ORIG 为 Character::operator+= 单调用，
// m_charac 成员现为 Character，与 ORIG 一致）。
// ============================================================================
#include "CDungeonNode/UpdateCharacter_2.inc"
}
