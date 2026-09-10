// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 MonsterAbilityLeafC3/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r Game/ 怪物能力升级域 —— 独占 TU（MonsterAbilityLeafC3）。
//
// 恢复范围（全部为 ORIG 真实符号，nm/objdump 实证；无函数级报告，
// 依据规则先读同类报告 CBattle_Field/set_monster_ability.md、调用方
// BattleMonsterDepsC2.cpp / BattleDepsB2.cpp 后逐指令转写）：
//   - upgradeAbilityByBossFeature            T 0x88b7c24（实为空函数）
//   - upgradeAbilityByChampionFeature        T 0x88b7c29
//   - upgradeAbilityByMonsterParameterCategory T 0x88b7f36
//   - upgradeAbilityByDifficultyOfDungeon    T 0x88b813a
//   - makeChampionFeatureList                T 0x88ba0fd
//   - CRidable::SetRidableObject(RidableScript&) T 0x82fe6d0（报告已完整阅读：
//     docs/class_func_reports/CRidable/SetRidableObject.md ## 汇编+## 反编译 C）
// 及其依赖链（同为 ORIG 符号）：
//   - haveChampionFeature                    T 0x8891ad4
//   - getToleranceActiveStatus               T 0x8891b2a
//   - randomSetChampionFeature               T 0x88b9dcf
//   - scriptSetChampionFeature               T 0x88b9f12
//   - addChampionFeature                     T 0x88b9f66
//   - STMonsterParameterCategory::getValueInt/getValueFloat W 0x88ba480/0x88ba4a2
//   - DungeonRidableObject::operator=(RidableObject)        W 0x830e9d8
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

// ---------------------------------------------------------------------------
// haveChampionFeature — ORIG 0x8891ad4：线性查找。
// ---------------------------------------------------------------------------
#include "MonsterAbilityLeafC3/haveChampionFeature.inc"
}

// upgradeAbilityDeathTower —— ORIG 0x088b93bf（定义前置：P210 callee 定义顺序
// 定律——getToleranceActiveStatus 本 TU 已见定义会使 4.4 outgoing 塌缩、全帧
// 槽号漂移；调用者定义必须先于 callee 定义。前置声明不触发塌缩。）
extern float DEATH_TOWER_BONUS[13][4];   // ORIG D 0x93f4c40（定义处 653 行 extern 重复声明，同型合法）
#include "MonsterAbilityLeafC3/getToleranceActiveStatus.inc"
#include "MonsterAbilityLeafC3/upgradeAbilityDeathTower.inc"
}
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// upgradeAbilityByDifficultyOfDungeon — ORIG 0x88b8c37（2 参版本）：
// 仅按 DIFFICULTY_OF_DUNGEON_BONUS 全局表（扁平 [d+off]）提升，无行乘。
// ---------------------------------------------------------------------------
#include "MonsterAbilityLeafC3/upgradeAbilityByDifficultyOfDungeon.inc"
}

// upgradeAbilityBloodSystem —— ORIG 0x088b917b
extern float BLOOD_SYSTEM_BONUS[13][5];   // ORIG D 0x93f4b20（后置定义处重复声明，同型合法）
#include "MonsterAbilityLeafC3/getToleranceActiveStatus_1.inc"
#include "MonsterAbilityLeafC3/upgradeAbilityBloodSystem.inc"
}

// getToleranceActiveStatus — ORIG 0x8891b2a：跳转表 0x8e040f4（17 项）。
#include "MonsterAbilityLeafC3/getToleranceActiveStatus_2.inc"
}

// ---------------------------------------------------------------------------
// addChampionFeature — ORIG 0x88b9f66：同组(fc/ fd/fe/ff)冲突拒绝 → 重复拒绝 →
// 点数足够才 push_back 并扣点。
// ---------------------------------------------------------------------------
// b28d-solo-MonsterAbilityLeafC3 verdict=AE ORIG 0x088b9f66 orig/ours=121/121。
// 修复：表符号 _ZL21CHAMPFEAT_POINT_MINUS（P204③ 文件级 static）、形参直读
// （P0：去 f/fGroup/group 局部）、每条件独立重调 operator[]（P157/P48）、含调用的
// || 链由 4.4 物化为 mov$1/mov$0/test/jne 岛（探针 q1/q2）、表值直算两次不缓存、
// FAIL/END 尾标签形。
#include "MonsterAbilityLeafC3/addChampionFeature.inc"
}

