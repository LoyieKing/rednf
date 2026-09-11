// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 STSkillScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <stdio.h>
#include <string.h>

#include "STSkillScript.h"

#include <map>

#include "DNFLexWrapper.h"
#include "CEnvironment.h"   // class SpinLock
#include "NpcScript.h"      // class STScriptFileList（0x18 布局）
#include "Quest.h"             // enum ENUM_CHARACTERJOB

// ===================== 前置声明（ORIG 自由函数，实现于各权威 TU） =====================

// （RegisterSkillTags / FinalizeImport 为 ORIG ImportSkillScript 内联段，无独立符号，
//   已分别内联到主循环注册分支与尾部修正，禁止以函数形式编造。）

#include "STSkillScript/importCommand.inc"
#include "STSkillScript/processModuleTagSet.inc"
                         int type, bool flag);                      // T 0x08a97637

extern char g_szSkillScriptBaseDirectory[0x100];                   // B 0x9500720（InitScriptProducers_4.cpp）
extern std::map<int, std::string> g_staSkillScriptFileList[11];     // B 0x9500820


// ---- ORIG ImportSkillScript 尾部修正段（8a9ac50..8a9af42）内联于 ImportSkillScript，
//      ORIG 无独立 FinalizeImport 符号；对应 back()/size()/empty() 等调用点须落在
//      ImportSkillScript 切片内，故直接内联在函数尾部 finalize 标签处，禁止拆独立函数。
// 汇编证据：docs/class_func_reports/_lt_global_gt_/ImportSkillScript.md 2862-3096 行 +
// Ghidra C（max level 夹紧 / 价格向量 back() 补齐 / 图标索引定位）。

// ===================== 嵌套结构 =====================

#include "STSkillScript/SkillUseItem.inc"


#include "STSkillScript/clear.inc"


#include "STSkillScript/SkillLevelFeature.inc"


#include "STSkillScript/SkillLevelFeature_1.inc"


#include "STSkillScript/dtor_SkillLevelFeature.inc"


SkillLevelFeature& SkillLevelFeature::operator=(const SkillLevelFeature& other)
{
    m_field0 = other.m_field0;
    m_str8 = other.m_str8;
    m_vecC = other.m_vecC;
    m_vec18 = other.m_vec18;
    return *this;
}

#include "STSkillScript/clear_1.inc"


#include "STSkillScript/STPassiveFeatureData.inc"


#include "STSkillScript/clear_2.inc"


#include "STSkillScript/STKeyCommand.inc"


#include "STSkillScript/dtor_STKeyCommand.inc"


#include "STSkillScript/clear_3.inc"


// ===================== STSkillScript =====================

#include "STSkillScript/STSkillScript.inc"


#include "STSkillScript/dtor_STSkillScript.inc"


#include "STSkillScript/Clear.inc"



// ===================== ImportSkillScript 辅助（ORIG 静态域） =====================

// ---- ORIG static map<const char*,int>（guard 0x9500948 / 对象 0x9500958）

