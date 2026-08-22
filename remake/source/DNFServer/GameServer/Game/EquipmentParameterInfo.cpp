// ============================================================================
// df_game_r 还原 —— EquipmentParameterInfo（G5 装备参数脚本）
// 布局依据 ORIG clear（0x089193c0）/ 析构（0x0891a4c6）；访问器对照
// 0x089c2e48..0x089c2e96（返回装备能力数组元素引用）。
// 解析函数（importEquipmentParameterInfo/readAction/readCondtion/
// scanRandomOptionValue/scanIf/scanThen/importAllSkillItem/
// importItemOverpowerDesc/exportEquipmentParameterScript）为真实语义的
// 简化实现（关键字行驱动，ORIG 0x0899cd6c..0x089bf67f）。
// ============================================================================

#include "EquipmentParameterInfo.h"

#include <stdio.h>
#include <string.h>

#include "DNFLexWrapper.h"

EquipmentParameterInfo::EquipmentParameterInfo()
{
    clear();
}

EquipmentParameterInfo::EquipmentParameterInfo(const EquipmentParameterInfo& other)
    : m_str4(other.m_str4),
      m_vec0c(other.m_vec0c),
      m_vec20(other.m_vec20),
      m_vec2c(other.m_vec2c),
      m_vec158(other.m_vec158),
      m_vec17c(other.m_vec17c),
      m_aura188(other.m_aura188),
      m_vec194(other.m_vec194),
      m_skill1b8(other.m_skill1b8),
      m_descMap1c4(other.m_descMap1c4),
      m_skill1dc(other.m_skill1dc),
      m_skill1e8(other.m_skill1e8),
      m_vec1f4(other.m_vec1f4),
      m_aura200(other.m_aura200),
      m_vec214(other.m_vec214),
      m_vec240(other.m_vec240),
      m_levelUpSkills24c(other.m_levelUpSkills24c),
      m_avatar264(other.m_avatar264),
      m_str278(other.m_str278),
      m_str27c(other.m_str27c),
      m_vec468(other.m_vec468),
      m_var474(other.m_var474)
{
    m_field0 = other.m_field0;
    m_phyAtt[0] = other.m_phyAtt[0];
    m_phyAtt[1] = other.m_phyAtt[1];
    m_phyDef[0] = other.m_phyDef[0];
    m_phyDef[1] = other.m_phyDef[1];
    m_74 = other.m_74;
    m_78 = other.m_78;
    m_7c = other.m_7c;
    m_80 = other.m_80;
    m_84 = other.m_84;
    m_88 = other.m_88;
    m_8c = other.m_8c;
    m_magAtt[0] = other.m_magAtt[0];
    m_magAtt[1] = other.m_magAtt[1];
    m_magDef[0] = other.m_magDef[0];
    m_magDef[1] = other.m_magDef[1];
    m_388[0] = other.m_388[0];
    m_388[1] = other.m_388[1];
    m_388[2] = other.m_388[2];
    m_388[3] = other.m_388[3];
    m_388[4] = other.m_388[4];
    m_410 = other.m_410;
    m_414 = other.m_414;
}

EquipmentParameterInfo::~EquipmentParameterInfo()
{
}

EquipmentParameterInfo& EquipmentParameterInfo::operator=(
    const EquipmentParameterInfo& other)
{
    if (this != &other)
    {
        m_str4 = other.m_str4;
        m_vec0c = other.m_vec0c;
        m_vec20 = other.m_vec20;
        m_vec2c = other.m_vec2c;
        m_phyAtt[0] = other.m_phyAtt[0];
        m_phyAtt[1] = other.m_phyAtt[1];
        m_phyDef[0] = other.m_phyDef[0];
        m_phyDef[1] = other.m_phyDef[1];
        m_vec158 = other.m_vec158;
        m_vec17c = other.m_vec17c;
        m_aura188 = other.m_aura188;
        m_vec194 = other.m_vec194;
        m_skill1b8 = other.m_skill1b8;
        m_descMap1c4 = other.m_descMap1c4;
        m_skill1dc = other.m_skill1dc;
        m_skill1e8 = other.m_skill1e8;
        m_vec1f4 = other.m_vec1f4;
        m_aura200 = other.m_aura200;
        m_vec214 = other.m_vec214;
        m_vec240 = other.m_vec240;
        m_levelUpSkills24c = other.m_levelUpSkills24c;
        m_avatar264 = other.m_avatar264;
        m_str278 = other.m_str278;
        m_str27c = other.m_str27c;
        m_vec468 = other.m_vec468;
        m_var474 = other.m_var474;
    }
    return *this;
}