// ---------------------------------------------------------------------------
// randomSetChampionFeature — ORIG 0x88b9dcf：srand(seed)；
// diff>3 直接取池[28+rand()%7]；否则 3700 次尝试内按过滤规则随机添加。
// 过滤：>0x30007 时仅 [0x3fe09,0x3fe0c] 与 0x40008 跳过；≤0x30007 且 ≥0x30004
// 跳过；等于 0x1ff00/0x20013/0xff01 跳过；其余添加。count<=1 提前终止。
// ---------------------------------------------------------------------------
// b28d-solo-MonsterAbilityLeafC3 verdict=DIFF[N18b/P75] ORIG 0x088b9dcf
// orig/ours=109/111 ndiff=56。已复现：do-while（体尾 subl/cmpl/setg/test/jne）、
// 6 路 continue 型 nop 岛（P223②/P51）、表符号 _ZL21CHAMPION_FEATURE_LIST（P204③）、
// diff>3 分支槽位（-0x10/-0x14/-0xc）。残余：4.4 -O0 对 int 局部与立即数比较恒折
// mem 直比（P209/N18b），ORIG 4.1 为单次 mov 后寄存器复用；实参装载 eax↔edx（P75）；
// jge→jg / jl→jle 常量规范化。语义等价封顶。
#include "MonsterAbilityLeafC3/randomSetChampionFeature.inc"
}

// ---------------------------------------------------------------------------
// scriptSetChampionFeature — ORIG 0x88b9f12：对脚本列表逐项 addChampionFeature。
// ---------------------------------------------------------------------------
#include "MonsterAbilityLeafC3/scriptSetChampionFeature.inc"
}

// ---------------------------------------------------------------------------
// makeChampionFeatureList — ORIG 0x88ba0fd。
// count = trunc((level-1)*0.2f + 2.0f)（常量 .rodata 0x8e084b8=0.2f /
// 0x8e084a0=2.0f）；脚本列表为空走随机池，否则按脚本逐项登记。
#include "MonsterAbilityLeafC3/makeChampionFeatureList.inc"
}

// ---------------------------------------------------------------------------
// upgradeAbilityByBossFeature — ORIG 0x88b7c24 仅 push ebp/mov/pop/ret，
// 即空函数（链接边界保留）。
// ---------------------------------------------------------------------------
#include "MonsterAbilityLeafC3/upgradeAbilityByBossFeature.inc"
}

