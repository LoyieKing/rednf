// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CHackLog_OverDamage/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CHackLog_OverDamage.h"
#include "CHackAnalyzer.h"   // WongWork::ENUM_HACKTYPE 权威枚举
#include "CUser.h"
#include "CParty.h"     // m_dungeon / m_battleField 视图
#include "CDungeon.h"   // get_min_level/get_index
#include "CBattle_Field.h"   // get_dungeon_diff

const unsigned int _S_KILL_MONSTER_OVER_DAMAGE_CONST[7] = {
    600, 1800, 3600, 25000, 55000, 70064, 712640
};   // {258,708,e10,61a8,d6d8,1170,ae60}

// ORIG _ZL41_S_KILL_MONSTER_OVER_DAMAGE_DUNGEON_RATIO（objdump 实测）：
// {2.0,1.8,1.7,1.6,1.0}——第 3/4 项原抄错为 0.075/0.025，已按 ORIG 修正。
const double _S_KILL_MONSTER_OVER_DAMAGE_DUNGEON_RATIO[5] = {
    2.0, 1.8, 1.7, 1.6, 1.0
};

const unsigned int _S_KILL_MONSTER_OVER_DAMAGE_HACK_TYPE[7] = {
    0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b
};

const int LEVEL_SECTION_TABLE[89] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5
};

#include "CHackLog_OverDamage/GetDungeonLimitDamageAndHackType.inc"
    CUser* user, unsigned int& limit, WongWork::ENUM_HACKTYPE& type)
{
    // [b28d-46 判词·N/N82] ORIG 0x08286790：149 vs 147；帧 0x54 vs 0x58 与
    // 局部预清槽（movb $0,-0x31 等）平移（N82）⇒ 封顶。
    // ORIG 0x0828679e-0x082868a0：4 条件单出口 bool 汇聚（3 guard 各自
    // `je <mov$0,eax>`，通过则 `mov$1,eax`，尾 `test%al; je ret`）；
    // 每使用点重 call GetParty()+mov 0xcac（P160 去缓存）。
    bool bOk = false;
    int level = 0;
    if (user != 0)
    {
        if (user->GetParty() != 0)
        {
            if (static_cast<CParty*>(user->GetParty())->m_battleField.m_pDungeon != 0)
            {
                level = user->get_charac_level();
                if (level >= 1)
                    bOk = true;
            }
        }
    }
    if (!bOk)
        return;

    int sectionIdx = 6;                       // ORIG local_30 初值
    double ratio = 1.0;                       // ORIG local_2c 初值
    int lvDelta = level - static_cast<CParty*>(user->GetParty())
                           ->m_battleField.m_pDungeon->get_min_level();   // ORIG local_20
    if (lvDelta < 0)
        lvDelta = 0;
    double divisor = 1.0;                     // ORIG local_1c 初值

    int dungeonIndex = static_cast<CParty*>(user->GetParty())
                               ->m_battleField.m_pDungeon->get_index();
    // ORIG 0x0828684d-0x082868a2：switch(dungeonIndex) 二分树
    // （cmp 0x2af8 双试 je/jg + cmp 0x64/0x2af9/0x2afe），**所有 case 汇聚
    // 同一 LAB_0828692e 尾计算块**（旧形把 limit/type 算块复制了两份）。
    switch (dungeonIndex)
    {
    case 100:
        ratio = _S_KILL_MONSTER_OVER_DAMAGE_DUNGEON_RATIO[
            reinterpret_cast<CBattle_Field*>(
                &static_cast<CParty*>(user->GetParty())->m_battleField)->get_dungeon_diff()];
        break;                              // → 尾（ORIG +0xeb..+0x0d jmp 19e）
    case 11000:
    case 0x2af9:
    case 0x2afe:
        break;                              // ratio/divisor 保持初值直达尾
    default:
        divisor = static_cast<double>(lvDelta / 20);
        if (divisor < 1.0)
            divisor = 1.0;
        int lvKey = level - 1;
        if (lvKey < 0)                      // movzbl/not/and 形态 = max(0)
            lvKey = 0;
        sectionIdx = LEVEL_SECTION_TABLE[lvKey];
        ratio = _S_KILL_MONSTER_OVER_DAMAGE_DUNGEON_RATIO[
            reinterpret_cast<CBattle_Field*>(
                &static_cast<CParty*>(user->GetParty())->m_battleField)->get_dungeon_diff()];
        break;
    }

    limit = static_cast<unsigned int>(
        static_cast<long long>(
            static_cast<double>(_S_KILL_MONSTER_OVER_DAMAGE_CONST[sectionIdx]) *
            ratio * divisor));
    type = static_cast<WongWork::ENUM_HACKTYPE>(
        _S_KILL_MONSTER_OVER_DAMAGE_HACK_TYPE[sectionIdx]);
}

#include "CHackLog_OverDamage/GetDeathTowerLimitDamage.inc"
{
    return _S_KILL_MONSTER_OVER_DAMAGE_CONST[6];
}

#include "CHackLog_OverDamage/GetDeathTowerOverDamageHackType.inc"
{
    return static_cast<WongWork::ENUM_HACKTYPE>(0x6b);
}

#include "CHackLog_OverDamage/GetDungeonLimitMaxDamage.inc"
{
    return _S_KILL_MONSTER_OVER_DAMAGE_CONST[6];
}

#include "CHackLog_OverDamage/GetHackTypeOverLimitMaxDamage.inc"
{
    return static_cast<WongWork::ENUM_HACKTYPE>(0x6b);
}

#include "CHackLog_OverDamage/GetBossTowerLimitDamage.inc"
{
    return _S_KILL_MONSTER_OVER_DAMAGE_CONST[5];
}

#include "CHackLog_OverDamage/GetBossTowerOverDamageHackType.inc"
{
    return static_cast<WongWork::ENUM_HACKTYPE>(0x85);
}

