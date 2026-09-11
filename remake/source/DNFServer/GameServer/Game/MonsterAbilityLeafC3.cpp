// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 MonsterAbilityLeafC3/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <algorithm>
#include <cstdlib>
#include <map>
#include <utility>
#include <vector>

#include "ActiveStaticInfo.h"
#include "CDungeon.h"     // RidableObject / RidableScript 完整类型
#include "CRidable.h"
#include "STMonsterScript.h"

// ---------------------------------------------------------------------------
// ORIG 枚举类型名仅用于 ABI（mangled 中 17CHAMPION_FEATURES /
// 19ENUM_ACTIVEPROPERTY）；具体枚举子命名未还原【推断】，调用点一律显式转换。
// ---------------------------------------------------------------------------
enum CHAMPION_FEATURES
{
    CHAMPION_FEATURES_MIN = 0
};
enum ENUM_ACTIVEPROPERTY
{
    ENUM_ACTIVEPROPERTY_MIN = 0
};

// ORIG .data D 0x93f4a00[0x120]（72 float）；定义在 InitScriptProducers_4.cpp
// （float[13][5]），运行期由脚本导入填充。此处按声明类型引用并做扁平索引。
extern float DIFFICULTY_OF_DUNGEON_BONUS[13][5];

// 冠军特性随机池：ORIG .rodata 0x8e08000 共 37 项，符号 _ZL21CHAMPION_FEATURE_LIST
// （文件级 static，P204③：_ZL 前缀 mangled 才与 ORIG 一致）；
// 低难度分支取 rand()%28，高难度(diff>3)分支取 [28,35)。
static const int CHAMPION_FEATURE_LIST[37] = {
    0x0001ff00, 0x0000ff01, 0x0002ff02, 0x0002ff03,  // 0..3
    0x00030004, 0x00030005, 0x00030006, 0x00030007,  // 4..7
    0x00040008, 0x0003fe09, 0x0003fe0a, 0x0003fe0b,  // 8..11
    0x0003fe0c, 0x0002000d, 0x0003000e, 0x0003000f,  // 12..15
    0x00020010, 0x00040011, 0x00010012, 0x00020013,  // 16..19
    0x0002fd14, 0x0002fd15, 0x0002fd16, 0x0002fd17,  // 20..23
    0x0002fd18, 0x0002fd19, 0x0002fd1a, 0x0003001b,  // 24..27
    0x0004fd1c, 0x0004fd1d, 0x0004fd1e, 0x0004fd1f,  // 28..31
    0x0004fd20, 0x0004fd21, 0x0004fd22, 0x0004fd23,  // 32..35
    0x0004fd24                                       // 36
};

