// df_game_r BlueMarbleInfoScript（G2-4 大富翁配置脚本）还原（2026-08-17）。
// 逐函数对照 docs/class_func_reports/BlueMarbleInfoScript.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径）。
// 脚本解析函数（loadRDARScriptFile/ScanType/ScanInt/ScanFloat）经 asm-label
// extern 调用真实符号。
#include <string>

#include "BlueMarbleInfoScript.h"

// ============================================================================
// 脚本解析方法（asm-label extern）
// ============================================================================
extern "C" bool sub_loadRDARScriptFile(const char* dir, const char* path)
    asm("_Z18loadRDARScriptFilePKcS0_");
extern "C" bool sub_ScanType(std::string& line, bool value)
    asm("_Z8ScanTypeRSsb");
extern "C" bool sub_ScanInt(int* out) asm("_Z7ScanIntPi");
extern "C" int sub_ScanIntBool(bool* out) asm("_Z7ScanIntPb");
extern "C" float sub_ScanFloat(float* out) asm("_Z9ScanFloatPf");

// ============================================================================
// BlueMarbleInfoScript 实现
// ============================================================================

BlueMarbleInfoScript::BlueMarbleInfoScript()
{
    clear();
}

BlueMarbleInfoScript::~BlueMarbleInfoScript()
{
}

void BlueMarbleInfoScript::clear()
{
    m_playerMax = 0;
    m_enterLevel = 0;
    m_enterMaxLevel = 0;
    m_enterCount = 0;
    m_dieCount = 0;
    m_diceMin = 0;
    m_diceMax = 0;
    m_itemWeightTotal = 0;
    m_dungeonDifficulty = 0;
    m_dungeonDifficultyList.clear();
    m_superArmorTime = 0;
    m_buffMaxValue = 0;
    m_goldBase = 0.0f;
    m_goldMulti = 0.0f;
    m_randomDungeonList.clear();
    m_bossDungeonList.clear();
    m_randomItemList.clear();
    m_tilePosList.clear();
    m_equipmentUpgradeMax = 1000;
}

std::vector<int> BlueMarbleInfoScript::getRandomDungeonIndex(int level)
{
    std::vector<int> result;
    unsigned int i = 0;
    while (i < m_randomDungeonList.size())
    {
        BlueMarbleRandomDungeonInfo& elem = m_randomDungeonList[i];
        if (elem.m_minLevel <= level && level <= elem.m_maxLevel)
        {
            result.push_back(elem.m_dungeonIndex);
        }
        ++i;
    }
    return result;
}

std::vector<int> BlueMarbleInfoScript::getBossDungeonIndex(int level)
{
    std::vector<int> result;
    unsigned int i = 0;
    while (i < m_bossDungeonList.size())
    {
        BlueMarbleRandomDungeonInfo& elem = m_bossDungeonList[i];
        if (elem.m_minLevel <= level && level <= elem.m_maxLevel)
        {
            result.push_back(elem.m_dungeonIndex);
        }
        ++i;
    }
    return result;
}

std::vector<int> BlueMarbleInfoScript::getUniqueDungeonIndex(int level)
{
    std::vector<int> result;
    unsigned int i = 0;
    while (i < m_uniqueDungeonList.size())
    {
        BlueMarbleRandomDungeonInfo& elem = m_uniqueDungeonList[i];
        if (elem.m_minLevel <= level && level <= elem.m_maxLevel)
        {
            result.push_back(elem.m_dungeonIndex);
        }
        ++i;
    }
    return result;
}

int BlueMarbleInfoScript::getDungeonDifficulty(int dungeon)
{
    unsigned int i = 0;
    while (i < m_dungeonDifficultyList.size())
    {
        if (m_dungeonDifficultyList[i].m_dungeon == dungeon)
        {
            return m_dungeonDifficultyList[i].m_difficulty;
        }
        ++i;
    }
    return m_dungeonDifficulty;
}

