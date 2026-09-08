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
    // b28c-L10：ORIG 0x0807fe8a `cmpl $0x0,-0xc(%ebp); jne HIT`，空值分支 fall-through
    // `mov $-1,%eax; jmp end`、命中分支跳走。早期返回形 `if(p==0)return -1; return *p;`
    // 与 ORIG 逐条同形；goto 尾标签形会在跳转目标前多 1 条 nop（19 vs 18）。
    if (p == 0)
        return -1;
    return *p;
}

int CNRDScriptData::getCLong(const char* key) const
{
    const int* p = m_clongMap.get(key);
    if (p == 0)                 // b28c-L10：同 getInt，ORIG 0x…getCLongEPKc 同型
        return -1;
    return *p;
}

float CNRDScriptData::getFloat(const char* key) const
{
    const float* p = m_floatMap.get(key);
    if (p == 0)                 // b28c-L10：同上，ORIG 0x…getFloatEPKc 同型
        return -1.0f;
    return *p;
}

float CNRDScriptData::getCFloat(const char* key) const
{
    const float* p = m_cfloatMap.get(key);
    if (p == 0)                 // b28c-L10：同族（台账极性族含此条）
        return -1.0f;
    return *p;
}

const char* CNRDScriptData::getString(const char* key) const
{
    const std::string* p = m_strMap.get(key);
    if (p == 0)                 // b28c-L10：同族：命中分支跳走、默认串走 fall-through
        return "CNRDScriptData::Error";
    return p->c_str();
}

int CNRDScriptData::import(const char* path)
{
    if (!loadRDARScriptFile("Data/", path))
    {
        // ORIG import 失败分支（0x807fb4e 起）：logFormat 记录后返回 0。
        LogManager::logFormat(0, "../../DNFShared/CNRDScriptData.cpp", __FUNCTION__, 0x33,
                                 "CNRDScriptData::import: fail to load file - %s", path);
        return 0;
    }

    reset();
    m_path = path;

    std::string section;
    std::string token;
    std::string value;
    // ORIG 39-41：函数级两个带初值 temps（int→-0x54 `movl $0x0`；float→-0x58
    // `mov $0x0,%eax; mov %eax,-0x58`），供 [int]/[float] 两支复用；
    // 44-50 段首扫描为 `movl $1,0x4(%esp); call ScanType; xor $1; test; jne 远端出口岛`
    // ⇒ for(;;) + `if (!ScanType(...)) break;`（P164 + P32 取反物化），内层
    // `while (ScanStr(&token))` 保持 test/jne 回边形（ORIG 193-194 实证）。
    int v = 0;
    float f = 0;
    for (;;)
    {
        if (!ScanType(section, true))
        {
            break;
        }
        if (section == "[int]")
        {
            while (ScanStr(&token))
            {
                v = ScanInt((bool*)0);
                m_intMap.insert(token.c_str(), v);
            }
        }
        else if (section == "[float]")
        {
            while (ScanStr(&token))
            {
                f = ScanFloat((bool*)0);
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