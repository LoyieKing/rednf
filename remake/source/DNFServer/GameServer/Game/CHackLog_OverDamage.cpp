// ============================================================================
// df_game_r 还原 —— CHackLog_OverDamage（过度伤害 hack 检测限值表）
//
// 报告：docs/class_func_reports/CHackLog_OverDamage/*.md（7 份全文，汇编+C）。
// 静态表数据由 ORIG ELF 实测（objdump -s .rodata 0x08c10c68..0x08c10e64，
// /home/wangyilei/下载/df_game_r），与 ORIG 本地符号 _ZL* 一一对应：
//   _ZL33_S_KILL_MONSTER_OVER_DAMAGE_CONST        7 x uint @0x08c10c68
//   _ZL41_S_KILL_MONSTER_OVER_DAMAGE_DUNGEON_RATIO 5 x double @0x08c10ca0
//   _ZL37_S_KILL_MONSTER_OVER_DAMAGE_HACK_TYPE    7 x int @0x08c10cc8
//   _ZL19LEVEL_SECTION_TABLE                     89 x int @0x08c10d00
//
// GetDungeonLimitDamageAndHackType 的 fp 舍入为 RC=truncate（fldcw 0xc in ah
// 后 fistpl），以 C 风格 (long long)(double) 强转等价复现。
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

unsigned int CHackLog_OverDamage::GetDeathTowerLimitDamage()
{
    return _S_KILL_MONSTER_OVER_DAMAGE_CONST[6];
}

WongWork::ENUM_HACKTYPE CHackLog_OverDamage::GetDeathTowerOverDamageHackType()
{
    return static_cast<WongWork::ENUM_HACKTYPE>(0x6b);
}

unsigned int CHackLog_OverDamage::GetDungeonLimitMaxDamage()
{
    return _S_KILL_MONSTER_OVER_DAMAGE_CONST[6];
}

WongWork::ENUM_HACKTYPE CHackLog_OverDamage::GetHackTypeOverLimitMaxDamage()
{
    return static_cast<WongWork::ENUM_HACKTYPE>(0x6b);
}

unsigned int CHackLog_OverDamage::GetBossTowerLimitDamage()
{
    return _S_KILL_MONSTER_OVER_DAMAGE_CONST[5];
}

WongWork::ENUM_HACKTYPE CHackLog_OverDamage::GetBossTowerOverDamageHackType()
{
    return static_cast<WongWork::ENUM_HACKTYPE>(0x85);
}

void CHackLog_OverDamage::GetDungeonLimitDamageAndHackType(
    CUser* user, unsigned int& limit, WongWork::ENUM_HACKTYPE& type)
{
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
