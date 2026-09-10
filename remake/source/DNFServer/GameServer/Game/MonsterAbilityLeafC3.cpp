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
bool haveChampionFeature(std::vector<int>* features, CHAMPION_FEATURES feature)
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
int getToleranceActiveStatus(ENUM_ACTIVEPROPERTY prop);
void upgradeAbilityDeathTower(int idx, ActiveStaticInfo* info)
{
    // 槽基垫形（探针 /tmp/l35_probe2.cpp v1）：4.4 槽域需 +0x14 才与 ORIG
    // （slot@-0x1c / cw@-0x1e·-0x20 / 循环 i@-0xc，帧 0x38）同槽；引用型
    // double 4B pad 位于槽域底使后续槽整体 -4，其余 +0x10 转发为 outgoing。
    // i 声明需先于首次赋值，否则 4.4 零行删守卫并转 i 下 -4（级联全帧）。
    // 关键：本 TU 中 callee getToleranceActiveStatus(129 行) 定义已见 → 4.4 按
    // 其真实 outgoing 塌缩本帧（P210）。本函数定义须位于其之前。独立 TU 验证
    // /tmp/l35_dt_tu.cpp（真实函数体 + 声明）= ORIG 帧 0x38 槽 -0x1c。
    double framePad; (void)&framePad;
    int i;
    // ORIG 0x088b93bf：全局表直接索引（无 tbl 指针变量）。
    info->m_hpBase = static_cast<int>(static_cast<float>(info->m_hpBase) *
                                      DEATH_TOWER_BONUS[0][idx + 0x10]);
    info->m_attr17Base[13] += static_cast<int>(DEATH_TOWER_BONUS[0][idx + 0xc]);
    info->m_statBase6 += static_cast<int>(DEATH_TOWER_BONUS[0][idx + 0x28]);
    for (i = 0; i <= 0x10; ++i)
    {
        if (getToleranceActiveStatus(static_cast<ENUM_ACTIVEPROPERTY>(i)) != 0x12)
            info->m_attr17Base[i] += static_cast<int>(DEATH_TOWER_BONUS[0][idx + 0x1c]);
    }
    info->m_statBase4 = static_cast<int>(static_cast<float>(info->m_statBase4) *
                                         DEATH_TOWER_BONUS[0][idx + 0x14]);
    info->m_statBase3 += static_cast<int>(DEATH_TOWER_BONUS[0][idx + 0x18]);
    info->m_statBase11 = static_cast<int>(static_cast<float>(info->m_statBase11) *
                                          DEATH_TOWER_BONUS[0][idx + 0x20]);
    info->m_statBase13 = static_cast<int>(static_cast<float>(info->m_statBase13) *
                                          DEATH_TOWER_BONUS[0][idx + 0x20]);
    info->m_statBase12 = static_cast<int>(static_cast<float>(info->m_statBase12) *
                                          DEATH_TOWER_BONUS[0][idx + 0x24]);
    info->m_statBase14 = static_cast<int>(static_cast<float>(info->m_statBase14) *
                                          DEATH_TOWER_BONUS[0][idx + 0x24]);
}
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// upgradeAbilityByDifficultyOfDungeon — ORIG 0x88b8c37（2 参版本）：
// 仅按 DIFFICULTY_OF_DUNGEON_BONUS 全局表（扁平 [d+off]）提升，无行乘。
// ---------------------------------------------------------------------------
void upgradeAbilityByDifficultyOfDungeon(int diff, ActiveStaticInfo* info)
{
    // ORIG 0x88b8c37：全局表直接索引（无 bonus 指针变量）。
    // +0x04 m_hpBase = trunc(hpBase*BONUS[d+0x14])
    info->m_hpBase = static_cast<int>(
        static_cast<float>(info->m_hpBase) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x14]);
    // +0xac attr17Base[13] += BONUS[d+0x0f]
    info->m_attr17Base[13] += static_cast<int>(DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x0f]);
    // +0x130 statBase6 += BONUS[d+0x32]
    info->m_statBase6 += static_cast<int>(DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x32]);
    // +0x78+i*4 attr17Base[i] +=（i=0..16；容错映射 0x12 跳过；
    // ORIG 0x88b8c37 用 setne+je 形式，即 if (x != 0x12) 正写）
    for (int i = 0; i <= 0x10; ++i)
    {
        if (getToleranceActiveStatus(static_cast<ENUM_ACTIVEPROPERTY>(i)) != 0x12)
            info->m_attr17Base[i] +=
                static_cast<int>(DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x23]);
    }
    // +0x120 statBase4 = trunc(statBase4*BONUS[d+0x19])
    info->m_statBase4 = static_cast<int>(
        static_cast<float>(info->m_statBase4) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x19]);
    // +0x118 statBase3 += （非零守卫）
    if (info->m_statBase3 != 0)
        info->m_statBase3 += static_cast<int>(DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x1e]);
    // +0x158/+0x168 statBase11/13 = trunc(*BONUS[d+0x28])
    info->m_statBase11 = static_cast<int>(
        static_cast<float>(info->m_statBase11) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x28]);
    info->m_statBase13 = static_cast<int>(
        static_cast<float>(info->m_statBase13) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x28]);
    // +0x160/+0x170 statBase12/14 = trunc(*BONUS[d+0x2d])
    info->m_statBase12 = static_cast<int>(
        static_cast<float>(info->m_statBase12) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x2d]);
    info->m_statBase14 = static_cast<int>(
        static_cast<float>(info->m_statBase14) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x2d]);
}

