#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CDungeonNode/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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


#include "CDungeonNode/InsertMonsterKey.inc"


#include "CDungeonNode/InsertSkillKey.inc"


#include "CDungeonNode/UpdateCharacter.inc"


#include "CDungeonNode/UpdateMonster.inc"


#include "CDungeonNode/UpdateSkill.inc"


#include "CDungeonNode/InsertSkillKey_1.inc"


#include "CDungeonNode/UpdateSkill_1.inc"


#include "CDungeonNode/InsertDB_1.inc"


#include "CDungeonNode/CDungeonSuccessNode.inc"


#include "CDungeonNode/InsertDB_2.inc"


#include "CDungeonNode/UpdateCharacter_1.inc"


#include "CDungeonNode/CSuccessJobNode.inc"


#include "CDungeonNode/InsertDB_3.inc"


#include "CDungeonNode/UpdateCharacter_2.inc"