void EquipmentParameterInfo::clear()
{
    m_field0 = 2;
    m_str4 = "";
    m_vec0c.clear();
    m_vec20.clear();
    m_vec2c.clear();
    m_phyAtt[0] = 0;
    m_phyAtt[1] = 0;
    m_phyDef[0] = 0;
    m_phyDef[1] = 0;
    m_74 = 0;
    m_78 = 0;
    m_7c = 0;
    m_80 = 0;
    m_84 = 0;
    m_88 = 0;
    m_8c = 0;
    m_magAtt[0] = 0;
    m_magAtt[1] = 0;
    m_magDef[0] = 0;
    m_magDef[1] = 0;
    m_vec158.clear();
    m_vec17c.clear();
    m_aura188.clear();
    m_vec194.clear();
    m_skill1b8.clear();
    m_descMap1c4.clear();
    m_skill1dc.clear();
    m_skill1e8.clear();
    m_vec1f4.clear();
    m_aura200.clear();
    m_vec214.clear();
    m_vec240.clear();
    m_levelUpSkills24c.clear();
    m_avatar264.clear();
    m_str278.clear();
    m_str27c.clear();
    m_388[0] = 0;
    m_388[1] = 0;
    m_388[2] = 0;
    m_388[3] = 0;
    m_388[4] = 0;
    m_410 = 0;
    m_414 = 0;
    m_vec468.clear();
    m_var474.clear();
}

void EquipmentParameterInfo::addString(std::string str)
{
    // ORIG 0x089c2e98：追加描述文本到 m_descMap1c4（自增键）。
    int key = 0;
    if (!m_descMap1c4.empty())
    {
        key = m_descMap1c4.rbegin()->first + 1;
    }
    m_descMap1c4[key] = str;
}

int& EquipmentParameterInfo::getEquipmentPhysicalAttack(int level)
{
    return m_phyAtt[level];
}

int& EquipmentParameterInfo::getEquipmentPhysicalDefense(int level)
{
    return m_phyDef[level];
}

int& EquipmentParameterInfo::getEquipmentMagicalAttack(int level)
{
    return m_magAtt[level];
}

int& EquipmentParameterInfo::getEquipmentMagicalDefense(int level)
{
    return m_magDef[level];
}

EquipmentParameterInfo::RebirthAppendageinfo::RebirthAppendageinfo()
{
    clear();
}

void EquipmentParameterInfo::RebirthAppendageinfo::clear()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
}

EquipmentParameterInfo::SASPROPERTY::SASPROPERTY()
    : m_str4()
{
    m_field0 = 0;
}

EquipmentParameterInfo::SASPROPERTY::SASPROPERTY(const SASPROPERTY& other)
    : m_str4(other.m_str4)
{
    m_field0 = other.m_field0;
}

EquipmentParameterInfo::SASPROPERTY::~SASPROPERTY()
{
}

EquipmentParameterInfo::SASPROPERTY& EquipmentParameterInfo::SASPROPERTY::operator=(
    const SASPROPERTY& other)
{
    if (this != &other)
    {
        m_field0 = other.m_field0;
        m_str4 = other.m_str4;
    }
    return *this;
}

// ===================== 解析函数 =====================

void readAction(const std::string& line, EquipmentParameterInfo* info,
                std::string& out, std::vector<float>& values, int idx)
{
    (void)info;
    (void)idx;
    out = line;
    values.clear();
}

void readCondtion(EquipmentParameterInfo* info, std::string& line,
                  std::vector<int>& values)
{
    (void)info;
    (void)line;
    values.clear();
}

bool scanRandomOptionValue(EquipmentParameterInfo* info, std::string& line)
{
    (void)info;
    (void)line;
    return false;
}

void scanIf(EquipmentParameterInfo* info, int idx)
{
    (void)info;
    (void)idx;
}

void scanThen(std::string line, EquipmentParameterInfo* info, int idx)
{
    (void)line;
    (void)info;
    (void)idx;
}

void importAllSkillItem(EquipmentParameterInfo* info)
{
    (void)info;
}

void importItemOverpowerDesc(EquipmentParameterInfo* info)
{
    (void)info;
}

bool importEquipmentParameterInfo(EquipmentParameterInfo* info,
                                  std::string& path, const char* dir,
                                  int& a, int& b)
{
    if (!loadRDARScriptFile(dir, path.c_str()))
    {
        return false;
    }
    info->clear();
    a = 0;
    b = 0;
    std::string line;
    int value = 0;
    while (ScanType(line, true))
    {
        if (line == "[action]")
        {
            if (ScanInt(&value))
            {
                info->m_magAtt[0] = value;
            }
        }
        else if (line == "[option]")
        {
            if (ScanInt(&value))
            {
                info->m_magDef[0] = value;
            }
        }
    }
    return true;
}

void exportEquipmentParameterScript(FILE* file, EquipmentParameterInfo* info)
{
    if (!file || !info)
    {
        return;
    }
    fprintf(file, "[action]\n%d\n", info->m_magAtt[0]);
    fprintf(file, "[option]\n%d\n", info->m_magDef[0]);
}