namespace
{

// TAG_* 常量：与 kSkillTags 表 id 一一对应（ORIG ImportSkillScript switch case 实证）
enum
{
    TAG_NAME = 0x129,
    TAG_NAME2 = 0x12c,
    TAG_EXPLAIN = 0x12f,
    TAG_BASIC_EXPLAIN = 0x132,
    TAG_BASIC_PROPERTY = 0x135,
    TAG_MAXIMUM_LEVEL = 0x159,
    TAG_PURCHASE_COST = 0x15c,
    TAG_SPECIAL_PURCHASE_COST = 0x166,
    TAG_REQUIRED_LEVEL = 0x172,
    TAG_REQUIRED_LEVEL_RANGE = 0x175,
    TAG_REQUIRED_RANK = 0x178,
    TAG_REQUIRED_GUILD_LEVEL = 0x17b,
    TAG_REQUIRED_GUILD_LEVEL_RANGE = 0x17e,
    TAG_PRE_REQUIRED_SKILL = 0x181,
    TAG_STEEL_LEARNING_SKILL = 0x18e,
    TAG_TYPE = 0x194,
    TAG_SKILL_CLASS = 0x19b,
    TAG_GROWTYPE_FITNESS = 0x19f,
    TAG_SECOND_GROWTYPE_FITNESS = 0x1a4,
    TAG_GROWTYPE_MAXIMUM_LEVEL = 0x1ac,
    TAG_SECOND_GROWTYPE_MAXIMUM_LEVEL = 0x1b1,
    TAG_SKILL_FITNESS_GROWTYPE = 0x1b9,
    TAG_SKILL_FITNESS_SECOND_GROWTYPE = 0x1c4,
    TAG_CASTING_TIME = 0x1cf,
    TAG_DURABILITY_DECREASE_RATE = 0x1d3,
    TAG_CONSUME_MP = 0x1d6,
    TAG_MAINTAIN_MP = 0x1dc,
    TAG_CONSUME_ITEM = 0x1e0,
    TAG_LIMIT_COUNT = 0x1ee,
    TAG_AUTO_LIMIT_COUNT_REDUCE = 0x1f8,
    TAG_COOL_TIME = 0x1fb,
    TAG_LEAGUE_BAN = 0x1ff,
    TAG_SKILL_UNDER_COOLTIME_EFFECT = 0x202,
    TAG_SKILL_UNDER_COOLTIME_EFFECT_EACH = 0x20d,
    TAG_AUTO_COOLTIME_APPLY = 0x218,
    TAG_WEAPON_EFFECT_TYPE = 0x21b,
    TAG_ICON = 0x224,
    TAG_COMMAND_KEY_EXPLAIN = 0x22a,
    TAG_COMMAND = 0x22d,
    TAG_STATIC_DATA = 0x230,
    TAG_COOLTIME_LEVEL_INFO = 0x23b,
    TAG_LEVEL_INFO = 0x23f,
    TAG_LEVEL_PROPERTY = 0x258,
    TAG_SHAKE_SCREEN = 0x27c,
    TAG_SEAL_ENABLE = 0x280,
    TAG_ACTIVE_SEAL_ENABLE = 0x283,
    TAG_PHYSICAL_MAGICAL_CHANGE = 0x287,
    TAG_SKILL_PRELOADING_IMAGE = 0x28b,
    TAG_PURCHASE_GSP = 0x292,
    TAG_PURCHASE_GOLD = 0x29d,
    TAG_START_COOL_TIME = 0x2a8,
    TAG_SKILL_COMMAND_ADVANTAGE = 0x2ac,
    TAG_IS_LINKED_BLAST = 0x2b1,
    TAG_LINKED_BLAST_ADD_POINT = 0x2b4,
    TAG_LINKED_BLAST_DEC_POINT = 0x2b7,
    TAG_CHILD_SKILL_INDEX = 0x2bb,
    TAG_FEATURE_SKILL_INDEX = 0x2be,
    TAG_FEATURE_SKILL_TYPE = 0x2c1,
    TAG_SPECIAL_LEVEL_UP = 0x2c4,
    TAG_BASIC_EXPLAIN_EX = 0x2e1,
    TAG_EXPLAIN_EX = 0x2e4,
    TAG_EXECUTABLE_STATES = 0x2e7,
    TAG_NUT_SKILL_SCRIPT = 0x2fe,
    TAG_COMMAND_CUSTOMIZING = 0x319,
    TAG_COMBO_SET = 0x31d,
};

// ORIG 全局实证：map<PKc,i> 全部用 less_TCHAR（内容比较）——
// find(token.c_str()) 为动态指针，默认 std::less 指针比较恒 miss。
// 65 张标签条目改为 ImportSkillScript 内函数局部静态 __lookup 的逐条 insert
// （ORIG __lookup 是函数局部 static，编译器 __cxa_guard 包裹，见主函数）。
typedef std::map<const char*, int, less_TCHAR> SkillTagMap;

} // namespace

#include "STSkillScript/ImportSkillScript.inc"

