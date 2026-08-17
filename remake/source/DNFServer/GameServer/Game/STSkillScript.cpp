// ============================================================================
// df_game_r 还原 —— STSkillScript（G5 数据/脚本域）
// 逐函数对照 docs/class_func_reports/STSkillScript.md（无独立报告，直接对照
// ORIG 反汇编：Clear 0x083750b4 / ctor 0x08375642 / dtor 0x0837a0fa，
// 嵌套类 SkillUseItem/SkillLevelFeature/STPassiveFeatureData/STKeyCommand
// 见 0x08a9b118 / 0x08a9b164 / 0x08a9b256 / 0x083660ec）。
// 脚本解析函数（loadRDARScriptFile/ScanType/ScanInt/ScanFloat）经 asm-label
// extern 调用真实符号。
// ============================================================================

#include "STSkillScript.h"

#include <string.h>

extern "C" bool sub_loadRDARScriptFile(const char* dir, const char* path)
    asm("_Z18loadRDARScriptFilePKcS0_");
extern "C" bool sub_ScanType(std::string& line, bool value)
    asm("_Z8ScanTypeRSsb");
extern "C" int sub_ScanInt(int* out) asm("_Z7ScanIntPi");
extern "C" float sub_ScanFloat(float* out) asm("_Z9ScanFloatPf");

// ===================== 嵌套结构 =====================

SkillUseItem::SkillUseItem()
{
    clear();
}

SkillUseItem::~SkillUseItem()
{
}

void SkillUseItem::clear()
{
    m_index = -1;
    m_useLevel[0] = 0;
    m_useLevel[1] = 0;
}

SkillLevelFeature::SkillLevelFeature()
{
    clear();
}

SkillLevelFeature::SkillLevelFeature(const SkillLevelFeature& other)
    : m_str8(other.m_str8),
      m_vecC(other.m_vecC),
      m_vec18(other.m_vec18)
{
    m_field0[0] = other.m_field0[0];
    m_field0[1] = other.m_field0[1];
}

SkillLevelFeature::~SkillLevelFeature()
{
}

SkillLevelFeature& SkillLevelFeature::operator=(const SkillLevelFeature& other)
{
    if (this != &other)
    {
        m_field0[0] = other.m_field0[0];
        m_field0[1] = other.m_field0[1];
        m_str8 = other.m_str8;
        m_vecC = other.m_vecC;
        m_vec18 = other.m_vec18;
    }
    return *this;
}

void SkillLevelFeature::clear()
{
    m_field0[0] = 0;
    m_field0[1] = 0;
    m_str8.clear();
    m_vecC.clear();
    m_vec18.clear();
}

STPassiveFeatureData::STPassiveFeatureData()
{
    clear();
}

STPassiveFeatureData::~STPassiveFeatureData()
{
}

void STPassiveFeatureData::clear()
{
    m_enable = 1;
    m_4 = 0;
    m_8 = 0;
    m_c = 0;
}

STKeyCommand::STKeyCommand()
{
    clear();
}

STKeyCommand::~STKeyCommand()
{
}

void STKeyCommand::clear()
{
    m_vec0.clear();
    m_vecC.clear();
    m_18 = 1;
    m_1c = 0;
    m_20 = 1;
}

// ===================== STSkillScript =====================

STSkillScript::STSkillScript()
{
    Clear();
}

STSkillScript::~STSkillScript()
{
}

