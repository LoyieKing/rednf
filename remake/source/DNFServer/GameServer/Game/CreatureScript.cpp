// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CreatureScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CreatureScript.h"

#include <string.h>
#include <cstdio>
#include <utility>

#include "DNFLexWrapper.h"
#include "GameScriptEnumHelpers.h"
#include "LogManager.h"
#include "NpcScript.h"
#include "STComputeDamageVariable.h"
// [b28d-solo-CreatureScript] 本 TU 发射的 STComputeDamageVariable::C1
// （ORIG W 0x08890936，定义在共享头 STComputeDamageVariable.h）：已按 ORIG
// 逐段修至 **257=257**——m_f00F[4..6] 循环（0x088909b0）、critMatrix 单循环两行
// （0x08890bd8）、reduce 率收尾（0x08890d94/d9f）三处全部对齐。唯一残差 = 该
// m_f00F 循环的 `lea 4(%i)`/this 装载序（ORIG `mov i,%edx; mov this,%eax;
// lea 4(%edx),%ecx` vs 4.4 `mov i,%eax; lea 4(%eax),%ecx; mov this,%eax`）
// ⇒ N82/P75 跨版本求值序封顶（探针 /tmp/probe_f7.cpp a1/b1/c1/d1 四形均 base-first）。
#include "CEnvironment.h"     // class SpinLock / struct less_TCHAR

#include "CreatureScript/processModuleTagSet.inc"
                         int type, bool flag);

#include "CreatureScript/InitAttackInfoScript.inc"


// ---- 全局（ORIG 属 RDARScriptObject.cpp/RDARScriptAttackInfo.cpp）----
char g_creatureScriptBaseDirectory[0x104];

// ORIG g_creatureScriptFileList（STScriptFileList，+0x00 即 map<int,string>）。
STScriptFileList g_creatureScriptFileList;

// 模块名表 MODULE_NAME_TAG（processModuleTagSet 用；+8 步长 = 开/闭 string 对）。
// [OurOnly33 全局符号轴 2026-09-11] 改名对齐：ORIG `MODULE_NAME_TAG` B 0x88 @0x0949b2c0
//（唯一一份、外部链接）；原 `g_moduleNameTag` 系我方发明名（段/大小逐项一致）。
// [OurOnly33 全局符号轴 2026-09-11 · Main 特批补初值] ORIG static_init 0x088c3055 实证
// （objdump 逐条）：该表 34 个 string 中 **32 项**以 `_ZNSsC1EPKcRKSaIcE` 逐项构造，
// 串地址序列（+`__tcf_0` 0x088c3027 逆序 34×`_ZNSsD1Ev` 印证 17×2 形态）：
//   [dungeon]/[/dungeon] ×4, [pvp]/[/pvp] ×2, [dungeon]/[/dungeon],
//   [warroom]/[/warroom], [dungeon]/[/dungeon], [assault]/[/assault],
//   [death tower]/[/death tower], [dungeon]/[/dungeon] ×4, [boss tower]/[/boss tower]
// ⇒ 前 16 组逐字节照抄（串区 0x8e08a08..0x8e08da7 dump）。
// 第 17 组（idx 16）**已确证**为默认构造（空串）：ORIG 该段以循环
//   `88c402b: movl $0x1,-0x40(%ebp)` / `88c4034: mov %ebx,(%esp); call _ZNSsC1Ev;
//    add $0x4,%ebx` / `88c403f: subl $0x1,-0x40(%ebp)` / `88c4043: cmpl $-1,-0x40(%ebp)`
//   逐项默认构造（计数 1→0→−1 = **2 次**），起点 = 表基址（%esi 经 %edi→%ebx）
//   ⇒ idx16[0]/[1] 均为 `std::string()`。此处以 `std::string()` 显式写出以对齐形态。
std::string MODULE_NAME_TAG[17][2] = {
    { "[dungeon]",     "[/dungeon]"     },
    { "[dungeon]",     "[/dungeon]"     },
    { "[dungeon]",     "[/dungeon]"     },
    { "[dungeon]",     "[/dungeon]"     },
    { "[pvp]",         "[/pvp]"         },
    { "[pvp]",         "[/pvp]"         },
    { "[dungeon]",     "[/dungeon]"     },
    { "[warroom]",     "[/warroom]"     },
    { "[dungeon]",     "[/dungeon]"     },
    { "[assault]",     "[/assault]"     },
    { "[death tower]", "[/death tower]" },
    { "[dungeon]",     "[/dungeon]"     },
    { "[dungeon]",     "[/dungeon]"     },
    { "[dungeon]",     "[/dungeon]"     },
    { "[dungeon]",     "[/dungeon]"     },
    { "[boss tower]",  "[/boss tower]"  },
    { std::string(),   std::string()    },   // [16] ORIG 默认构造 ×2（上注实证）
};