// 特性 tier→点数开销表：ORIG .rodata 0x8e08094[0x400]，符号
// _ZL21CHAMPFEAT_POINT_MINUS；ORIG 按 (feature & 0xff0000) >> 16 索引。
static const int CHAMPFEAT_POINT_MINUS[256] = {
    0x0000000a, 0x00000008, 0x00000005, 0x00000003, 0x00000002, 0x00000002, 0x0000000c, 0x00000029,
    0x00000021, 0x00000021, 0x00000006, 0x0001869f, 0x00000004, 0x00000003, 0x00000027, 0x000000b4,
    0x00000002, 0x0000000a, 0x00000005, 0x00000014, 0x00000006, 0x00000009, 0x00124f80, 0x000000b4,
    0x00000004, 0x00000006, 0x00000007, 0x00000031, 0x00000008, 0x00000009, 0x00000003, 0x44a54a80,
    0x000003e8, 0x00000005, 0x00000005, 0x00000050, 0x00000096, 0x000000ff, 0x00000010, 0x000000c8,
    0x000000d1, 0x00000003, 0x0000000a, 0x0000003c, 0x00000002, 0x0098967f, 0x0000001d, 0x3b9aca00,
    0x00002710, 0x00989680, 0x000003e8, 0x64050a11, 0x00030ac9, 0x7fffffff, 0x00000258, 0x641e0164,
    0x00000064, 0x0000001e, 0x000003e8, 0x000006a4, 0x00001220, 0x00001220, 0x000003e8, 0x000007d0,
    0x00000a8c, 0x00001388, 0x00001220, 0x00070a1e, 0x46a7b9d0, 0x46d5de50, 0x46e1bbd0, 0x47686d10,
    0x479fcc10, 0x47b208d0, 0x478e7050, 0x000249f0, 0x0007a120, 0x000f4240, 0x000000c8, 0x000000c8,
    0x0001869f, 0x00000004, 0x00000007, 0x00000000, 0x00000040, 0x00000000, 0x00002710, 0x00640064,
    0x00090014, 0x000a0028, 0x0014000a, 0x00640008, 0x00020001, 0x00000005, 0x030a0504, 0x00000032,
    0x000000b4, 0x00000002, 0x00000008, 0x00000004, 0x00000028, 0x00000002, 0x00000004, 0x00000002,
    0x00000007, 0x00000064, 0x0000000a, 0x0000000a, 0x00000080, 0x00000010, 0x0000ea60, 0x00000001,
    0x0000001e, 0x000004b0, 0x00001388, 0x0000270f, 0x00000003, 0x00000008, 0x00000078, 0x00000032,
    0x00000001, 0x00007805, 0x00000080, 0x00000064, 0x0000000f, 0x00000046, 0x000000c8, 0x00000078,
    0x0000003c, 0x00000078, 0x0000005a, 0x0000000f, 0x00000032, 0x00000008, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x0000040f, 0x00000410, 0x00000411, 0x00000412, 0x00000413,
    0x00000414, 0x00000415, 0x00000416, 0x00000009, 0x00000012, 0x00000014, 0x0000f68a, 0x00000014,
    0x00000002, 0x00000013, 0x0000003b, 0x0000002d, 0xffffffff, 0x00000002, 0x0000000b, 0x00000014,
    0x0000000a, 0x00000003, 0x0000001e, 0x0098967f, 0x00000037, 0x00001388, 0x000000dc, 0x0000003c,
    0x00003390, 0x00000005, 0x00000014, 0x00000032, 0x000000ff, 0x00000096, 0x00000046, 0x00000001,
    0x000000c8, 0x00000000, 0x00000000, 0x00000003, 0x00000002, 0x00000000, 0x00000003, 0x00000004,
    0x00000002, 0x00000000, 0x00000002, 0x00000003, 0x00000002, 0x00000001, 0x00000001, 0x00000003,
    0x00000000, 0x00000001, 0x00000000, 0x00000000, 0x00000000, 0x00000001, 0x00000001, 0x00000003,
    0x00000004, 0x00000002, 0x00000000, 0x00000003, 0x00000001, 0x00000001, 0x00000003, 0x00000000,
    0x00000001, 0x00000001, 0x00000001, 0x00000002, 0x00000001, 0x00000001, 0x00000002, 0x00000000,
    0x00000002, 0x00000003, 0x00000002, 0x00000000, 0x00000000, 0x00000001, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000005, 0x00000003, 0x00000032, 0x00989763, 0x000186a0, 0x00000004,
    0x00000003, 0x00000004, 0x00000000, 0x00000014, 0x00000004, 0x00000000, 0x00000001, 0x00000002,
    0x00000003, 0x08100810, 0x0000001b, 0x00000046, 0x00000006, 0x0000000c, 0x00000006, 0x00000000
};

#include "MonsterAbilityLeafC3/haveChampionFeature.inc"
{
    for (std::vector<int>::size_type i = 0; i < features->size(); ++i)
    {
        if ((*features)[i] == static_cast<int>(feature))
            return true;
    }
    return false;
}

// upgradeAbilityDeathTower —— ORIG 0x088b93bf（定义前置：P210 callee 定义顺序
// 定律——getToleranceActiveStatus 本 TU 已见定义会使 4.4 outgoing 塌缩、全帧
// 槽号漂移；调用者定义必须先于 callee 定义。前置声明不触发塌缩。）
extern float DEATH_TOWER_BONUS[13][4];   // ORIG D 0x93f4c40（定义处 653 行 extern 重复声明，同型合法）
#include "MonsterAbilityLeafC3/getToleranceActiveStatus.inc"
#include "MonsterAbilityLeafC3/upgradeAbilityDeathTower.inc"

#include "MonsterAbilityLeafC3/upgradeAbilityByDifficultyOfDungeon.inc"