// ---------------------------------------------------------------------------
// upgradeAbilityByChampionFeature — ORIG 0x88b7c29。
// 角色系数表为函数内静态初值（ORIG 栈上立即数构造，与 ORIG 数值一致）：
//   elem 0..3：elemAttack[e] += (150-old)*ratioA[role]
//   elem == 4：attack/magicAttack *= ratioB[role]
//   公共：hpBase *= ratioC[role]；随后按特性调整 statBase*；末尾 attr17Base[13]+=50。
// 偏移实证：m_elemAttack@+0x38、m_attack@+0x18、m_magicAttack@+0x20、
// m_hpBase@+0x04、statBase3/4/6@+0x118/+0x120/+0x130、statBase11..14@
// +0x158/+0x160/+0x168/+0x170、attr17Base[13]=+0xac（ActiveStaticInfo.h 布局）。
// ---------------------------------------------------------------------------
// b28d-solo-MonsterAbilityLeafC3 verdict=DIFF[P209/N18b] ORIG 0x088b7c29
// orig/ours=229/223 ndiff=164。已复现：12 条栈上 float 常量初始化（非 static 局部
// const 数组）、5 处 haveChampionFeature + 立即数实参、fadd st,st 加倍形、lea 加 50。
// 残余：4.4 对 elem 与立即数比较恒 mem 直比（ORIG 单次 mov+test/cmp 复用 %eax）、
// IF/ELSEIF 块序镜像（P47）、索引写回 add-vs-lea（P30/N4）。语义等价封顶。
// [L14 round2 修] P251/P286 switch 化：if/else-if → `switch(elem){case 0..3 合并; case 4;}`
// ⇒ 复现 ORIG `mov 0x8(%ebp),%eax; test; js; cmp $3; jle; cmp $4; je` 寄存器比较链，
// gap **−6→0**（229/229）、差异块 5→2、strict 33→21（无回退）。余槽整块浅 4 ⇒ N126。
// [D07 round5 收口] AE 达成（229/229，verdict DIFF→AE）：补第二枚具名局部
// `const int newValue`（ORIG 0x088b7cf2 `fistpl -0xc(%ebp)` 独立结果槽 + 0x088b7cf8
// `mov -0xc(%ebp),%edx` 写回；旧写把 ROUND 结果复用 -0x4c 临时槽 ⇒ 12×float 数组
// 基址浅 4（-0x40→-0x3c）且初始化块整体错位）。补 `oldValue`+`newValue` 两槽后
// 数组基址与 ORIG 对齐、ROUND 段逐条全等（P0 槽-局部双向律）。
#include "MonsterAbilityLeafC3/upgradeAbilityByChampionFeature.inc"
}

// ---------------------------------------------------------------------------
// STMonsterParameterCategory 访问器 — ORIG W 0x88ba480 / 0x88ba4a2。
// ---------------------------------------------------------------------------
#include "MonsterAbilityLeafC3/getValueInt.inc"
}

#include "MonsterAbilityLeafC3/getValueFloat.inc"
}

// ---------------------------------------------------------------------------
// upgradeAbilityByMonsterParameterCategory — ORIG 0x88b7f36。
// 先扫描是否存在 id==13（hasCat13）；再逐条目：解析目标字段后——
//   hasCat13&&isChampion&&id==10 跳过；hasCat13&&!isChampion&&id==13 跳过；
//   m_field4!=0 → 目标 = trunc(target_f * getValueFloat())；
//   否则 → 目标 += getValueInt()（即 m_field8/100）。
// ---------------------------------------------------------------------------
// b28d-solo-MonsterAbilityLeafC3 verdict=AE ORIG 0x088b7f36 orig/ours=163/163。
// 修复：第一循环引用局部（P157 反面：消 sete 物化 + 恢复 -0x18 槽）、switch case
// 书写序 = ORIG 跳转表叶块排放序（P46：0..5,10,11,12,6..9,13）、删 default、
// 三守卫合并为单 || goto 尾标签（消 3 个 continue 中继岛）。case→字段映射与
// Ghidra 跳转表一致（0..13 全核，无字段错位）。
#include "MonsterAbilityLeafC3/upgradeAbilityByMonsterParameterCategory.inc"
}

// ---------------------------------------------------------------------------
// upgradeAbilityByDifficultyOfDungeon — ORIG 0x88b813a（4 参版本；level 参数
// 在本重载中未使用）。table 为副本难度表，行访问除 +0x118 一处使用 operator[]
// 外均为 at()（保留 ORIG 的越界抛出语义）；BONUS 扁平索引见各注释。
// ---------------------------------------------------------------------------
// b28d-solo-MonsterAbilityLeafC3 verdict=AE ORIG 0x088b813a orig/ours=330/330。
// 真语义修复：attr17Base[13]/statBase6/attr17Base[i]/statBase3 四项原先多乘了字段
// 自身（ORIG 仅 m_hpBase/statBase4/statBase11..14 乘字段）；表空时须回落调用 3 参版
// （ORIG 尾 call _Z35upgradeAbilityByDifficultyOfDungeoniP16ActiveStaticInfoi）。
// 形：内联乘算（删 scaleByBonus helper）、BONUS 全局直接索引、`if (!table.empty())`
// 取反守卫（P292）、循环 `!= 0x12` 正向形（setne/je）、行访问 at()/operator[] 照抄。
#include "MonsterAbilityLeafC3/upgradeAbilityByDifficultyOfDungeon_1.inc"