// ---- 原生类型生命周期 ----

#include "CreatureScript/selectiveGrowCreatureInfo.inc"


#include "CreatureScript/clear.inc"


#include "CreatureScript/clear_1.inc"


#include "CreatureScript/CreatureScript.inc"


#include "CreatureScript/dtor_CreatureScript.inc"


#include "CreatureScript/clear_2.inc"


// ---- AttackInfo 生命周期与方法 ----

#include "CreatureScript/AttackInfo.inc"


#include "CreatureScript/dtor_AttackInfo.inc"


#include "CreatureScript/clear_3.inc"


#include "CreatureScript/isIntendDamage.inc"


#include "CreatureScript/setElement.inc"


#include "CreatureScript/isValidAttackInfoElement.inc"

#include "CreatureScript/isValidElement.inc"


#include "CreatureScript/setAttackType.inc"


#include "CreatureScript/getAttackType.inc"


// ---- 辅助自由函数 ----

#include "CreatureScript/getElement.inc"


#include "CreatureScript/getDrawLayer.inc"


// ORIG 0x88944fb：active status 字符串→0..0x11，默认 0x12。
// [OurOnly33 符号名轴 2026-09-11] ORIG 权威（0x088944fb 两条 `mov 0xXXXX(,%eax,4)` 装载）：
//   ACTIVESTATUS_ATTACK_TAG          @0x093f54e0 (D, 0x4c=19 指针=18 项+NULL，带括号标签)
//   ACTIVE_STATUS_NO_BOX_BRACKET_TAG @0x093f5540 (D, 0x4c，无括号标签)
// 原函数内 static kTag/kNoBox（我方发明名 `_ZZ19getActiveStatusTypeRKsSE4kTag`/`6kNoBox`）
// ⇒ 删除：ATTACK_TAG 引用 CharacterAIA2.cpp 的唯一定义（STMonsterScript 同法），
// NO_BOX 表本 TU 定义（19 项，与 ORIG 大小一致）；比较序按 ORIG（ATTACK 先、NO_BOX 后）。
// [R13 G 轴数据内容 2026-09-12] ORIG 表体 raw（fastd.py orig --raw 0x093f5540 76，
// 19×4B 指针 0x08e2836d/72/79/80/85/8b/91/9b/a1/a7/ac/ba/c4/ca/d0/d8/e0/e5/f1）
// 逐项读串（raw 0x08e2836d 205B）后逐字节对齐：
//   #10 = "weapon break]"（我方旧 "weapon break" 缺尾 ]）
//   #11 = "bleeding]"（我方旧 "bleeding" 缺尾 ]）
//   #18 = "none"（索引 18 因消费点循环 i<=0x11 不可达，ORIG 侧亦为 "none" 非 NULL；照抄）
// 其余 16 项与 ORIG 逐字节一致。
extern "C" const char* ACTIVESTATUS_ATTACK_TAG[20];   // D 0x093f54e0（CharacterAIA2.cpp）
extern "C" const char* ACTIVE_STATUS_NO_BOX_BRACKET_TAG[19];
const char* ACTIVE_STATUS_NO_BOX_BRACKET_TAG[19] = {
    "slow", "freeze", "poison", "stun", "curse", "blind", "lightning",
    "stone", "sleep", "burn", "weapon break]", "bleeding]", "haste",
    "bless", "element", "confuse", "hold", "armor break",
    "none"
};

#include "CreatureScript/getActiveStatusType.inc"
{
    for (int i = 0; i <= 0x11; ++i)
    {
        if (s == ACTIVESTATUS_ATTACK_TAG[i] || s == ACTIVE_STATUS_NO_BOX_BRACKET_TAG[i])
            return i;
    }
    return 0x12;
}

// ============================================================================
// importCreatureScript —— ORIG 0x8a35e47（RDARScriptObject.cpp）
// 逐行 ScanType 分派；标签→字段写回。loadRDARScriptFile(g_creatureScriptBaseDirectory,
// path) 装载；末尾以 STScriptFileList::FindIndexByFullScan 写回 m_id。
// ============================================================================
namespace
{
// [skill list] 子域查找表 tagId（ORIG 汇编 0x8a3776f 区 dispatch 树 +
// 12 段 pair 注册的 movl $0xNNN 常量实证）。
enum CreatureSkillTag
{
    CSTAG_NODE_END = 0x25f6,             // [/skill node]
    CSTAG_SKILL_TYPE = 0x260c,           // [skill type]
    CSTAG_SKILL_NAME = 0x2612,           // [skill name]
    CSTAG_SKILL_EXPLAIN = 0x2617,        // [skill explain]
    CSTAG_SKILL_RECOVERY = 0x261c,       // [skill recovery time]
    CSTAG_LEARN_SKILL_LEVEL = 0x2623,    // [learn skill level]
    CSTAG_SKILL_MP = 0x262a,             // [skill mp]
    CSTAG_SKILL_MOTION = 0x2631,         // [skill motion]
    CSTAG_SKILL_UNDER_EFFECT = 0x2636,   // [skill under effect]
    CSTAG_SKILL_OVER_EFFECT = 0x263b,    // [skill over effect]
    CSTAG_SKILL_INFO = 0x2640,           // [skill info]
    CSTAG_ATTACK_INFO = 0x2645,          // [attack info]
};
}  // namespace