// upgradeAbilityBloodSystem —— ORIG 0x088b917b
extern float BLOOD_SYSTEM_BONUS[13][5];   // ORIG D 0x93f4b20（后置定义处重复声明，同型合法）
#include "MonsterAbilityLeafC3/getToleranceActiveStatus_1.inc"
#include "MonsterAbilityLeafC3/upgradeAbilityBloodSystem.inc"


#include "MonsterAbilityLeafC3/getToleranceActiveStatus_2.inc"


#include "MonsterAbilityLeafC3/addChampionFeature.inc"


#include "MonsterAbilityLeafC3/randomSetChampionFeature.inc"


#include "MonsterAbilityLeafC3/scriptSetChampionFeature.inc"


#include "MonsterAbilityLeafC3/makeChampionFeatureList.inc"


#include "MonsterAbilityLeafC3/upgradeAbilityByBossFeature.inc"


#include "MonsterAbilityLeafC3/upgradeAbilityByChampionFeature.inc"


#include "MonsterAbilityLeafC3/getValueInt.inc"


#include "MonsterAbilityLeafC3/getValueFloat.inc"


#include "MonsterAbilityLeafC3/upgradeAbilityByMonsterParameterCategory.inc"


#include "MonsterAbilityLeafC3/upgradeAbilityByDifficultyOfDungeon_1.inc"
                                         int level);

#include "MonsterAbilityLeafC3/upgradeAbilityByDifficultyOfDungeon_2.inc"



// ---------------------------------------------------------------------------
// upgradeAbilityByDifficultyOfDungeon — ORIG 0x88b85cf（3 参版本）：
// MONSTER_DIFFICULTY_BONUS[diff] 行表 size>=level 时按行乘提升，否则同 2 参版。
// ---------------------------------------------------------------------------
extern std::vector<std::vector<float> > MONSTER_DIFFICULTY_BONUS[5];

#include "MonsterAbilityLeafC3/upgradeAbilityByDifficultyOfDungeon_3.inc"


// 逐双字拷贝前 5 个字段（+0x14 m_field18 与 state 均不动）。
// ---------------------------------------------------------------------------
// b28d-solo-MonsterAbilityLeafC3 verdict=IDENTICAL ORIG 0x0830e9d8
// orig/ours=19/19 ndiff=0。ORIG 按值 5 dword 压栈后逐槽直读，字段集/顺序逐条一致
// （P129 尾无 mov this,eax ⇒ void）。
void DungeonRidableObject::operator=(RidableObject other)
{
    m_x = other.m_x;
    m_y = other.m_y;
    m_field0c = other.m_field0c;
    m_field10 = other.m_field10;
    m_field14 = other.m_field14;
    // ORIG 尾无 mov this,eax ⇒ 返回 void（P129）
}

#include "MonsterAbilityLeafC3/SetRidableObject.inc"


// ===========================================================================
// OnlyOrigBatch12：upgradeAbilityBloodSystem / upgradeAbilityByParty /
// upgradeAbilityDeathTower（ORIG T 0x088b917b / 0x088b8e88 / 0x088b93bf）。
// 表：BLOOD_SYSTEM_BONUS（0x93f4b20，72 float）/ DEATH_TOWER_BONUS（0x93f4c40，
// 52 float）定义于 InitScriptProducers_4.cpp；DUNGEON_PARTY_BALANCE_TABLE
// （0x9494f5c）同。ActiveStaticInfo 槽位按反汇编偏移映射（+0x1c 起 17 槽
// 位型访问，同 setBasicAbilityByLevel 先例）。
// ===========================================================================
extern float BLOOD_SYSTEM_BONUS[13][5];   // ORIG D 0x93f4b20
extern float DEATH_TOWER_BONUS[13][4];    // ORIG D 0x93f4c40

// DungeonPartyBalance（InitScriptProducers_4.cpp 同布局：+0xd0/+0xd4 两 float，
// 前置 float[13][4] 线性区；ODR 同定义，仅本 TU 使用）。
struct DungeonPartyBalance
{
    float m_grid[13][4];   // +0x00
    float m_a;             // +0xd0
    float m_b;             // +0xd4
};
extern std::vector<DungeonPartyBalance> DUNGEON_PARTY_BALANCE_TABLE;



#include "MonsterAbilityLeafC3/upgradeAbilityByParty.inc"

