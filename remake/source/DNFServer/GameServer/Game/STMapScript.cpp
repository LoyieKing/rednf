// ============================================================================
// df_game_r 还原 —— STMapScript 类方法（G5 地图脚本表）
// ORIG：Clear 0x0836c688 / ctor 0x0836cc94 / copy 0x0837e698 / dtor 0x0836d4bc；
// 类布局见 CMap.h（0x37c）。ImportMapScript 0x089dcf54 / ExportMapScript 0x089e3c55
// 为脚本解析/导出（语义简化实现）。
// ============================================================================

#include <stdio.h>
#include <string.h>

#include "CMap.h"
#include "DNFLexWrapper.h"

STMapScript::STMapScript()
{
    Clear();
}

STMapScript::STMapScript(const STMapScript& other)
    : m_list4(other.m_list4),
      m_vec10(other.m_vec10),
      m_list2c(other.m_list2c),
      m_vec34(other.m_vec34),
      m_vec40(other.m_vec40),
      m_list58(other.m_list58),
      m_vec60(other.m_vec60),
      m_vec250(other.m_vec250),
      m_vec25c(other.m_vec25c),
      m_vec278(other.m_vec278),
      m_vec288(other.m_vec288),
      m_vec294(other.m_vec294),
      m_map2b8(other.m_map2b8),
      m_timeLine330(other.m_timeLine330),
      m_str34c(other.m_str34c),
      m_str370(other.m_str370)
{
    m_0 = other.m_0;
    m_c = other.m_c;
    m_4c = other.m_4c;
    m_268 = other.m_268;
    m_284 = other.m_284;
    m_324 = other.m_324;
    m_328 = other.m_328;
    m_36e = other.m_36e;
    memcpy(m_pathArea1, other.m_pathArea1, sizeof(m_pathArea1));
    memcpy(m_pathArea2, other.m_pathArea2, sizeof(m_pathArea2));
}

STMapScript::~STMapScript()
{
}

void STMapScript::Clear()
{
    m_0 = -1;
    m_list4.clear();
    m_c = 0;
    m_vec10.clear();
    m_list2c.clear();
    m_vec34.clear();
    m_vec40.clear();
    m_4c = 0;
    m_list58.clear();
    m_vec60.clear();
    memset(m_pathArea1, 0xff, sizeof(m_pathArea1));
    memset(m_pathArea2, 0xff, sizeof(m_pathArea2));
    m_vec250.clear();
    m_vec25c.clear();
    m_268 = 0;
    m_vec278.clear();
    m_284 = 0;
    m_vec288.clear();
    m_vec294.clear();
    m_map2b8.clear();
    m_324 = 0;
    m_328 = 0;
    m_str34c.clear();
    m_36e = 0;
    m_str370.clear();
}

bool ImportMapScript(STMapScript* script, const char* path)
{
    if (!loadRDARScriptFile("Script/MapScript", path))
    {
        return false;
    }
    script->Clear();
    std::string line;
    int value = 0;
    if (!ScanType(line, true))
    {
        return false;
    }
    if (!ScanInt(&value))
    {
        return false;
    }
    script->m_0 = value;
    if (!ScanInt(&value))
    {
        return false;
    }
    script->m_c = value;
    return true;
}

bool ExportMapScript(STMapScript* script, const char* path)
{
    FILE* file = fopen(path, "w");
    if (!file)
    {
        return false;
    }
    fprintf(file, "%d\n%d\n", script->m_0, script->m_c);
    fclose(file);
    return true;
}