#include "CreatureScript/importCreatureScript.inc"


// ============================================================================
// ImportAttackInfoScript —— ORIG 0x88d145a（RDARScriptAttackInfo.cpp）
// ============================================================================
// 注意:此函数与 importCreatureScript 的逐行 if-else 直链不同,ORIG 汇编
// (/tmp/ImportAttackInfoScript.md 汇编 0x1577 区 + Ghidra C)为:
//   static map<const char*,int,less_TCHAR> __lookup(函数局部,guard 构造)
//   + SpinLock __init_lock + bool __inited;
//   注册在 switch 内逐 case 完成:!__inited 时每个 case 构造
//   pair(key,tagId) 并 map::insert 后贯穿到下一 case(0x42→0x46→…→0x1d2 链,
//   末 case insert 后跳公共尾 0x24bb),尾处 leave + __inited=1 并重查同一
//   token 做真实解析;之后每行 find→switch。首行 tagId=0 与 case 0x42 共享
//   入口。每行处理前先调 processModuleTagSet:返回 false 表示该行
//   是 module 开/闭边界行(已被其内部 ScanType 消费),跳过本行数据解析;第二
//   输出(subId)!= -1 时写 AttackInfo::m_moduleTag(+0x8c)。
// module 化字段:[elemental property](+0xa4 set)与 [active status] 各自持有
// module 名缓存 string(初值 MODULE_NAME_TAG+0x18 = g_moduleNameTag[6],与
// clear() 的 m_moduleTag=3 对齐);遇 module 变化分别 clear m_elements / 清零
// active status 的 int3 槽(0xec..0xf4)与 3 个 vector(0xf8/0x104/0x110)。
// ORIG 0x88d1f95 的重置循环不清 type/value/int/int2 槽,也不重置 statusIdx。
// [active status] 槽计数 statusIdx(ORIG -0x34)只在函数入口清零:每个文件至多
// 3 个 status;超限 log 后 ORIG 0x88d20b9 跳 0x88d393d 重查同一 token(数据非法
// 时会循环),此处以内层 continue 同构重建。
// [human active status rate] 除以 _DAT_08e0ab18 = 100.0f(原 ELF 0x08e0ab18
// 实测 4 字节 0x42c80000)。
namespace
{
enum AttackInfoTag
{
    AITAG_DAMAGE = 0x42,                     // [damage]
    AITAG_DAMAGE_INCREASE_RATE = 0x46,       // [damage increase rate]
    AITAG_ABSOLUTE_DAMAGE = 0x4a,            // [absolute damage]
    AITAG_DAMAGE_BONUS = 0x4e,               // [damage bonus]
    AITAG_ATTACK_TYPE = 0x52,                // [attack type]
    AITAG_WEAPON_DAMAGE_APPLY = 0x63,        // [weapon damage apply]（99）
    AITAG_ATTACK_MYSELF = 0x67,              // [attack myself]
    AITAG_ATTACK_FRIEND = 0x6b,              // [attack friend]
    AITAG_ATTACK_ENEMY = 0x6f,               // [attack enemy]
    AITAG_ELEMENTAL_PROPERTY = 0x73,         // [elemental property]
    AITAG_ACTIVE_STATUS = 0x86,              // [active status]
    AITAG_DAMAGE_REACTION = 0xe2,            // [damage reaction]
    AITAG_PUSH_ASIDE = 0xf6,                 // [push aside]
    AITAG_LIFT_UP = 0xfa,                    // [lift up]
    AITAG_CRITICAL_HIT = 0xfe,               // [critical hit]
    AITAG_STUCK = 0x102,                     // [stuck]
    AITAG_STUCK_PROB_INCREASE = 0x106,       // [stuck prob increase]
    AITAG_KNUCK_BACK = 0x10a,                // [knuck back]
    AITAG_WEAKNESS = 0x11b,                  // [weakness]
    AITAG_BOUNCE = 0x120,                    // [bounce]
    AITAG_IGNORE_DEFENSE = 0x124,            // [ignore defense]
    AITAG_HUMAN_DAMAGE_RATE = 0x128,         // [human damage rate]
    AITAG_FORT_DAMAGE_RATE = 0x12c,          // [fort damage rate]（300）
    AITAG_MONSTER_DAMAGE_RATE = 0x130,       // [monster damage rate]
    AITAG_HP_DRAIN = 0x134,                  // [HP drain]
    AITAG_MP_DRAIN = 0x13d,                  // [MP drain]
    AITAG_HIT_WAV = 0x146,                   // [hit wav]
    AITAG_ATTACK_DIRECTION = 0x14a,          // [attack direction]
    AITAG_HIT_INFO = 0x15b,                  // [hit info]
    AITAG_IS_USE_WEAPON_HIT_DELAY_RATE = 0x17d,  // [is use weapon hit delay rate]
    AITAG_FORCE_HIT_STUN_TIME = 0x181,       // [force hit stun time]
    AITAG_HIT_DIRECTION = 0x186,             // [hit direction]
    AITAG_IGNORE_SUPER_ARMOR = 0x19b,        // [ignore super armor]
    AITAG_IGNORE_WEIGHT = 0x19f,             // [ignore weight]
    AITAG_HUMAN_ACTIVE_STATUS_RATE = 0x1a3,  // [human active status rate]
    AITAG_IS_HIT_NUMBER_INCREASING = 0x1aa,  // [is hit number increasing]
    AITAG_CATCH_OBJECT = 0x1af,              // [catch object]
    AITAG_MAX_TARGET_HIT_NUMBER = 0x1c6,     // [max target hit number]
    AITAG_IGNORE_DIEHARD = 0x1cd,            // [ignore diehard]
    AITAG_HIT_CHECK_BY_SCREEN_POS = 0x1d2,   // [hit check by screen pos]
};

// ORIG 全局实证同 ImportSkillScript:map<const char*,int> 全部用 less_TCHAR
//(strcmp 内容比较);find(tag.c_str()) 传动态指针,默认 less 会恒 miss。
// 查找表本体为函数局部静态(__lookup/__init_lock/__inited),见下方函数。

}  // namespace