// upgradeAbilityBloodSystem —— ORIG 0x088b917b
extern float BLOOD_SYSTEM_BONUS[13][5];   // ORIG D 0x93f4b20（后置定义处重复声明，同型合法）
int getToleranceActiveStatus(ENUM_ACTIVEPROPERTY prop);
void upgradeAbilityBloodSystem(int idx, ActiveStaticInfo* info)
{
    // ORIG 0x088b917b：全局表直接索引（无 tbl 指针变量）。
    info->m_hpBase = static_cast<int>(static_cast<float>(info->m_hpBase) *
                                      BLOOD_SYSTEM_BONUS[0][idx + 0x14]);
    info->m_attr17Base[13] += static_cast<int>(BLOOD_SYSTEM_BONUS[0][idx + 0xf]);
    info->m_statBase6 += static_cast<int>(BLOOD_SYSTEM_BONUS[0][idx + 0x32]);
    for (int i = 0; i <= 0x10; ++i)
    {
        if (getToleranceActiveStatus(static_cast<ENUM_ACTIVEPROPERTY>(i)) != 0x12)
            info->m_attr17Base[i] += static_cast<int>(BLOOD_SYSTEM_BONUS[0][idx + 0x23]);
    }
    info->m_statBase4 = static_cast<int>(static_cast<float>(info->m_statBase4) *
                                         BLOOD_SYSTEM_BONUS[0][idx + 0x19]);
    info->m_statBase3 += static_cast<int>(BLOOD_SYSTEM_BONUS[0][idx + 0x1e]);
    info->m_statBase11 = static_cast<int>(static_cast<float>(info->m_statBase11) *
                                          BLOOD_SYSTEM_BONUS[0][idx + 0x28]);
    info->m_statBase13 = static_cast<int>(static_cast<float>(info->m_statBase13) *
                                          BLOOD_SYSTEM_BONUS[0][idx + 0x28]);
    info->m_statBase12 = static_cast<int>(static_cast<float>(info->m_statBase12) *
                                          BLOOD_SYSTEM_BONUS[0][idx + 0x2d]);
    info->m_statBase14 = static_cast<int>(static_cast<float>(info->m_statBase14) *
                                          BLOOD_SYSTEM_BONUS[0][idx + 0x2d]);
}

