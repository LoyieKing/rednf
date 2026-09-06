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
    if (user == 0)
        return;
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0 || party->m_battleField.m_pDungeon == 0)
        return;
    int level = user->get_charac_level();
    if (level < 1)
        return;

    int sectionIdx = 6;                       // ORIG local_30 初值
    double ratio = 1.0;                       // ORIG local_2c 初值
    int lvDelta = level -
                  party->m_battleField.m_pDungeon->get_min_level();   // ORIG local_20
    if (lvDelta < 0)
        lvDelta = 0;
    double divisor = 1.0;                     // ORIG local_1c 初值

    int dungeonIndex = party->m_battleField.m_pDungeon->get_index();
    if (dungeonIndex != 11000)                // ORIG 0x2af8
    {
        if (dungeonIndex == 100 || dungeonIndex == 0x2af9 ||
            dungeonIndex == 0x2afe)
        {
            // ratio 保持初始 1.0 直达 LAB_0828692e
            // （dungeonIndex==100 汇编落于中段分支会改写 ratio——见下）
            if (dungeonIndex == 100)
            {
                // ratio 值未进入下方默认分支前的改写路径（ORIG +0x0eb..+0x10d
                // 与默认分支共用同一 DUNGEON_RATIO 表查询形态）：
                CBattle_Field& field =
                    *reinterpret_cast<CBattle_Field*>(&party->m_battleField);
                ratio = _S_KILL_MONSTER_OVER_DAMAGE_DUNGEON_RATIO[field.get_dungeon_diff()];
            }
            limit = static_cast<unsigned int>(
                static_cast<long long>(
                    static_cast<double>(_S_KILL_MONSTER_OVER_DAMAGE_CONST[sectionIdx]) *
                    ratio * divisor));
            type = static_cast<WongWork::ENUM_HACKTYPE>(
                _S_KILL_MONSTER_OVER_DAMAGE_HACK_TYPE[sectionIdx]);
            return;
        }
        divisor = static_cast<double>(lvDelta / 20);
        if (divisor < 1.0)
            divisor = 1.0;
        int lvKey = level - 1;
        if (lvKey < 0)                        // movzbl/not/and 形态 = max(0)
            lvKey = 0;
        sectionIdx = LEVEL_SECTION_TABLE[lvKey];
        CBattle_Field& field =
            *reinterpret_cast<CBattle_Field*>(&party->m_battleField);
        ratio = _S_KILL_MONSTER_OVER_DAMAGE_DUNGEON_RATIO[field.get_dungeon_diff()];
    }

    limit = static_cast<unsigned int>(
        static_cast<long long>(
            static_cast<double>(_S_KILL_MONSTER_OVER_DAMAGE_CONST[sectionIdx]) *
            ratio * divisor));
    type = static_cast<WongWork::ENUM_HACKTYPE>(
        _S_KILL_MONSTER_OVER_DAMAGE_HACK_TYPE[sectionIdx]);
}