int BlueMarbleInfoScript::importScript(const char* dir, const char* path)
{
    if (!sub_loadRDARScriptFile(dir, path))
    {
        return 0;
    }
    // 局部声明顺序与 ORIG 栈布局一致（d4/-0x20、d3/-0x2c、ri/-0x34、
    // rw/-0x3c、tp/-0x44、d2/-0x4c、d1/-0x58、line/-0x10、flag/-0x14）
    BlueMarbleRandomDungeonInfo d4;
    BlueMarbleRandomDungeonInfo d3;
    BlueMarbleRandomItemInfo ri;
    BlueMarbleRewardInfo rw;
    BlueMarbleTilePos tp;
    BlueMarbleDungeonDifficulty d2;
    BlueMarbleRandomDungeonInfo d1;
    std::string line;
    int loopFlag = 0;
    (void)loopFlag;

    while (sub_ScanType(line, true))
    {
        if (line == "[dungeon list]")
        {
            m_randomDungeonList.clear();
            while (sub_ScanInt(&d4.m_minLevel) && sub_ScanInt(&d4.m_maxLevel) &&
                   sub_ScanInt(&d4.m_dungeonIndex))
            {
                m_randomDungeonList.push_back(d4);
            }
        }
        else if (line == "[boss dungeon list]")
        {
            m_bossDungeonList.clear();
            while (sub_ScanInt(&d3.m_minLevel) && sub_ScanInt(&d3.m_maxLevel) &&
                   sub_ScanInt(&d3.m_dungeonIndex))
            {
                m_bossDungeonList.push_back(d3);
            }
        }
        else if (line == "[item list]")
        {
            m_randomItemList.clear();
            while (sub_ScanInt(&ri.m_itemIndex) && sub_ScanInt(&ri.m_weight))
            {
                m_itemWeightTotal += ri.m_weight;
                m_randomItemList.push_back(ri);
            }
        }
        else if (line == "[player max]")
        {
            m_playerMax = sub_ScanIntBool((bool*)0);
        }
        else if (line == "[enter level]")
        {
            m_enterLevel = sub_ScanIntBool((bool*)0);
        }
        else if (line == "[enter max level]")
        {
            m_enterMaxLevel = sub_ScanIntBool((bool*)0);
        }
        else if (line == "[enter count]")
        {
            m_enterCount = sub_ScanIntBool((bool*)0);
        }
        else if (line == "[die count]")
        {
            m_dieCount = sub_ScanIntBool((bool*)0);
        }
        else if (line == "[dice value]")
        {
            sub_ScanInt(&m_diceMin);
            sub_ScanInt(&m_diceMax);
        }
        else if (line == "[reward item]")
        {
            m_rewardList.clear();
            while (sub_ScanInt(&rw.m_a) && sub_ScanInt(&rw.m_b))
            {
                m_rewardList.push_back(rw);
            }
        }
        else if (line == "[tile pos]")
        {
            m_tilePosList.clear();
            while (sub_ScanInt(&tp.m_a) && sub_ScanInt(&tp.m_b))
            {
                m_tilePosList.push_back(tp);
            }
        }
        else if (line == "[dungeon difficulty]")
        {
            m_dungeonDifficulty = sub_ScanIntBool((bool*)0);
        }
        else if (line == "[dungeon difficulty list]")
        {
            m_dungeonDifficultyList.clear();
            while (sub_ScanInt(&d2.m_dungeon) && sub_ScanInt(&d2.m_difficulty))
            {
                m_dungeonDifficultyList.push_back(d2);
            }
        }
        else if (line == "[super armor time]")
        {
            m_superArmorTime = sub_ScanIntBool((bool*)0);
        }
        else if (line == "[unique dungeon list]")
        {
            m_uniqueDungeonList.clear();
            while (sub_ScanInt(&d1.m_minLevel) && sub_ScanInt(&d1.m_maxLevel) &&
                   sub_ScanInt(&d1.m_dungeonIndex))
            {
                m_uniqueDungeonList.push_back(d1);
            }
        }
        else if (line == "[buff max value]")
        {
            m_buffMaxValue = sub_ScanIntBool((bool*)0);
        }
        else if (line == "[gold base]")
        {
            m_goldBase = sub_ScanFloat((float*)0);
        }
        else if (line == "[gold multi]")
        {
            m_goldMulti = sub_ScanFloat((float*)0);
        }
        else if (line == "[equipment upgrade max]")
        {
            m_equipmentUpgradeMax = sub_ScanIntBool((bool*)0);
        }
    }
    return 1;
}