// getToleranceActiveStatus — ORIG 0x8891b2a：跳转表 0x8e040f4（17 项）。
int getToleranceActiveStatus(ENUM_ACTIVEPROPERTY prop)
{
    switch (static_cast<int>(prop))
    {
    // case 书写序 = ORIG 叶块排放序（P46）：0xf,0x10,0..0xa,0x11,0xb，
    // 同值 0x12 的 12/13/default 连续书写合并为单叶直落。
    case 14: return 0x0f;
    case 15: return 0x10;
    case 0: return 0x00;
    case 1: return 0x01;
    case 2: return 0x02;
    case 3: return 0x03;
    case 4: return 0x04;
    case 5: return 0x05;
    case 6: return 0x06;
    case 7: return 0x07;
    case 8: return 0x08;
    case 9: return 0x09;
    case 10: return 0x0a;
    case 16: return 0x11;
    case 11: return 0x0b;
    case 12: case 13: default: return 0x12;  // 调用方以 0x12 跳过；合并单叶直落
    }
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
void addChampionFeature(CHAMPION_FEATURES feature, int& count,
                        std::vector<int>& list)
{
    // ORIG 0x088b9f66：形参直读（P0：无 f/fGroup/group 具名局部），每个条件
    // 独立重调 operator[]（P157/P48），含调用的 || 链由 4.4 物化成
    // mov $1/mov $0/test/jne 岛（探针 q1/q2 实证）；末尾表值直算两次（不缓存）。
    for (std::vector<int>::size_type i = 0; i < list.size(); ++i)
    {
        if (((((list[i] & 0xff00) >> 8) == 0xff) &&
             (((static_cast<int>(feature) & 0xff00) >> 8) == 0xff)) ||
            ((((list[i] & 0xff00) >> 8) == 0xfe) &&
             (((static_cast<int>(feature) & 0xff00) >> 8) == 0xfe)) ||
            ((((list[i] & 0xff00) >> 8) == 0xfd) &&
             (((static_cast<int>(feature) & 0xff00) >> 8) == 0xfd)) ||
            ((((list[i] & 0xff00) >> 8) == 0xfc) &&
             (((static_cast<int>(feature) & 0xff00) >> 8) == 0xfc)))
            return;
        if (list[i] == static_cast<int>(feature))
            return;
    }

    if (count <
        CHAMPFEAT_POINT_MINUS[(static_cast<int>(feature) & 0xff0000) >> 16])
        goto FAIL;
    list.push_back(static_cast<int>(feature));
    count -= CHAMPFEAT_POINT_MINUS[(static_cast<int>(feature) & 0xff0000) >> 16];
    goto END;
FAIL: ;
END: ;
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
void randomSetChampionFeature(int seed, int& count, std::vector<int>& temp,
                              int diff)
{
    std::srand(static_cast<unsigned int>(seed));

    if (diff > 3)
    {
        const int f = CHAMPION_FEATURE_LIST[rand() % 7 + 0x1c];
        addChampionFeature(static_cast<CHAMPION_FEATURES>(f), count, temp);
        return;
    }

    // ORIG 0x088b9dcf：do-while（体尾 subl/cmpl/setg/test/jne）+ 六路
    // continue 型 nop 岛（P223②/P51）；过滤判定为纯跳转链，无 bool 槽。
    int remain = 0xe74;
    do
    {
        if (count <= 1)
            return;

        // [试形记录] `register int f` 实测与无 register 逐字节相同（P307 提示对本形
        // 零效）⇒ 维持普通具名局部；cmp 形态差归 P209/N18b 封顶（见函数头判词）。
        const int f = CHAMPION_FEATURE_LIST[rand() % 28];
        if (f > 0x30007)
            goto BIG;
        if (f >= 0x30004)
            goto SKIP1;
        if (f == 0x1ff00)
            goto SKIP2;
        if (f == 0x20013)
            goto SKIP3;
        if (f == 0xff01)
            goto SKIP4;
        goto ADD;
    BIG:
        if (f < 0x3fe09)
            goto ADD;
        if (f <= 0x3fe0c)
            goto SKIP5;
        if (f == 0x40008)
            goto SKIP6;
    ADD:
        addChampionFeature(static_cast<CHAMPION_FEATURES>(f), count, temp);
        goto NEXT;
    SKIP1:
        goto NEXT;
    SKIP2:
        goto NEXT;
    SKIP3:
        goto NEXT;
    SKIP4:
        goto NEXT;
    SKIP5:
        goto NEXT;
    SKIP6:
        goto NEXT;
    NEXT: ;
    } while (--remain > 0);
}

// ---------------------------------------------------------------------------
// scriptSetChampionFeature — ORIG 0x88b9f12：对脚本列表逐项 addChampionFeature。
// ---------------------------------------------------------------------------
void scriptSetChampionFeature(std::vector<int>& scriptList, int& count,
                              std::vector<int>& temp)
{
    for (std::vector<int>::size_type i = 0; i < scriptList.size(); ++i)
        addChampionFeature(static_cast<CHAMPION_FEATURES>(scriptList[i]),
                           count, temp);
}

// ---------------------------------------------------------------------------
// makeChampionFeatureList — ORIG 0x88ba0fd。
// count = trunc((level-1)*0.2f + 2.0f)（常量 .rodata 0x8e084b8=0.2f /
// 0x8e084a0=2.0f）；脚本列表为空走随机池，否则按脚本逐项登记。
void makeChampionFeatureList(std::vector<int>& features, int level, int seed,
                             std::vector<int>& temp, int diff)
{
    int count =
        static_cast<int>(static_cast<float>(level - 1) * 0.2f + 2.0f);

    // [b28c-lane24] ORIG 0x088ba0fd：`call size(); test %eax,%eax; sete %al;
    // test %al,%al; je`（P197④ call 结果寄存器直比物化）；empty() 在 4.4-O0
    // 生 out-of-line empty 调用且无 sete 段 ⇒ 改显式 size()==0。
    if (features.size() == 0)
        randomSetChampionFeature(seed, count, temp, diff);
    else
        scriptSetChampionFeature(features, count, temp);
}

// ---------------------------------------------------------------------------
// upgradeAbilityByBossFeature — ORIG 0x88b7c24 仅 push ebp/mov/pop/ret，
// 即空函数（链接边界保留）。
// ---------------------------------------------------------------------------
void upgradeAbilityByBossFeature(ActiveStaticInfo* /*info*/)
{
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
void upgradeAbilityByChampionFeature(int elem, int roleType,
                                     std::vector<int>* features,
                                     ActiveStaticInfo* info)
{
    // ORIG 0x088b7c29 开头 12 条 mov $imm -> 栈槽 ⇒ 原码是**非 static** 局部
    // const 数组（栈上逐元素初始化），不是 .rodata 静态表。
    const float kElemRatio[4] = {0.0f, 0.3f, 0.5f, 0.0f};
    const float kAttackRatio[4] = {0.0f, 1.5f, 2.0f, 0.0f};
    const float kHpRatio[4] = {0.0f, 1.5f, 3.0f, 0.0f};

    // [L14 round2] P251/P286：ORIG 0x088b7c29 该段为
    // `mov 0x8(%ebp),%eax; test %eax,%eax; js; cmp $0x3,%eax; jle; cmp $0x4,%eax; je`
    // （形参载入寄存器一次 + 多路比较链，无栈槽往返）⇒ 原码是 switch(elem) 的
    // 0..3 合并 case + case 4，而非 if/else-if（4.4 对 if 链恒折 `cmpl $0,%ebp` 直比）。
    switch (elem)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    {
        const int oldValue = info->m_elemAttack[elem];
        // ORIG 0x088b7cbf..0x088b7d04：oldValue 与 ROUND 结果各占独立具名槽
        // （Ghidra local_14@-0x10 / local_10@-0xc，写回用后者），P0 槽-局部双向律。
        const int newValue = static_cast<int>(
            static_cast<float>(oldValue) +
            (150 - oldValue) * kElemRatio[roleType]);
        info->m_elemAttack[elem] = newValue;
        break;
    }
    case 4:
        info->m_attack =
            static_cast<int>(static_cast<float>(info->m_attack) *
                             kAttackRatio[roleType]);
        info->m_magicAttack =
            static_cast<int>(static_cast<float>(info->m_magicAttack) *
                             kAttackRatio[roleType]);
        break;
    default:
        break;
    }

    info->m_hpBase = static_cast<int>(static_cast<float>(info->m_hpBase) *
                                      kHpRatio[roleType]);

    if (haveChampionFeature(features, static_cast<CHAMPION_FEATURES>(0x2000d)))
        info->m_statBase3 =
            static_cast<int>(static_cast<float>(info->m_statBase3) * 1.5f);
    if (haveChampionFeature(features, static_cast<CHAMPION_FEATURES>(0x3000e)))
        info->m_statBase4 = static_cast<int>(
            static_cast<float>(info->m_statBase4) +
            static_cast<float>(info->m_statBase4));  // fadd st,st
    if (haveChampionFeature(features, static_cast<CHAMPION_FEATURES>(0x2ff02)))
        info->m_statBase12 =
            static_cast<int>(static_cast<float>(info->m_statBase12) * 2.5f);
    if (haveChampionFeature(features, static_cast<CHAMPION_FEATURES>(0x2ff03)))
        info->m_statBase14 =
            static_cast<int>(static_cast<float>(info->m_statBase14) * 2.5f);
    if (haveChampionFeature(features, static_cast<CHAMPION_FEATURES>(0x3000f)))
        info->m_statBase6 = static_cast<int>(
            static_cast<float>(info->m_statBase6) +
            static_cast<float>(info->m_statBase6));  // fadd st,st

    info->m_attr17Base[13] += 50;                    // +0xac
}

// ---------------------------------------------------------------------------
// STMonsterParameterCategory 访问器 — ORIG W 0x88ba480 / 0x88ba4a2。
// ---------------------------------------------------------------------------
int STMonsterParameterCategory::getValueInt() const
{
    return m_field8 / 100;
}

float STMonsterParameterCategory::getValueFloat() const
{
    return static_cast<float>(m_field8) / 100.0f;
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
void upgradeAbilityByMonsterParameterCategory(
    const std::vector<STMonsterParameterCategory>* categories,
    ActiveStaticInfo* info, bool isChampion)
{
    bool hasCat13 = false;
    for (std::vector<STMonsterParameterCategory>::size_type i = 0;
         i < categories->size(); ++i)
    {
        // ORIG 有引用局部槽 -0x18（P157 反面：引用局部保留 operator[] 一次）；
        // 直接内联 .m_field0 会使 4.4 物化 sete（含 call 条件）。
        const STMonsterParameterCategory& cat = (*categories)[i];
        if (cat.m_field0 == 13)
        {
            hasCat13 = true;
            break;
        }
    }

    for (std::vector<STMonsterParameterCategory>::size_type j = 0;
         j < categories->size(); ++j)
    {
        const STMonsterParameterCategory& cat = (*categories)[j];
        // P167：ORIG 为内联 switch（无需独立 helper，§4.1），字段映射直接内联。
        // case 书写序 = ORIG 跳转表叶块排放序（P46）：0..5,10,11,12,6..9,13。
        int* target = 0;
        switch (cat.m_field0)
        {
        case 0:  target = &info->m_statBase4;       break; // +0x120
        case 1:  target = &info->m_statBase3;       break; // +0x118
        case 2:  target = &info->m_attack;          break; // +0x18
        case 3:  target = &info->m_magicAttack;     break; // +0x20
        case 4:  target = &info->m_defense;         break; // +0x28
        case 5:  target = &info->m_magicDefense;    break; // +0x30
        case 10: target = &info->m_hpBase;          break; // +0x04
        case 11: target = &info->m_statBase6;       break; // +0x130
        case 12: target = &info->m_attr17Base[13];  break; // +0xac
        case 6:  target = &info->m_statBase11;      break; // +0x158
        case 7:  target = &info->m_statBase12;      break; // +0x160
        case 8:  target = &info->m_statBase13;      break; // +0x168
        case 9:  target = &info->m_statBase14;      break; // +0x170
        case 13: target = &info->m_hpBase;          break; // +0x04
        }
        if (target == 0 ||
            (hasCat13 && isChampion && cat.m_field0 == 10) ||
            (hasCat13 && !isChampion && cat.m_field0 == 13))
            goto NEXT;

        if (cat.m_field4 != 0)
        {
            *target = static_cast<int>(static_cast<float>(*target) *
                                       cat.getValueFloat());
            goto NEXT;
        }
        *target += cat.getValueInt();
    NEXT: ;
    }
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
void upgradeAbilityByDifficultyOfDungeon(int diff, ActiveStaticInfo* info,
                                         int level);

void upgradeAbilityByDifficultyOfDungeon(
    int diff, ActiveStaticInfo* info, int level,
    const std::vector<std::vector<float> >& table)
{
    (void)level;  // ORIG 本体未读取 0x10(%ebp)
    // ORIG 0x088b813a：empty 守卫为 `call empty; xor $1; test; je <body>`（P292
    // bool 取反形 ⇒ 源码 `if (!table.empty())`）；乘算全内联（无 helper call），
    // BONUS 表全局直接索引（无 bonus 指针局部），行访问 at()/operator[] 照抄。
    if (!table.empty())
    {
        info->m_hpBase = static_cast<int>(
            static_cast<float>(info->m_hpBase) *
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x14] *
            table.at(4).at(diff));
        info->m_attr17Base[13] += static_cast<int>(
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x0f] *
            table.at(3).at(diff));
        info->m_statBase6 += static_cast<int>(
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x32] *
            table.at(10).at(diff));
        for (int i = 0; i <= 0x10; ++i)
        {
            if (getToleranceActiveStatus(static_cast<ENUM_ACTIVEPROPERTY>(i)) !=
                0x12)
                info->m_attr17Base[i] += static_cast<int>(
                    DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x23] *
                    table.at(7).at(diff));
        }
        info->m_statBase4 = static_cast<int>(
            static_cast<float>(info->m_statBase4) *
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x19] *
            table.at(5).at(diff));
        if (info->m_statBase3 != 0)
            info->m_statBase3 += static_cast<int>(
                DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x1e] * table[6][diff]);
        info->m_statBase11 = static_cast<int>(
            static_cast<float>(info->m_statBase11) *
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x28] *
            table.at(8).at(diff));
        info->m_statBase13 = static_cast<int>(
            static_cast<float>(info->m_statBase13) *
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x28] *
            table.at(8).at(diff));
        info->m_statBase12 = static_cast<int>(
            static_cast<float>(info->m_statBase12) *
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x2d] *
            table.at(9).at(diff));
        info->m_statBase14 = static_cast<int>(
            static_cast<float>(info->m_statBase14) *
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x2d] *
            table.at(9).at(diff));
        return;
    }
    // ORIG 0x088b813a 尾部 `call _Z35upgradeAbilityByDifficultyOfDungeoniP16ActiveStaticInfoi`
    // ⇒ 表空时回落到 3 参版（0x088b85cf），非内联展开。
    upgradeAbilityByDifficultyOfDungeon(diff, info, level);
}


