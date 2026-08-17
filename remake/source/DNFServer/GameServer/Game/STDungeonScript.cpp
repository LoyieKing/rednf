// ============================================================================
// df_game_r 还原 —— STDungeonScript 类方法（G5 副本脚本表）
// ORIG：Clear 0x0836abc6 / ctor 0x0836b5e2 / dtor 0x0836bd5c；
// 类布局见 CDungeon.h（0x50a）。ImportDungeonScript 0x088e5faa /
// exportDungeonScript 0x088f0a98 / ImportAdvanceAltarDungeonTag 0x088f20a4
// 为脚本解析/导出（语义简化实现）。
// ============================================================================

#include <stdio.h>
#include <string.h>

#include "CDungeon.h"

extern "C" bool sub_loadRDARScriptFile(const char* dir, const char* path)
    asm("_Z18loadRDARScriptFilePKcS0_");
extern "C" bool sub_ScanType(std::string& line, bool value)
    asm("_Z8ScanTypeRSsb");
extern "C" int sub_ScanInt(int* out) asm("_Z7ScanIntPi");

STDungeonScript::STDungeonScript()
{
    Clear();
}

STDungeonScript::~STDungeonScript()
{
}

void STDungeonScript::Clear()
{
    m_0 = 0;
    m_4 = 0;
    m_8 = 0;
    m_c = 0;
    m_str10.clear();
    m_14 = 0;
    m_18 = 0;
    m_str1c.clear();
    m_str20.clear();
    m_str24.clear();
    m_str28.clear();
    m_2c = 0;
    m_str30.clear();
    m_34 = 0;
    m_vec38.clear();
    m_44 = 0;
    m_vec48.clear();
    m_str54.clear();
    m_str58.clear();
    m_str74.clear();
    m_78 = 0;
    m_88 = 0;
    m_vec98.clear();
    m_a4 = 0;
    m_a8 = 0;
    m_ac = 0;
    m_b0 = 0;
    m_vecc8.clear();
    m_vecd4.clear();
    m_vece0.clear();
    m_vecec.clear();
    m_140 = 0;
    m_141 = 0;
    m_142 = 0;
    m_vec144.clear();
    m_150 = 0;
    m_154 = 0;
    m_158 = 0;
    m_159 = 0;
    m_vec15c.clear();
    m_mazeList170.clear();
    m_str17c.clear();
    m_statMap290.clear();
    m_2a8 = 0;
    m_2a9 = 0;
    m_2aa = 0;
    m_2ab = 0;
    m_2ac = 0;
    m_2bc[0] = 0;
    m_2bc[1] = 0;
    m_2bc[2] = 0;
    m_vec2c8.clear();
    m_dimension308.clear();
    m_314 = 0;
    m_315 = 0;
    m_316 = 0;
    m_317 = 0;
    m_dailySchedule.clear();
    m_324 = 0;
    m_325 = 0;
    m_338 = 0;
    m_33c = 0;
    m_map340.clear();
    m_358 = 0;
}

bool ImportDungeonScript(STDungeonScript* script, const char* path)
{
    if (!sub_loadRDARScriptFile("Script/DungeonScript", path))
    {
        return false;
    }
    script->Clear();
    std::string line;
    int value = 0;
    if (!sub_ScanType(line, true))
    {
        return false;
    }
    if (!sub_ScanInt(&value))
    {
        return false;
    }
    script->m_0 = value;
    if (!sub_ScanInt(&value))
    {
        return false;
    }
    script->m_4 = value;
    return true;
}

bool exportDungeonScript(STDungeonScript* script, const char* path)
{
    FILE* file = fopen(path, "w");
    if (!file)
    {
        return false;
    }
    fprintf(file, "%d\n%d\n", script->m_0, script->m_4);
    fclose(file);
    return true;
}

bool ImportAdvanceAltarDungeonTag(STDungeonScript* script, std::string str,
                                  const char* path)
{
    (void)str;
    (void)path;
    return ImportDungeonScript(script, path);
}
