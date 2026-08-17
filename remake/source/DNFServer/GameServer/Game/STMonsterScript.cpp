// ============================================================================
// df_game_r 还原 —— STMonsterScript 类方法（G5 怪物脚本表）
// ============================================================================

#include "STMonsterScript.h"

#include <stdio.h>
#include <string.h>

extern "C" bool sub_loadRDARScriptFile(const char* dir, const char* path)
    asm("_Z18loadRDARScriptFilePKcS0_");
extern "C" bool sub_ScanType(std::string& line, bool value)
    asm("_Z8ScanTypeRSsb");
extern "C" int sub_ScanInt(int* out) asm("_Z7ScanIntPi");

STMonsterScript::STMonsterScript()
{
    Clear();
}

STMonsterScript::~STMonsterScript()
{
}

STMonsterScript& STMonsterScript::operator=(const STMonsterScript& other)
{
    if (this != &other)
    {
        m_field0 = other.m_field0;
        m_str4 = other.m_str4;
        m_vec7c = other.m_vec7c;
    }
    return *this;
}

void STMonsterScript::Clear()
{
    m_field0 = 0;
    m_str4.clear();
    m_vec7c.clear();
}

int ImportMonsterScript(STMonsterScript* script, int type, const char* path,
                        bool flag)
{
    (void)type;
    (void)flag;
    if (!sub_loadRDARScriptFile("Script/MonsterScript", path))
    {
        return 0;
    }
    script->Clear();
    std::string line;
    int value = 0;
    if (!sub_ScanType(line, true))
    {
        return 0;
    }
    if (!sub_ScanInt(&value))
    {
        return 0;
    }
    script->m_field0 = value;
    if (!sub_ScanType(line, true))
    {
        return 0;
    }
    script->m_str4 = line;
    return 1;
}

bool ImportMonsterBaseTable(STMonsterScript* script, const char* path)
{
    return ImportMonsterScript(script, 0, path, false) != 0;
}

bool exportMonsterScript(
    STMonsterScript* script, const char* path,
    std::map<std::string, std::vector<std::string> >& extra)
{
    (void)extra;
    FILE* file = fopen(path, "w");
    if (!file)
    {
        return false;
    }
    fprintf(file, "%d\n%s\n", script->m_field0, script->m_str4.c_str());
    fclose(file);
    return true;
}

int getMonsterTableIndex(const STMonsterScript* script, int a, int b,
                         bool c, bool d)
{
    (void)a;
    (void)b;
    (void)c;
    (void)d;
    return script ? script->m_field0 : -1;
}

void setBasicAbilityByLevel(STMonsterScript* script, ActiveStaticInfo* info,
                            unsigned int level, int dungeonIdx, int roleType,
                            bool a, bool b)
{
    (void)script;
    (void)info;
    (void)level;
    (void)dungeonIdx;
    (void)roleType;
    (void)a;
    (void)b;
}

int getRandomChampionElement(STMonsterScript* script, int seed)
{
    (void)script;
    return (seed & 0x7fffffff) % 4;
}
