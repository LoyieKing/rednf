// ============================================================================
// df_game_r 还原 —— STStackableScript（G5 堆叠物脚本表）
// Clear 0x0899087a / ctor 0x0898f218 / copy 0x0898f8e6 / dtor 0x0899013c /
// operator= 0x089c418e；Arad_STStackableScript 0x089c33fa..0x089c4052。
// ============================================================================

#include "STStackableScript.h"

#include <string.h>

#include "DNFLexWrapper.h"

void STStatChangeVector::clear()
{
    m_field0 = 0;
}

Arad_STStackableScript::Arad_STStackableScript()
{
    AradStClear();
}

Arad_STStackableScript::~Arad_STStackableScript()
{
}

Arad_STStackableScript& Arad_STStackableScript::operator=(
    const Arad_STStackableScript& other)
{
    if (this != &other)
    {
        m_0 = other.m_0;
        m_4 = other.m_4;
        m_str8 = other.m_str8;
        m_strc = other.m_strc;
        m_str10 = other.m_str10;
    }
    return *this;
}

void Arad_STStackableScript::AradStClear()
{
    m_0 = 0;
    m_4 = -1;
    m_str8 = "";
    m_strc = "";
    m_str10 = "";
}

STStackableScript::STStackableScript()
{
    Clear();
}

STStackableScript::STStackableScript(STStackableScript& other)
    : STItemScript(other),
      m_arad178(other.m_arad178),
      m_vec190(other.m_vec190),
      m_rabbit19c(other.m_rabbit19c),
      m_str1d8(other.m_str1d8),
      m_vec1ec(other.m_vec1ec),
      m_place1f8(other.m_place1f8),
      m_passive208(other.m_passive208),
      m_server214(other.m_server214),
      m_vec224(other.m_vec224),
      m_str230(other.m_str230),
      m_str234(other.m_str234),
      m_str238(other.m_str238),
      m_vec244(other.m_vec244),
      m_vec250(other.m_vec250),
      m_vec25c(other.m_vec25c),
      m_package268(other.m_package268),
      m_package274(other.m_package274),
      m_package2a4(other.m_package2a4),
      m_str368(other.m_str368),
      m_vec374(other.m_vec374),
      m_map3a4(other.m_map3a4),
      m_vec3c0(other.m_vec3c0),
      m_enchant3f4(other.m_enchant3f4),
      m_str410(other.m_str410),
      m_set480(other.m_set480),
      m_set498(other.m_set498),
      m_str4b4(other.m_str4b4),
      m_set4d4(other.m_set4d4),
      m_vec4fc(other.m_vec4fc),
      m_set50c(other.m_set50c)
{
    m_1a8 = other.m_1a8;
    m_1ac = other.m_1ac;
    m_1b0 = other.m_1b0;
    m_1b4 = other.m_1b4;
    m_1b8 = other.m_1b8;
    m_1d4 = other.m_1d4;
    m_1dc = other.m_1dc;
    m_1e0 = other.m_1e0;
    m_1e4 = other.m_1e4;
    m_23c = other.m_23c;
    m_240 = other.m_240;
}

STStackableScript::~STStackableScript()
{
}

STStackableScript& STStackableScript::operator=(const STStackableScript& other)
{
    if (this != &other)
    {
        STItemScript::operator=(other);
        m_arad178 = other.m_arad178;
        m_vec190 = other.m_vec190;
        m_rabbit19c = other.m_rabbit19c;
        m_vec1ec = other.m_vec1ec;
        m_place1f8 = other.m_place1f8;
        m_passive208 = other.m_passive208;
        m_server214 = other.m_server214;
        m_vec224 = other.m_vec224;
        m_str230 = other.m_str230;
        m_str234 = other.m_str234;
        m_str238 = other.m_str238;
        m_vec244 = other.m_vec244;
        m_vec250 = other.m_vec250;
        m_vec25c = other.m_vec25c;
        m_package268 = other.m_package268;
        m_package274 = other.m_package274;
        m_package2a4 = other.m_package2a4;
        m_str368 = other.m_str368;
        m_vec374 = other.m_vec374;
        m_map3a4 = other.m_map3a4;
        m_vec3c0 = other.m_vec3c0;
        m_enchant3f4 = other.m_enchant3f4;
        m_str410 = other.m_str410;
        m_set480 = other.m_set480;
        m_set498 = other.m_set498;
        m_str4b4 = other.m_str4b4;
        m_set4d4 = other.m_set4d4;
        m_vec4fc = other.m_vec4fc;
        m_set50c = other.m_set50c;
    }
    return *this;
}

void STStackableScript::Clear()
{
    STItemScript::Clear();
    m_field4 = 0;
    m_arad178.AradStClear();
    m_rabbit19c.clear();
    m_vec190.clear();
    m_1a8 = 0;
    m_1ac = 0;
    m_1b0 = 0;
    m_1b4 = -1;
    m_1b8 = 0;
    m_str1d8 = "";
    m_1dc = 0;
    m_1d4 = 0;
    m_1e0 = 0x27;
    m_1e4 = 0;
    m_str238 = "";
    m_23c = 1;
    m_240 = -1;
    m_1b9 = 0;
    m_1ba = 0;
    m_1bb = 0;
    m_statChange34c.clear();
    m_1bc = 0;
    m_1bd = 0;
    m_1be = 0;
    m_1bf = 0;
    m_1c0 = 0;
    m_1c4 = 0;
    m_1c8 = 0;
    m_vec1ec.clear();
    m_place1f8.clear();
    m_passive208.clear();
    m_server214.clear();
    m_vec224.clear();
    m_str230.clear();
    m_str234.clear();
    m_vec244.clear();
    m_vec250.clear();
    m_vec25c.clear();
    m_package268.clear();
    m_package274.clear();
    m_package2a4.clear();
    m_str368.clear();
    m_vec374.clear();
    m_map3a4.clear();
    m_vec3c0.clear();
    m_enchant3f4.clear();
    m_str410.clear();
    m_set480.clear();
    m_set498.clear();
    m_str4b4.clear();
    m_set4d4.clear();
    m_vec4fc.clear();
    m_set50c.clear();
}

void RecipeInfo::additionalInfo(STStackableScript& script)
{
    (void)script;
}

bool ImportStackableScript(STStackableScript* script, const char* path,
                           bool flag, int idx)
{
    (void)flag;
    (void)idx;
    if (!loadRDARScriptFile("Script/StackableScript", path))
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
    script->m_field0 = 0;
    if (!ScanType(line, true))
    {
        return false;
    }
    script->m_str6c = line;
    if (!ScanInt(&value))
    {
        return false;
    }
    script->m_1a8 = value;
    return true;
}

bool StackableScriptSpecialMonsterDrop(STStackableScript* script,
                                       std::string str, const char* path)
{
    (void)script;
    (void)str;
    (void)path;
    return false;
}