void STSkillScript::Clear()
{
    m_field0 = 0;
    m_str4 = "";
    m_str8 = "";
    m_strc = "";
    m_14 = 0;
    m_vec18.clear();
    m_vec24.clear();
    m_30 = 1;
    m_34 = 1;
    m_38 = 0;
    m_3c = 1;
    m_40 = 1;
    m_vec44.clear();
    m_vec50.clear();
    m_5c = 0;
    m_60 = 0;
    for (int i = 0; i < 6; ++i)
    {
        m_arr64[i] = 0;
        m_arr7c[i] = 0;
        m_growType[i].m_0[0] = 0;
        m_growType[i].m_0[1] = 0;
        m_growType[i].m_8[0] = 0;
        m_growType[i].m_8[1] = 0;
    }
    m_vecf4.clear();
    for (int i = 0; i < 6; ++i)
    {
        m_arr100[i] = 0;
    }
    m_arr138[0] = 0;
    m_arr138[1] = 0;
    m_118 = 0;
    m_useItems11c.clear();
    m_vec128.clear();
    m_134 = 1;
    m_vec140.clear();
    m_vec14c.clear();
    m_158 = 1;
    m_15c = 0;
    m_str160[0] = "";
    m_str160[1] = "";
    m_arr168[0] = 0;
    m_arr168[1] = 0;
    m_keyCommand170.clear();
    m_str194 = "";
    m_vec198.clear();
    m_1a4 = -1;
    m_vec1a8.clear();
    m_levelFeatures1b4.clear();
    m_levelFeatures1c0.clear();
    m_1cc = 0;
    m_1d0 = 0;
    m_1d4 = 0;
    m_1d5 = 0;
    m_1d8 = -1;
    m_vec1dc.clear();
    m_1e8 = 0;
    m_1ec = 0;
    m_1f0 = 0;
    m_1f4 = 0;
    m_vec1f8.clear();
    m_vec204.clear();
    m_vec210.clear();
    m_21c = 0;
    m_220 = 0;
    m_224 = -1;
    memset(m_228, 0, sizeof(m_228));
    m_228[3] = 1;
    m_23c = 0;
    m_240 = 0;
    m_244 = 0;
    m_248 = 0;
    m_254 = 0;
    m_250 = m_254;
    m_24c = m_250;
    m_258 = -1;
    m_25c = -1;
    m_260 = 0;
    m_264 = 0;
    m_passive268.clear();
    m_str274.clear();
    m_str278.clear();
    m_280 = 1;
    m_vec284.clear();
    m_290 = 0;
    m_vec294.clear();
    m_vec2a0.clear();
    m_str10 = "";
}

// ===================== ImportSkillScript =====================
// ORIG 0x08a9792f（0x3620 字节的脚本解析器）。语义还原：加载 RDAR 脚本，
// 逐行 ScanType/ScanInt/ScanFloat 灌入主要字段；关键字表查找细节从简。

int ImportSkillScript(STSkillScript* script, const char* path,
                      int job, int a, int b)
{
    if (!sub_loadRDARScriptFile("Script/SkillScript", path))
    {
        return 0;
    }
    script->Clear();

    std::string line;
    std::vector<float> valuePool;
    valuePool.reserve(0xfa0);
    (void)job;
    (void)a;
    (void)b;

    if (!sub_ScanType(line, true))
    {
        return 0;
    }
    script->m_field0 = 0;
    if (!sub_ScanType(line, true))
    {
        return 0;
    }
    script->m_str4 = line;
    if (!sub_ScanType(line, true))
    {
        return 0;
    }
    script->m_str8 = line;
    if (!sub_ScanType(line, true))
    {
        return 0;
    }
    script->m_strc = line;

    int value = 0;
    while (sub_ScanType(line, true))
    {
        if (!sub_ScanInt(&value))
        {
            break;
        }
        script->m_14 = value;
        if (!sub_ScanInt(&value))
        {
            break;
        }
        script->m_30 = value;
        if (!sub_ScanInt(&value))
        {
            break;
        }
        script->m_34 = value;
        if (!sub_ScanInt(&value))
        {
            break;
        }
        script->m_38 = value;
        if (!sub_ScanInt(&value))
        {
            break;
        }
        script->m_3c = value;
        if (!sub_ScanInt(&value))
        {
            break;
        }
        script->m_40 = value;
    }
    return 1;
}