#include "CreatureScript/ImportAttackInfoScript.inc"


// ============================================================================
// user_creature::CCreatureScript —— 实现（CreatureScript.h 声明）
// ============================================================================
#include "CCreatureScript.h"

namespace user_creature
{

#include "CreatureScript/CCreatureScript.inc"


#include "CreatureScript/dtor_CCreatureScript.inc"


#include "CreatureScript/Set.inc"


#include "CreatureScript/GetItemEvolutionCreatureId.inc"


#include "CreatureScript/IsAvailableArtifactSlot.inc"


#include "CreatureScript/SetItemId.inc"


#include "CreatureScript/GetItemId.inc"


#include "CreatureScript/IsExistEvolutionQuest.inc"


#include "CreatureScript/GetEvolutionCreatureId.inc"


#include "CreatureScript/validEventEvlouteCreature.inc"


#include "CreatureScript/isEvoluationCreature.inc"


#include "CreatureScript/GetEvolutionLevel.inc"


#include "CreatureScript/GetMaxLevel.inc"


#include "CreatureScript/GetSkillRecoveryTime.inc"


#include "CreatureScript/GetOverSkillRecoveryTime.inc"


#include "CreatureScript/GetLearnOverSkillLevel.inc"


#include "CreatureScript/validItemExp.inc"


#include "CreatureScript/validItem_AType.inc"


#include "CreatureScript/validItem_BType.inc"


#include "CreatureScript/IsItemGrowCreature.inc"


#include "CreatureScript/GetTypeSelectEvolutionLevel.inc"


#include "CreatureScript/GetGrowItemCountforLevelUp.inc"


#include "CreatureScript/GET_ATypeCreature.inc"


#include "CreatureScript/GET_BTypeCreature.inc"


}  // namespace user_creature

// ORIG 0x88bfee0：ENUM_ELEMENT → 名称字符串（getElementName::buffer）。
const char* getElementName(ENUM_ELEMENT element)
{
    static char buffer[0x10];
    if (element == 0)
        strncpy(buffer, "[fire element]", 0x10);
    else if (element == 1)
        memcpy(buffer, "[water element]", 0x10);
    else if (element == 2)
        strncpy(buffer, "[dark element]", 0x10);
    else if (element == 3)
        memcpy(buffer, "[light element]", 0x10);
    else
        strncpy(buffer, "[no element]", 0x10);
    return buffer;
}

#include "CreatureScript/CreatureAdditionalAnimation.inc"

