// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CreatureScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— native CreatureScript / AttackInfo 类型实现 + 两个解析函数
//   importCreatureScript / ImportAttackInfoScript 及 g_creatureScriptFileList 全局。
//   依据 docs/class_func_reports/_lt_global_gt_/importCreatureScript.md、
//   ImportAttackInfoScript.md 与 AttackInfo/*.md。
// b22 判词·P93/P154（§6.3）：`__tcf_0` 为编译器生成的通用静态析构名，本 TU 与
// CDeathTower/CShutdowManager 域均有同名符号。compare 工具按名取到 ORIG
// 0x0828f940（CDeathTowerRanking::getRankTable 静态表 s_tables[4] 的数组析构，
// 属 WongWorkDeathTower TU）——索引伪差集，非本 TU 实现缺口；真实 teardown 由
// CDeathTower.cpp getRankTable 的 `static CDeathTowerRanking s_tables[4]` 生成。
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

// processModuleTagSet ORIG T 0x08a97637（RDARScriptSkill.cpp 模块标签域；实现于
// LinkMiscSecuTimerSolo.cpp）。ImportAttackInfoScript 每行先送其辨认 module 开/闭
// 边界行（返回 0 表示已消费，跳过该行数据解析）。
#include "CreatureScript/processModuleTagSet.inc"
#include "CreatureScript/InitAttackInfoScript.inc"
}

// ---- 全局（ORIG 属 RDARScriptObject.cpp/RDARScriptAttackInfo.cpp）----
char g_creatureScriptBaseDirectory[0x104];

// ORIG g_creatureScriptFileList（STScriptFileList，+0x00 即 map<int,string>）。
STScriptFileList g_creatureScriptFileList;

// 模块名表 MODULE_NAME_TAG（processModuleTagSet 用；+8 步长 = 开/闭 string 对）。
std::string g_moduleNameTag[17][2];  // ORIG MODULE_NAME_TAG B 0x88 字节（17 对 = 34 string）

// ---- 原生类型生命周期 ----

#include "CreatureScript/selectiveGrowCreatureInfo.inc"
}

#include "CreatureScript/clear.inc"
}

#include "CreatureScript/clear_1.inc"
}

// ORIG 0x833e5ac：构造全部字符串/向量/映射成员（自动成员构造）后调 clear()。
#include "CreatureScript/CreatureScript.inc"
}

// ORIG 0x8340ca4：逆序析构容器成员（成员自动析构）。
#include "CreatureScript/dtor_CreatureScript.inc"
}

// ORIG 0x833dfac：复位全部标量 + clear 各容器。
#include "CreatureScript/clear_2.inc"
}

// ---- AttackInfo 生命周期与方法 ----

// ORIG 0x889037c：构造 string/set/向量 后调 clear()。
// ORIG 0x0889037c：只构造成员容器（string/set/vector<int>[3]/vector<CatchObjectInfo>）
// 后调 clear()，标量值全部由 clear() 设定（ctor 不预置 0）。
#include "CreatureScript/AttackInfo.inc"
}

#include "CreatureScript/dtor_AttackInfo.inc"
}

// ORIG 0x88904b8：复位全部标量 + 各容器 clear。
#include "CreatureScript/clear_3.inc"
}

// ORIG 0x8890760：+0x14 != 0 或 damage>0 或 absoluteDamage>0 → true。
#include "CreatureScript/isIntendDamage.inc"
}

// ORIG 0x889079a：param==4(无属性) 清空集合，否则 erase+insert。
#include "CreatureScript/setElement.inc"
}

// ORIG 0x809576e：SDC 元素合法性（STComputeDamageVariable* 版本，定义于 SDCLink.cpp）。
#include "CreatureScript/isValidAttackInfoElement.inc"
#include "CreatureScript/isValidElement.inc"
}

// ORIG 0x88908b8：m_attackType(+0x10) = type。
#include "CreatureScript/setAttackType.inc"
}

// ORIG 0x88908c6：按 "attack type physical/magical/absolute" 返回 0/1/2。
// [b28c-L7] 判词：AE 修法=具名局部累加形（ORIG `movl $0x7,-0xc` 初值 +
// 各分支 `movl $N,-0xc` + 单出口 `mov -0xc,%eax`）；早返 return 形多 jmp 岛。
#include "CreatureScript/getAttackType.inc"
}

// ---- 辅助自由函数 ----

// ORIG 0x88bfe5b：字符串→ENUM_ELEMENT（fire/water/dark/light，默认 4）。
#include "CreatureScript/getElement.inc"
}

// ORIG 0x88be12d：draw layer 字符串→0..7。
#include "CreatureScript/getDrawLayer.inc"
}

// ORIG 0x88944fb：active status 字符串→0..0x11，默认 0x12。
#include "CreatureScript/getActiveStatusType.inc"
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
}

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

// ORIG 0x88d145a:ImportAttackInfoScript(AttackInfo*, char const*, int)
// 注册链与解析共用同一 switch：ORIG 每个 case 头部 `if (!__inited)` 则
// 构造 pair(key, tagId) 并 map::insert 后**贯穿(fall through)到下一 case**
// 继续注册，形成 0x42→…→0x1d2 的 40 段直链（汇编各 insert 尾 `jmp <next
// case>`）；已注册时执行真实解析并 break 至公共尾(0x24bb)。首行 tagId=0
// 与 case 0x42 共享入口（dispatch 树 `test %eax,%eax; je 0x19ca`）。
#include "CreatureScript/ImportAttackInfoScript.inc"
}