// ---------------------------------------------------------------------------
// upgradeAbilityByDifficultyOfDungeon — ORIG 0x88b85cf（3 参版本）：
// MONSTER_DIFFICULTY_BONUS[diff] 行表 size>=level 时按行乘提升，否则同 2 参版。
// ---------------------------------------------------------------------------
extern std::vector<std::vector<float> > MONSTER_DIFFICULTY_BONUS[5];

void upgradeAbilityByDifficultyOfDungeon(int diff, ActiveStaticInfo* info,
                                         int level)
{
    // ORIG 0x88b85cf：全局表直接索引（无 bonus 指针变量）。
    if (MONSTER_DIFFICULTY_BONUS[diff].size() >= static_cast<unsigned int>(level))
    {
        std::vector<float>& row =
            MONSTER_DIFFICULTY_BONUS[diff].at(level - 1);   // ORIG 非 const at

        info->m_hpBase = static_cast<int>(
            static_cast<float>(info->m_hpBase) *
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x14] * row.at(4));
        info->m_attr17Base[13] += static_cast<int>(
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x0f] * row.at(3));
        info->m_statBase6 += static_cast<int>(
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x32] * row.at(10));
        for (int i = 0; i <= 0x10; ++i)
        {
            if (getToleranceActiveStatus(static_cast<ENUM_ACTIVEPROPERTY>(i)) != 0x12)
                info->m_attr17Base[i] += static_cast<int>(
                    DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x23] * row.at(7));
        }
        info->m_statBase4 = static_cast<int>(
            static_cast<float>(info->m_statBase4) *
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x19] * row.at(5));
        if (info->m_statBase3 != 0)
            info->m_statBase3 += static_cast<int>(
                DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x1e] * row.at(6));
        info->m_statBase11 = static_cast<int>(
            static_cast<float>(info->m_statBase11) *
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x28] * row.at(8));
        info->m_statBase13 = static_cast<int>(
            static_cast<float>(info->m_statBase13) *
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x28] * row.at(8));
        info->m_statBase12 = static_cast<int>(
            static_cast<float>(info->m_statBase12) *
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x2d] * row.at(9));
        info->m_statBase14 = static_cast<int>(
            static_cast<float>(info->m_statBase14) *
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x2d] * row.at(9));
        return;
    }

    // ORIG 0x88b85cf：size<level 时 ORIG 内联展开 2 参版逻辑（非调用）。
    info->m_hpBase = static_cast<int>(
        static_cast<float>(info->m_hpBase) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x14]);
    info->m_attr17Base[13] += static_cast<int>(DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x0f]);
    info->m_statBase6 += static_cast<int>(DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x32]);
    for (int i = 0; i <= 0x10; ++i)
    {
        if (getToleranceActiveStatus(static_cast<ENUM_ACTIVEPROPERTY>(i)) != 0x12)
            info->m_attr17Base[i] +=
                static_cast<int>(DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x23]);
    }
    info->m_statBase4 = static_cast<int>(
        static_cast<float>(info->m_statBase4) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x19]);
    if (info->m_statBase3 != 0)
        info->m_statBase3 += static_cast<int>(DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x1e]);
    info->m_statBase11 = static_cast<int>(
        static_cast<float>(info->m_statBase11) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x28]);
    info->m_statBase13 = static_cast<int>(
        static_cast<float>(info->m_statBase13) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x28]);
    info->m_statBase12 = static_cast<int>(
        static_cast<float>(info->m_statBase12) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x2d]);
    info->m_statBase14 = static_cast<int>(
        static_cast<float>(info->m_statBase14) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x2d]);
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
void CRidable::SetRidableObject(RidableScript& script)
{
    if (script.m_0 <= 0)
        return;

    // ORIG 0x082fe6d0：obj（DungeonRidableObject local_c8）与 tmp（RidableObject
    // local_e0）在函数头各构造一次、两分支复用；count < size 分支顺序遍历
    // m_objects（循环条件每次重调 size()），否则拷贝 pool 随机抽取（循环条件用
    // script.m_0）；p167 实证 ORIG operator=(RidableObject) 仅 1 次（IF 分支），
    // ELSE 分支走基类隐式整体赋值（6 dword）。
    DungeonRidableObject obj;
    RidableObject tmp;
    // ORIG 槽图：循环1 计数器 -0xc（浅）、循环2 计数器 -0x10（深）⇒ 头部声明序
    // 实测 4.4 头部标量「后声明浅」：j 先声明确保 i 落浅槽 -0xc（P309 反向）；
    // ELSE 分支的 `j = 0;` 在 pool 构造之前发射（ORIG 0x082fe81c `movl $0x0,
    // -0x10(%ebp)` 先于 C1ERKS_ 0x082fe836）。
    int j;
    int i;

    m_flag18 = script.m_4;                     // ORIG 0x082fe707 movzbl 0x4(%eax),%edx
    m_count1c = script.m_0;

    // ORIG 0x082fe72f：`cmp %eax,%ebx; setge %al; test %al,%al; je <ELSE(0x14c)>`
    // ⇒ fall-through（m_0 >= size）走「顺序遍历 m_objects（上界 size）」，ELSE 走
    // 「拷贝 pool 随机抽取（上界 m_0）」。旧写 `<` 把两分支对象来源互换（真语义 bug）。
    if (script.m_0 >= static_cast<int>(script.m_objects.size()))
    {
        for (i = 0; i < static_cast<int>(script.m_objects.size()); ++i)
        {
            obj = script.m_objects[i];
            obj.m_state = 4;
            m_objects.insert(
                std::pair<int, DungeonRidableObject>(obj.m_field0c, obj));
        }
    }
    else
    {
        j = 0;
        std::vector<RidableObject> pool(script.m_objects);
        for (; j < script.m_0; ++j)
        {
            std::random_shuffle(pool.begin(), pool.end());
            tmp = pool.back();
            pool.pop_back();
            // ORIG 0x082fe8d3..0x082fe915 拷贝序（tmp@-0xdc → obj@-0xc4）：
            // +0xc,+0x10,+0x00,+0x04,+0x08,+0x14（4.1 对 24B 结构赋值的分块展开，
            // 直接写 `*(RidableObject*)&obj = tmp;` 4.4 恒按声明序 +0,+4,+8,+0xc,
            // +0x10,+0x14 发射 ⇒ 逐字段按 ORIG 序书写复现）。
            obj.m_field10 = tmp.m_field10;
            obj.m_field14 = tmp.m_field14;
            obj.m_x = tmp.m_x;
            obj.m_y = tmp.m_y;
            obj.m_field0c = tmp.m_field0c;
            obj.m_field18 = tmp.m_field18;
            obj.m_state = 4;
            m_objects.insert(
                std::pair<int, DungeonRidableObject>(obj.m_field0c, obj));
        }
    }
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
void upgradeAbilityByParty(int job, ActiveStaticInfo* info, int level)
{
    if (info == 0)
        return;
    if (job < 0)
        job = 0;
    else if (job > 3)
        job = 3;
    for (std::vector<DungeonPartyBalance>::size_type i = 0;
         i < DUNGEON_PARTY_BALANCE_TABLE.size(); ++i)
    {
        const DungeonPartyBalance& bal = DUNGEON_PARTY_BALANCE_TABLE[i];
        if (static_cast<float>(level) < bal.m_a)
            continue;
        if (static_cast<float>(level) != bal.m_b)
            continue;
        info->m_hpBase = static_cast<int>(static_cast<float>(info->m_hpBase) *
                                          bal.m_grid[4][job]);
        info->m_attr17Base[13] += static_cast<int>(bal.m_grid[3][job]);
        info->m_statBase6 += static_cast<int>(bal.m_grid[10][job]);
        for (int k = 0; k <= 0x10; ++k)
        {
            if (getToleranceActiveStatus(static_cast<ENUM_ACTIVEPROPERTY>(k)) != 0x12)
                info->m_attr17Base[k] +=
                    static_cast<int>(bal.m_grid[7][job]);
        }
        info->m_statBase4 = static_cast<int>(static_cast<float>(info->m_statBase4) *
                                             bal.m_grid[5][job]);
        if (info->m_statBase3 != 0)
            info->m_statBase3 += static_cast<int>(bal.m_grid[6][job]);
        info->m_statBase11 = static_cast<int>(static_cast<float>(info->m_statBase11) *
                                              bal.m_grid[8][job]);
        info->m_statBase13 = static_cast<int>(static_cast<float>(info->m_statBase13) *
                                              bal.m_grid[8][job]);
        info->m_statBase12 = static_cast<int>(static_cast<float>(info->m_statBase12) *
                                              bal.m_grid[9][job]);
        info->m_statBase14 = static_cast<int>(static_cast<float>(info->m_statBase14) *
                                              bal.m_grid[9][job]);
        return;
    }
}
