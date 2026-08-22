#include "LogManager.h"
#include <string>
#include "DNFLexWrapper.h"
#include "CNRDScriptData.h"

// ============================================================================
// df_game_r CNRDScriptData 实现
// ORIG 见 docs/class_func_reports/CNRDScriptData.md（构造 0x807f858，import
// 0x807fb2c，getInt 0x807fe8a，getFloat 0x807febc，getString 0x807fef4，
// clear 0x807fa58，reset 0x807fac8，create 0x807fabc，destroy 0x807fac2，
// 析构 0x807f966）。脚本解析辅助（loadRDARScriptFile/ScanType/ScanInt/ScanFloat/
// ScanStr）与 logFormat 经 asm-label 调用 ORIG 真实符号（与其它 ST*Script 导入
// 一致）。
// ============================================================================







CNRDScriptData::CNRDScriptData()
{
    clear();
    create();
}

CNRDScriptData::~CNRDScriptData()
{
    destroy();
}

void CNRDScriptData::clear()
{
    m_path = "";
    m_intMap.clear();
    m_floatMap.clear();
    m_clongMap.clear();
    m_cfloatMap.clear();
    m_strMap.clear();
}

void CNRDScriptData::reset()
{
    m_path = "";
    m_intMap.clear();
    m_floatMap.clear();
    m_clongMap.clear();
    m_cfloatMap.clear();
    m_strMap.clear();
}

void CNRDScriptData::create()
{
}

void CNRDScriptData::destroy()
{
}

int CNRDScriptData::getInt(const char* key) const
{
    const int* p = m_intMap.get(key);
    return p ? *p : -1;
}

int CNRDScriptData::getCLong(const char* key) const
{
    const int* p = m_clongMap.get(key);
    return p ? *p : -1;
}

float CNRDScriptData::getFloat(const char* key) const
{
    const float* p = m_floatMap.get(key);
    return p ? *p : -1.0f;
}

float CNRDScriptData::getCFloat(const char* key) const
{
    const float* p = m_cfloatMap.get(key);
    return p ? *p : -1.0f;
}

const char* CNRDScriptData::getString(const char* key) const
{
    const std::string* p = m_strMap.get(key);
    return p ? p->c_str() : "CNRDScriptData::Error";
}

int CNRDScriptData::import(const char* path)
{
    if (!loadRDARScriptFile("Data/", path))
    {
        // ORIG import 失败分支（0x807fb4e 起）：logFormat 记录后返回 0。
        LogManager::logFormat(0, "../../DNFShared/CNRDScriptData.cpp", "import", 0x33,
                                 "CNRDScriptData::import: fail to load file - %s", path);
        return 0;
    }

    reset();
    m_path = path;

    std::string section;
    std::string token;
    std::string value;
    while (ScanType(section, true))
    {
        if (section == "[int]")
        {
            while (ScanStr(&token))
            {
                int v = ScanInt((bool*)0);
                m_intMap.insert(token.c_str(), v);
            }
        }
        else if (section == "[float]")
        {
            while (ScanStr(&token))
            {
                float f = ScanFloat((bool*)0);
                m_floatMap.insert(token.c_str(), f);
            }
        }
        else if (section == "[clong]")
        {
            while (ScanStr(&token))
            {
                int v = ScanInt((bool*)0);
                m_clongMap.insert(token.c_str(), v);
            }
        }
        else if (section == "[cfloat]")
        {
            while (ScanStr(&token))
            {
                float f = ScanFloat((bool*)0);
                m_cfloatMap.insert(token.c_str(), f);
            }
        }
        else if (section == "[string]")
        {
            while (ScanStr(&token))
            {
                ScanStr(&value);
                m_strMap.insert(token.c_str(), value);
            }
        }
    }
    return 1;
}