// ============================================================================
// user_creature::CCreatureScript —— 实现（CreatureScript.h 声明）
// ============================================================================
#include "CCreatureScript.h"

namespace user_creature
{

// ORIG 0x833c77e：vector +8、+0x24/+0x28/+0x2c 清零、set +0x34、vector +0x68、
// clear +8、+0x20 置 0、clear +0x68。
#include "CreatureScript/CCreatureScript.inc"
}

// ORIG 0x833c848：释放 +0x20 数组（delete[]），析构 +0x68/+0x34/+8。
// [b28-BM7 判词] we-larger 22 = N89：ORIG 3 处容器 clear 为 4.1 发射的
// out-of-line 模板实例（0x8083dda/0x80d9f34，objdump 误标 _GLOBAL__I），
// 4.4 libstdc++ 全内联展开循环+delete call；EH landing 双段同构。语义全等。
#include "CreatureScript/dtor_CCreatureScript.inc"
}

// ORIG 0x833c8e8：从原生 CreatureScript* 拷贝字段。
// ORIG 0x833c8e8：从原生 CreatureScript* 拷贝字段。
// ORIG 无 src==0 检查（0x833c8e8 直接 _Znaj 后访问 src）；flags 以 0x833c90b
// 循环读（mov 0xc(%ebp),%eax; mov i,%ecx; add $0x14,%ecx; mov (%eax,%ecx,4),%eax
// = src + 4*(i+0x14) = src+0x50+4i，即 m_artifactSlotRed/Blue/Green 组）。
#include "CreatureScript/Set.inc"
}

// ORIG 0x833cad8：find(m_set34, creatureId)；命中返回 creatureId，否则 0。
#include "CreatureScript/GetItemEvolutionCreatureId.inc"
}

// ORIG 0x833cb38：slot-0x17 在 [0,4) 内返回 m_artifactSlotFlags[slot-0x17]，否则 0。
// 返回 bool + 字段 bool*：callee 尾 movzbl 直落 + caller 0x833771d call 后
// 无物化 = bool→bool 透传（int 返回在 4.4 caller 必插 test+setne）。
#include "CreatureScript/IsAvailableArtifactSlot.inc"
}

#include "CreatureScript/SetItemId.inc"
}

#include "CreatureScript/GetItemId.inc"
}

#include "CreatureScript/IsExistEvolutionQuest.inc"
}

#include "CreatureScript/GetEvolutionCreatureId.inc"
}

// ORIG 0x833f150：遍历 +0x68 向量，命中返回 true。
#include "CreatureScript/validEventEvlouteCreature.inc"
}

// ORIG 0x833f1e0：直访 this+0x8（m_vec8），size() 缓存到具名局部 n 后
//   for(i=0;i<n;i++) 遍历 m_vec8[i]，operator[] 每点直调；命中返回 true。
#include "CreatureScript/isEvoluationCreature.inc"
}

#include "CreatureScript/GetEvolutionLevel.inc"
}

#include "CreatureScript/GetMaxLevel.inc"
}

#include "CreatureScript/GetSkillRecoveryTime.inc"
}

#include "CreatureScript/GetOverSkillRecoveryTime.inc"
}

#include "CreatureScript/GetLearnOverSkillLevel.inc"
}

// ORIG 0x833f280：if (m_field4c != 0) return 0;
//   if (m_field5c==itemIdx || m_field60==itemIdx) return 1 else 0。
#include "CreatureScript/validItemExp.inc"
}

// ORIG 0x833f2bc：if (m_field4c != 0) return 0; if (m_field5c==itemIdx) return 1 else 0。
#include "CreatureScript/validItem_AType.inc"
}

// ORIG 0x833f2ee：if (m_field4c != 0) return 0; if (m_field60==itemIdx) return 1 else 0。
#include "CreatureScript/validItem_BType.inc"
}

// ORIG 0x833f320：if (m_field5c==-1 && m_field60==-1) return 0 else return m_field4c。
#include "CreatureScript/IsItemGrowCreature.inc"
}

#include "CreatureScript/GetTypeSelectEvolutionLevel.inc"
}

#include "CreatureScript/GetGrowItemCountforLevelUp.inc"
}

#include "CreatureScript/GET_ATypeCreature.inc"
}

#include "CreatureScript/GET_BTypeCreature.inc"
}

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

// ---- OnlyOrigBatch3：CreatureAdditionalAnimation 3参构造（ORIG W 0x08a3dcc2）----
// ORIG：m_name 默认构造（_ZNSsC1Ev）**先于**标量 store（+0x0/+0x4），再赋值
// （_ZNSsaSEPKc）。4.1 发射序=meminit（按声明序）→ ctor 体；把 string 写进初始化
// 列表、标量移入 body 即复现（P239 ctor 标量段拆入 body）。
#include "CreatureScript/CreatureAdditionalAnimation.inc"
}