#include "MonsterAbilityLeafC3/upgradeAbilityByDifficultyOfDungeon_2.inc"
}


// ---------------------------------------------------------------------------
// upgradeAbilityByDifficultyOfDungeon — ORIG 0x88b85cf（3 参版本）：
// MONSTER_DIFFICULTY_BONUS[diff] 行表 size>=level 时按行乘提升，否则同 2 参版。
// ---------------------------------------------------------------------------
extern std::vector<std::vector<float> > MONSTER_DIFFICULTY_BONUS[5];

#include "MonsterAbilityLeafC3/upgradeAbilityByDifficultyOfDungeon_3.inc"
}

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

// ---------------------------------------------------------------------------
// CRidable::SetRidableObject — ORIG 0x82fe6d0（报告 SetRidableObject.md 全文）。
// script.count<=0 直接返回；写 +0x18 字节=script.m_4、+0x1c=script.m_0；
// count < 对象数：复制对象池，每次 random_shuffle 后取 back() 弹出插入
// map（key=第 3 字段）；否则顺序插入全部对象（注意循环条件是 size 而非 count）。
// state 统一置 4（ORIG 在 operator= 之后单独写 +0x18=4）。
// ---------------------------------------------------------------------------
// b28d-solo-MonsterAbilityLeafC3 verdict=DIFF[P75/P47] ORIG 0x082fe6d0
// orig/ours=193/194 ndiff=48。真语义修复：IF(count<size) 与 ELSE 两分支的循环上界/
// 对象来源原先互换（ORIG IF=顺序遍历 m_objects[i]（i<size）、ELSE=拷贝 pool 随机抽取
// （i<count））；obj/tmp 提到函数头各构造一次（ORIG local_c8/local_e0）；
// make_pair→std::pair<int,DungeonRidableObject> 直构（P192）。p167 调用点差集
// CLEAR（0 missing/0 extra，21=21）。残余：m_flag18 装载多 1 条 regmove（N18a）、
// 条件块序镜像（P47）、operator[] 实参装载序/lea-vs-add（P75/P30）。语义等价封顶。
#include "MonsterAbilityLeafC3/SetRidableObject.inc"
}

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



// upgradeAbilityByParty —— ORIG 0x088b8e88：job 0..3 截断；首个满足
// level>=m_a && level==m_b 的 balance 应用后立即返回。
// b28d-solo-MonsterAbilityLeafC3 verdict=DIFF[N85/N121] ORIG 0x088b8e88
// orig/ours=229/229 ndiff=40。已复现：m_grid[row][job] 二维索引（行=偏移/4，复现
// add+scaled 而非指针算术 shl 链，P311/P30）、job 截断、do-while 结构、表全局直接
// 传址。残余：x87 比较 lowering（ORIG fucompp/fnstsw/sahf vs 4.4 fucomip/fstp，
// N85 跨版本）+ 命中 return 的 epilogue 尾岛（N121）。语义等价封顶。
// [D07 试形记录 2026-09-10] ①`-march=i486 -mtune=generic` 手工追加旗标重编本 TU：
// party/rand/ridable 三件 verdict 逐字不变（我方仍 `fucomip %st(1),%st; fstp`）⇒
// 本条 x87 比较 lowering 不受 march 控制，非 P230 补所指的 fnstsw 族，封 N85。
// ②条件极性两形（`level < m_a` / `!(m_a <= level)`）实测同产 `fildl;flds;fucomip;
// fstp;seta`；ORIG 的 `fxch;fucompp;fnstsw %ax;sahf;setae` 无源形可达。
#include "MonsterAbilityLeafC3/upgradeAbilityByParty.inc"
}
