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
#include "LogManager.h"
#include "ScriptValue.h"
#include "SyncScriptDeps.h"   // toTString / getCharacterJob
#include "CreatureScript.h"  // ENUM_ELEMENT（getElementName 形参）
#include "CEnvironment.h"    // less_TCHAR / SpinLock（GetSkillPhase static map）

// ORIG 全局记账（0x949b28c/0x949b294，C1/C2 递增、D1 递减）：
// 实例计数与按 sizeof 累积的分配字节数。
int g_EquipmentParameterInfoCount = 0;
int g_EquipmentParameterInfoSize = 0;

// ORIG C1（0x8919ce8）：成员默认构造后，先更新全局记账
// （count++、size = count * sizeof），再调 clear。
EquipmentParameterInfo::EquipmentParameterInfo()
{
    ++g_EquipmentParameterInfoCount;
    g_EquipmentParameterInfoSize =
        g_EquipmentParameterInfoCount * sizeof(EquipmentParameterInfo);
    clear();
}

// ORIG C2（0x891a0d4）：成员默认构造 + 函数体 `*this = other`（0x891a2a2 call op=）。
// ORIG C2（0x891a0d4）：成员默认构造 + 全局记账 + `*this = other`。
EquipmentParameterInfo::EquipmentParameterInfo(const EquipmentParameterInfo& other)
{
    ++g_EquipmentParameterInfoCount;
    g_EquipmentParameterInfoSize =
        g_EquipmentParameterInfoCount * sizeof(EquipmentParameterInfo);
    *this = other;
}

// ORIG D1（0x891a4c6）：先递减全局记账，再析构成员（编译器生成）。
EquipmentParameterInfo::~EquipmentParameterInfo()
{
    --g_EquipmentParameterInfoCount;
    g_EquipmentParameterInfoSize =
        g_EquipmentParameterInfoCount * sizeof(EquipmentParameterInfo);
}


// ORIG 0x089193c0（0x928 字节）：标量清零 + 数组循环清零 + 容器 clear。
// 数组循环（m_pad30/m_pad29c/m_pad39c 区）按 ORIG 的 `((int*)this)[i+K]`
// 大数组视图书写；add-vs-lea 属 P30 结构性伪影（语义等价）。
void EquipmentParameterInfo::clear()
{
    m_field0 = 2;
    m_hpMax = 0;
    m_hpMaxRate = 0;
    m_mpMax = 0;
    m_mpMaxRate = 0;
    m_physicalAttack = 0;
    m_physicalDefense = 0;
    m_magicalAttack = 0;
    m_20 = 0;
    m_magicalDefense = 0;
    m_28 = 0;
    m_2c = 0;
    {
        int i;
        int* p = (int*)this;
        for (i = 0; i <= 1; ++i)
        {
            p[i + 12] = 0;
            p[i + 14] = 0;
            p[i + 16] = 0;
            p[i + 18] = 0;
        }
    }
    m_50 = 0;
    {
        int i;
        for (i = 0; i <= 1; ++i)
        {
            m_phyAtt[i] = 0;
            m_phyDef[i] = 0;
            m_magAtt[i] = 0;
            m_magDef[i] = 0;
        }
    }
    m_physicalAbsoluteDamage = 0;
    m_physicalDamageReduce = 0;
    m_physicalAbsoluteDefense = 0;
    m_magicalAbsoluteDamage = 0;
    m_magicalDamageReduce = 0;
    m_magicalAbsoluteDefense = 0;
    m_94 = 0;
    m_98 = 0;
    m_9c = 0;
    m_a0 = 0;
    m_a4 = 0;
    m_a8 = 0;
    m_ac = 0;
    {
        int i;
        for (i = 0; i <= 3; ++i)
        {
            ((int*)this)[i + 44] = 0;
        }
    }
    m_allElementalResistance = 0;
    {
        int i;
        for (i = 0; i <= 16; ++i)
        {
            ((int*)this)[i + 49] = 0;
        }
    }
    m_inventoryLimit = 0;
    m_hpRegenSpeed = 0;
    m_mpRegenSpeed = 0;
    m_mpRegenSpeedRate = 0;
    m_moveSpeed = 0;
    m_xMoveDashSpeed = 0;
    m_yMoveDashSpeed = 0;
    m_attackSpeed = 0;
    m_castSpeed = 0;
    m_hitRecovery = 0;
    m_jumpPower = 0;
    m_physicalCriticalHit = 0;
    m_magicalCriticalHit = 0;
    m_elementalProperty = 4;
    m_activeStatusType = 0x12;
    m_activeStatusValue = 0;
    m_activeStatusLevel = 0;
    m_activeStatusFlag = 0;
    m_activeStatusValue2 = 0;
    m_vec158.clear();
    m_attackedActiveStatusType = 0x12;
    m_attackedActiveStatusValue = 0;
    m_attackedActiveStatusLevel = 0;
    m_attackedActiveStatusFlag = 0;
    m_attackedActiveStatusValue2 = 0;
    m_vec17c.clear();
    m_164 = 0;
    m_vec240.clear();
    m_164 = 0;
    m_attackedActiveStatusValue = 0;
    m_attackedActiveStatusLevel = 0;
    m_attackedActiveStatusFlag = 0;
    m_attackedActiveStatusValue2 = 0;
    m_vec17c.clear();
    m_aura188.clear();
    m_vec194.clear();
    m_addAbsoluteDamageType = 4;
    m_addAbsoluteDamageValue = 0;
    m_addAbsoluteDamageExtra = 0;
    m_addAbsoluteDefenseType = 4;
    m_addAbsoluteDefenseValue = 0;
    m_addAbsoluteDefenseExtra = 0;
    m_pushAside = 0;
    m_liftUp = 0;
    m_stuck = 0;
    m_ignoreDefense = 0;
    {
        int i;
        for (i = 0; i <= 2; ++i)
        {
            ((float*)this)[i + 0x8c] = 0.0f;
        }
    }
    m_hpDrain = 0;
    m_mpDrain = 0;
    {
        int i;
        for (i = 0; i <= 1; ++i)
        {
            m_levelUpSkills[i].clear();
        }
    }
    m_avatar264.clear();
    m_270 = -1;
    m_roomListMoveSpeedRate = 0;
    m_str278 = "";
    m_str27c = "";
    m_antiEvil = 0;
    m_rigidity = 0;
    {
        int i;
        for (i = 0; i <= 3; ++i)
        {
            ((int*)this)[i + 0xa2] = 0;
        }
    }
    {
        int i;
        for (i = 0; i <= 17; ++i)
        {
            ((int*)this)[i + 0xa7] = 0;
            ((int*)this)[i + 0xbb] = 0;
            ((int*)this)[i + 0xcb] = 0;
        }
    }
    {
        int i;
        for (i = 0; i <= 4; ++i)
        {
            ((int*)this)[i + 0xdd] = 0;
        }
    }
    m_388[0] = 0;
    m_388[1] = 0;
    *(float*)&m_388[2] = 1.0f;
    *(float*)&m_388[3] = 1.0f;
    m_388[4] = 0;
    *(int*)&m_pad39c[0] = 0;
    m_414 = 0;
    {
        int i;
        for (i = 0; i <= 1; ++i)
        {
            ((int*)this)[i + 0xc] = 0;
            ((int*)this)[i + 0xe] = 0;
            ((int*)this)[i + 0x10] = 0;
            ((int*)this)[i + 0x12] = 0;
            ((int*)this)[i + 0xf8] = 0;
            ((int*)this)[i + 0x102] = 0;
        }
    }
    {
        int i;
        for (i = 0; i <= 3; ++i)
        {
            ((int*)this)[i + 0xe8] = 0;
            ((int*)this)[i + 0xec] = 0;
            ((int*)this)[i + 0xf0] = 0;
            ((int*)this)[i + 0xf4] = 0;
            ((int*)this)[i + 0xfa] = 0;
            ((float*)this)[i + 0x100] = 1.0f;
        }
    }
    m_410 = 0;
    m_skill1b8.clear();
    m_descMap1c4.clear();
    m_skill1dc.clear();
    m_skill1e8.clear();
    m_vec1f4.clear();
    m_aura200.clear();
    m_vec214.clear();
    m_decreaseCriticalDamage = 0;
    m_210 = 0;
    m_119.m_424 = 0;
    m_119.m_425 = 0;
    m_434 = 0;
    m_438 = -1;
    m_43c = -1;
    m_upgradeCostDiscount = 0;
    m_448 = 0;
    m_assaultCostDiscount = 0;
    m_allElementalAttack = 0;
    m_8c = 0;
    m_90 = 0;
    m_rebirth450.clear();
    m_vec468.clear();
    m_var474.clear();
    m_440 = 0;
}

// ORIG 0x089c2e98：遍历查找 str 已存在的 key（无则 size()+1），m[key]=str 后返回 key。
int EquipmentParameterInfo::addString(std::string str)
{
    int key = -1;
    char b = 0;
    if (!b)
    {
        std::map<int, std::string>::iterator it = m_descMap1c4.begin();
        std::map<int, std::string>::iterator e = m_descMap1c4.end();
        while (it != e && (b ^= 1))
        {
            std::pair<const int, std::string>& p = *it;
            if (b)
            {
                if (p.second == str)
                {
                    key = p.first;
                }
                else
                {
                    b = 0;
                }
            }
            ++it;
        }
    }
    if (key == -1)
    {
        key = (int)m_descMap1c4.size() + 1;
        m_descMap1c4[key] = str;
    }
    return key;
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
    : m_pair4()
{
    clear();
}

void EquipmentParameterInfo::RebirthAppendageinfo::clear()
{
    m_field0 = 0;
    m_fieldc = 0;
    m_field14 = 0;
    m_field10 = 0;
    m_pair4.first = -1;
    m_pair4.second = 0;
}





// ---- ORIG 全局枚举转换 helper（真实符号，定义在各自 TU；此处仅声明） ----
int getCharacterJobToEnumValue(std::string& s);        // 0x89bbc0f
int getActiveStatusToEnumValue(std::string& s);        // 0x88c29a2
int getStateToEnumValue(std::string& s);               // 0x89bbc05
int getElementToEnumValue(std::string& s);             // 0x89bca42
int getAttackConditionToEnumValue(std::string& s);     // 0x89bcb6d
int getCategoryToEnumValue(std::string& s);            // 0x89bcbf2
int getGradeToEnumValue(std::string& s);               // 0x89bcd5b
int getChangeStatusType(std::string& s);               // 0x8892b0d
int getObjectTypeToEnumValue(std::string& s);          // 0x89bbd99
int getTargetToEnumValue(std::string& s);              // 0x88c2c34
int scanModuleType();                                  // 0x88c0b9a（ORIG 返回 int）
bool ScanTypeOrStr(bool& isType, std::string& s, bool consume);  // 0x88bc4e5 族
int getElement(const char* s);                         // 0x88bfe5b（CreatureScript.cpp）
int getActiveStatusType(const std::string& s);         // 0x88944fb（CreatureScript.cpp）
const char* getElementName(ENUM_ELEMENT element);      // 0x88bfee0（CreatureScript.cpp）

// ---- 由 CStackableItem.cpp 提供（ORIG 0x08a86d28，'[hat avatar]' 带括号变体） ----
int GetEquipmentTypeStringToEnum(std::string& type);

// ============================================================================
// 本 TU 内 ORIG helper（地址均在 EquipmentParameterInfo.cpp 域）：
//   getTargetTag(0x89a7413)/getStatusType(0x89a74a2)/getSign(0x89a752a) 为 static
//   （ORIG 小写 t）；GetStat/GetStatValue/GetStatByCondition/GetApplyStatusInRange/
//   GetSkillLevelUp/getBuffToEnumValue/getSkillDataExpansionToEnumValue/
//   getEquipmentTypeToEnumValue 为全局 T。scanScriptValue/importSkillDataUp 属
//   0x088bxxxx 域，但为 readAction 直接解析依赖，并入本 TU 统一提供。
// ============================================================================

static int getTargetTag(int* out)
{
    if (out == 0) return 0;
    std::string s;
    if (!ScanStr(&s)) return 0;
    int v = getTargetToEnumValue(s);
    if (v == 10) return 0;
    *out = v;
    return 1;
}

static int getStatusType(int* out)
{
    if (out == 0) return 0;
    std::string s;
    if (!ScanStr(&s)) return 0;
    int v = getChangeStatusType(s);
    *out = v;
    if (*out != 0x4d) return 1;
    return 0;
}

static int getSign(int* out)
{
    if (out == 0) return 0;
    std::string s;
    if (!ScanStr(&s)) return 0;
    if (s == "+") { *out = 0; return 1; }
    if (s == "%") { *out = 1; return 1; }
    return 0;
}

int GetStatValue(std::vector<float>* v)
{
    std::string s;
    if (!ScanStr(&s)) return 0;
    float st = (float)getChangeStatusType(s);
    if (!ScanStr(&s)) return 0;
    float pct = (s == "%") ? 1.0f : 0.0f;
    bool ok = false;
    int val = ScanInt(&ok);
    if (ok)
    {
        v->push_back(st);
        v->push_back(pct);
        v->push_back((float)val);
        return 1;
    }
    return 0;
}

void GetStat(std::vector<float>* v)
{
    v->push_back(12.0f);
    GetStatValue(v);
}

int GetStatByCondition(std::vector<float>* v)
{
    v->push_back(38.0f);
    std::string s;
    if (!ScanStr(&s)) return 0;
    float st = (float)getChangeStatusType(s);
    if (!ScanStr(&s)) return 0;
    float pct = (s == "%") ? 1.0f : 0.0f;
    bool ok = false;
    int val = ScanInt(&ok);
    if (ok)
    {
        v->push_back(st);
        v->push_back(pct);
        v->push_back((float)val);
        return 1;
    }
    return 0;
}

int GetApplyStatusInRange(std::vector<float>* v)
{
    v->push_back(39.0f);
    std::string s;
    if (!ScanStr(&s)) return 0;
    float st = (float)getChangeStatusType(s);
    if (!ScanStr(&s)) return 0;
    float pct = (s == "%") ? 1.0f : 0.0f;
    // ORIG 0x089912c8：ScanInt(bool*) 形式，失败不中止（值取 0 继续）。
    bool ok1 = false;
    int v1 = ScanInt(&ok1);
    bool ok2 = false;
    int v2 = ScanInt(&ok2);
    v->push_back(st);
    v->push_back(pct);
    v->push_back((float)v1);
    v->push_back((float)v2);
    return 1;
}

int GetSkillLevelUp(std::vector<float>* v)
{
    std::string s;
    bool isType = false;
    int val = -1;
    for (;;)
    {
        if (!ScanTypeOrStr(isType, s, true)) return 0;
        if (isType)
        {
            return (s == "[/skill levelup]") ? 1 : 0;
        }
        v->push_back(29.0f);
        if (s == "[common]")
        {
            v->push_back(11.0f);
        }
        else
        {
            v->push_back((float)getCharacterJob(s.c_str()));
        }
        if (!ScanInt(&val)) return 0;
        v->push_back((float)val);
        if (!ScanInt(&val)) return 0;
        v->push_back((float)val);
    }
}

int getBuffToEnumValue(std::string& s)
{
    if (s == "superarmor")   return 0;
    if (s == "unbreakable")  return 1;
    if (s == "camoflage")    return 2;
    return 3;
}

int getSkillDataExpansionToEnumValue(const std::string& s)
{
    if (s == "[static]")             return 0;
    if (s == "[level]")              return 1;
    if (s == "[cooltime]")           return 2;
    if (s == "[mp]")                 return 3;
    if (s == "[maintain mp]")        return 4;
    if (s == "[skill consume item]") return 5;
    if (s == "[casting time]")       return 6;
    return 7;
}

int getEquipmentTypeToEnumValue(std::string& s)
{
    if (s == "hat avatar")   return 0;
    if (s == "hair avatar")  return 1;
    if (s == "face avatar")  return 2;
    if (s == "coat avatar")  return 3;
    if (s == "pants avatar") return 4;
    if (s == "shoes avatar") return 5;
    if (s == "breast avatar") return 6;
    if (s == "waist avatar") return 7;
    if (s == "skin avatar")  return 8;
    if (s == "aurora avatar") return 9;
    if (s == "weapon")       return 10;
    if (s == "title name")   return 11;
    if (s == "coat")         return 12;
    if (s == "shoulder")     return 13;
    if (s == "pants")        return 14;
    if (s == "shoes")        return 15;
    if (s == "waist")        return 16;
    if (s == "amulet")       return 17;
    if (s == "wrist")        return 18;
    if (s == "ring")         return 19;
    if (s == "support")      return 20;
    if (s == "magic stone")  return 21;
    return 22;
}

// ---- importSkillDataUp（ORIG 0x88c049c，readAction [skill data up] 依赖） ----
// scanScriptValue/pushScriptValue/ScriptValue 由 ScriptValue.h/ScriptValue.cpp
// （FixScriptValuePipeline）提供；importSkillDataUp 在本文件末尾定义。

// ============================================================================
// readAction —— ORIG 0x89a13e5（0x602e 字节）逐分支转写。
// param 映射：line=param_1（仅 [target] 失败日志用）、info=param_2（字段写入）、
// out=param_3（关键字，dispatch 依据）、values=param_4（float 输出栈）、idx=param_5。
// 每个关键字 push 一组 float 并消费对应 token（ScanInt/ScanStr/ScanFloat/
// ScanTypeOrInt/ScanTypeOrStr/ScanType）；任一扫描失败返回 false（0），
// 成功返回 true（1）。ORIG 的 __lookup 静态 map + 惰性初始化仅为编译期
// 优化，行为等价于按关键字字符串 if/else；此处省略惰性初始化直接分派。
// ============================================================================
bool readAction(const std::string& line, EquipmentParameterInfo* info,
                std::string& out, std::vector<float>& values, int idx)
{
    (void)idx;
    std::string token;
    std::string token2;
    int iv;
    float fv;

    // 0x1248 [consume item]
    if (out == "[consume item]")
    {
        values.push_back(0.0f);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        return true;
    }
    // 0x1257 [target]
    if (out == "[target]")
    {
        int t = 0;
        if (!getTargetTag(&t)) return false;
        if (!ScanInt(&iv))
        {
            LogManager::logFormat(0, "../RDARScriptItem.cpp", __FUNCTION__, 0x125f,
                "[SCRIPT] invalid [TARGET] tag arguments. [%s]", line.c_str());
            iv = -1;
        }
        values.push_back(1.0f);
        values.push_back((float)t);
        values.push_back((float)iv);
        return true;
    }
    // 0x126a [duration]
    if (out == "[duration]")
    {
        values.push_back(2.0f);
        if (!scanScriptValue(values)) return false;
        return true;
    }
    // 0x127b [equipment duration]
    if (out == "[equipment duration]")
    {
        values.push_back(3.0f);
        if (!scanScriptValue(values)) return false;
        return true;
    }
    // 0x128c [skill duration]
    if (out == "[skill duration]")
    {
        values.push_back(52.0f);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        if (!ScanStr(&token)) return false;
        values.push_back((float)getSkillDataExpansionToEnumValue(token));
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        return true;
    }
    // 0x12a0 [reduce duration at pvp type module]
    if (out == "[reduce duration at pvp type module]")
    {
        values.push_back(26.0f);
        values.push_back(1.0f);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        return true;
    }
    // 0x12ac [reduce duration at pvp module]
    if (out == "[reduce duration at pvp module]")
    {
        values.push_back(26.0f);
        values.push_back(2.0f);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        return true;
    }
    // 0x12b8 [reduce duration to human armor]
    if (out == "[reduce duration to human armor]")
    {
        values.push_back(26.0f);
        values.push_back(3.0f);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        return true;
    }
    // 0x12c4 [reduce duration to human armor at pvp type module]
    if (out == "[reduce duration to human armor at pvp type module]")
    {
        values.push_back(26.0f);
        values.push_back(4.0f);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        return true;
    }
    // 0x12d0 [reduce duration to human armor at pvp module]
    if (out == "[reduce duration to human armor at pvp module]")
    {
        values.push_back(26.0f);
        values.push_back(4.0f);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        return true;
    }
    // 0x12dc [delay]
    if (out == "[delay]")
    {
        values.push_back(4.0f);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        return true;
    }
    // 0x12e7 [probability]
    if (out == "[probability]")
    {
        values.push_back(5.0f);
        if (!ScanFloat(&fv)) return false;
        values.push_back(fv);
        return true;
    }
    // 0x12f2 [reduce probability at pvp type module]
    if (out == "[reduce probability at pvp type module]")
    {
        values.push_back(27.0f);
        values.push_back(1.0f);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        return true;
    }
    // 0x12fe [reduce probability at pvp module]
    if (out == "[reduce probability at pvp module]")
    {
        values.push_back(27.0f);
        values.push_back(2.0f);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        return true;
    }
    // 0x130a [reduce probability to human armor]
    if (out == "[reduce probability to human armor]")
    {
        values.push_back(27.0f);
        values.push_back(0.0f);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        return true;
    }
    // 0x1316 [reduce probability to human armor at pvp type module]
    if (out == "[reduce probability to human armor at pvp type module]")
    {
        values.push_back(27.0f);
        values.push_back(3.0f);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        return true;
    }
    // 0x1322 [reduce probability to human armor at pvp module]
    if (out == "[reduce probability to human armor at pvp module]")
    {
        values.push_back(27.0f);
        values.push_back(4.0f);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        return true;
    }
    // 0x132e [active status] —— 先压 tag/状态/数值，再扫描附加状态子键（armor break…curse）
    if (out == "[active status]")
    {
        values.push_back(6.0f);
        if (!ScanStr(&token)) return false;
        values.push_back((float)getActiveStatusToEnumValue(token));
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        // 嵌套子键循环：ScanTypeOrInt 得 token，查表分派附加状态参数
        bool isType = false;
        bool isInt = false;
        for (;;)
        {
            if (ScanTypeOrInt(isType, token, isInt, true) == 0 && !isType && !isInt)
                return false;
            if (isType && (token == "armor break" || token == "weapon break" ||
                           token == "poison" || token == "lightning" ||
                           token == "bleeding" || token == "burn" ||
                           token == "bless" || token == "curse" ||
                           token == "blind" || token == "slow" || token == "haste" ||
                           token == "stone" || token == "confuse" || token == "weakness"))
            {
                // 各附加状态扫 0~4 个 float；此处按扫描流消费（ORIG 对每状态
                // 扫对应数量 float 入栈）。
                int cnt = 0;
                if (token == "haste" || token == "armor break" || token == "weapon break")
                    cnt = 2;
                else if (token == "bleeding" || token == "burn")
                    cnt = 1;
                else if (token == "curse")
                    cnt = 4;
                for (int k = 0; k < cnt; ++k)
                {
                    if (!ScanFloat(&fv)) return false;
                    values.push_back(fv);
                }
            }
            else
            {
                break;
            }
        }
        return true;
    }
    // 0x136b [disenchant]
    if (out == "[disenchant]")
    {
        if (!ScanInt(&iv)) return false;
        if (iv == 1)
            values.push_back(7.0f);
        return true;
    }
    // 0x1374 [buff]
    if (out == "[buff]")
    {
        values.push_back(8.0f);
        if (!ScanStr(&token)) return false;
        values.push_back((float)getBuffToEnumValue(token));
        return true;
    }
    // 0x137e [appendage]
    if (out == "[appendage]")
    {
        values.push_back(33.0f);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        return true;
    }
    // 0x138a [appendage unique]
    if (out == "[appendage unique]")
    {
        values.push_back(34.0f);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        return true;
    }
    // 0x1396 [aura]
    if (out == "[aura]")
    {
        values.push_back(35.0f);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        return true;
    }
    // 0x13a1 [add absolute damage]
    if (out == "[add absolute damage]")
    {
        values.push_back(9.0f);
        if (!ScanStr(&token)) return false;
        if (!ScanStr(&token)) return false;
        float pct = (token == "%") ? 1.0f : 0.0f;
        if (!ScanInt(&iv)) return false;
        values.push_back(pct);
        values.push_back((float)iv);
        return true;
    }
    // 0x13c8 [add absolute defense]
    if (out == "[add absolute defense]")
    {
        values.push_back(10.0f);
        if (!ScanStr(&token)) return false;
        if (!ScanStr(&token)) return false;
        float pct = (token == "%") ? 1.0f : 0.0f;
        if (!ScanInt(&iv)) return false;
        values.push_back(pct);
        values.push_back((float)iv);
        return true;
    }
    // 0x13e6 [add absolute defense percent]
    if (out == "[add absolute defense percent]")
    {
        values.push_back(43.0f);
        if (!ScanStr(&token)) return false;
        if (!ScanFloat(&fv)) return false;
        values.push_back(fv);
        return true;
    }
    // 0x13fe [aura active]
    if (out == "[aura active]")
    {
        values.push_back(11.0f);
        if (!ScanStr(&token)) return false;
        values.push_back((float)getChangeStatusType(token));
        if (!ScanStr(&token)) return false;
        if (!ScanFloat(&fv)) return false;
        if (!ScanInt(&iv)) return false;
        float pct = (token == "%") ? 1.0f : 0.0f;
        values.push_back(pct);
        values.push_back(fv);
        values.push_back((float)iv);
        return true;
    }
    // 0x1419 [apply status in range]
    if (out == "[apply status in range]")
    {
        GetApplyStatusInRange(&values);
        return true;
    }
    // 0x141e [stat by condition]
    if (out == "[stat by condition]")
    {
        GetStatByCondition(&values);
        return true;
    }
    // 0x1423 [stat level replace]
    if (out == "[stat level replace]")
    {
        values.push_back(40.0f);
        if (!ScanStr(&token)) return false;
        values.push_back((float)getChangeStatusType(token));
        if (!ScanStr(&token)) return false;
        float pct = (token == "%") ? 1.0f : 0.0f;
        if (!ScanInt(&iv)) return false;
        values.push_back(pct);
        values.push_back((float)iv);
        return true;
    }
    // 0x1432 [stat]
    if (out == "[stat]")
    {
        GetStat(&values);
        return true;
    }
    // 0x1435 [active status resistance]
    if (out == "[active status resistance]")
    {
        values.push_back(13.0f);
        if (!ScanStr(&token)) return false;
        float st = (float)getActiveStatusToEnumValue(token);
        if (!ScanInt(&iv)) return false;
        int v = iv;
        if (!ScanStr(&token)) return false;
        float pct = (token == "%") ? 1.0f : 0.0f;
        values.push_back(st);
        values.push_back(pct);
        values.push_back((float)v);
        return true;
    }
    // 0x144a [element resistance]
    if (out == "[element resistance]")
    {
        values.push_back(14.0f);
        if (!ScanStr(&token)) return false;
        values.push_back((float)getElementToEnumValue(token));
        if (!ScanStr(&token)) return false;
        float pct = (token == "%") ? 1.0f : 0.0f;
        if (!ScanInt(&iv)) return false;
        values.push_back(pct);
        values.push_back((float)iv);
        return true;
    }
    // 0x1461 [damage rate]
    if (out == "[damage rate]")
    {
        values.push_back(15.0f);
        if (!ScanStr(&token)) return false;
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        return true;
    }
    // 0x146e [active status duration]
    if (out == "[active status duration]")
    {
        values.push_back(41.0f);
        if (!ScanFloat(&fv)) return false;
        values.push_back(fv);
        return true;
    }
    // 0x1478 [active status buff event type]
    if (out == "[active status buff event type]")
    {
        values.push_back(42.0f);
        if (!ScanFloat(&fv)) return false;
        values.push_back(fv);
        return true;
    }
    // 0x14af [passive object] —— 循环扫描对象组，[/passive object] 终止
    if (out == "[passive object]")
    {
        values.push_back(16.0f);
        int cnt = 0;
        values.push_back(0.0f);
        bool isType = false;
        bool isInt = false;
        for (;;)
        {
            if (ScanTypeOrInt(isType, token, isInt, true) == 0 && !isType && !isInt)
                return false;
            if (isType && token == "[/passive object]")
            {
                if (values.size() > 1)
                    values[1] = (float)cnt;
                return true;
            }
            int a = 0;
            if (!ScanInt(&a)) return false;
            int b = 0;
            if (!ScanInt(&b)) return false;
            int c = 0;
            if (!ScanInt(&c)) return false;
            int d = 0;
            if (!ScanInt(&d)) return false;
            int e = 0;
            if (!ScanInt(&e)) return false;
            int f = 0;
            if (!ScanInt(&f)) return false;
            if (!ScanStr(&token)) return false;
            values.push_back((float)a);
            values.push_back((float)b);
            values.push_back((float)c);
            values.push_back((float)d);
            values.push_back((float)e);
            values.push_back((float)f);
            cnt = cnt + 1;
        }
    }
    // 0x14ec [summon monster]
    if (out == "[summon monster]")
    {
        values.push_back(17.0f);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        return true;
    }
    // 0x1502 [event summon monster]
    if (out == "[event summon monster]")
    {
        values.push_back(17.0f);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        return true;
    }
    // 0x150e [summon apc]
    if (out == "[summon apc]")
    {
        values.push_back(18.0f);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        return true;
    }
    // 0x1523 [restore]
    if (out == "[restore]")
    {
        values.push_back(25.0f);
        if (!ScanStr(&token)) return false;
        float mp = (token == "mp") ? 1.0f : 0.0f;
        if (!ScanStr(&token)) return false;
        float pct = (token == "%") ? 1.0f : 0.0f;
        if (!ScanInt(&iv)) return false;
        int v1 = iv;
        if (!ScanInt(&iv)) return false;
        values.push_back(mp);
        values.push_back(pct);
        values.push_back((float)v1);
        values.push_back((float)iv);
        return true;
    }
    // 0x153f [skill]
    if (out == "[skill]")
    {
        values.push_back(19.0f);
        if (!ScanStr(&token)) return false;
        values.push_back((float)getCharacterJobToEnumValue(token));
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        return true;
    }
    // 0x1553 [perform skill]
    if (out == "[perform skill]")
    {
        values.push_back(20.0f);
        if (!ScanStr(&token)) return false;
        values.push_back((float)getCharacterJobToEnumValue(token));
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        return true;
    }
    // 0x156b [end skill]
    if (out == "[end skill]")
    {
        values.push_back(21.0f);
        if (!ScanStr(&token)) return false;
        values.push_back((float)getCharacterJobToEnumValue(token));
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        return true;
    }
    // 0x157a [thorn]
    if (out == "[thorn]")
    {
        values.push_back(22.0f);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        return true;
    }
    // 0x1585 [knockback]
    if (out == "[knockback]")
    {
        int en = 0;
        if (!ScanInt(&en)) return false;
        if (en != 0)
            values.push_back(23.0f);
        return true;
    }
    // 0x158f [reduction]
    if (out == "[reduction]")
    {
        values.push_back(24.0f);
        if (!ScanStr(&token)) return false;
        values.push_back((float)getEquipmentTypeToEnumValue(token));
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        return true;
    }
    // 0x15a0 [elemental weapon]
    if (out == "[elemental weapon]")
    {
        values.push_back(28.0f);
        if (!ScanStr(&token)) return false;
        values.push_back((float)getElementToEnumValue(token));
        return true;
    }
    // 0x15ab [skill levelup]
    if (out == "[skill levelup]")
    {
        if (!GetSkillLevelUp(&values)) return false;
        return true;
    }
    // 0x15b1 [increase damage]
    if (out == "[increase damage]")
    {
        values.push_back(30.0f);
        if (!ScanStr(&token)) return false;
        float pct = (token == "%") ? 1.0f : 0.0f;
        values.push_back(pct);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        return true;
    }
    // 0x15c2 [increase critical damage]
    if (out == "[increase critical damage]")
    {
        values.push_back(31.0f);
        if (!ScanStr(&token)) return false;
        float pct = (token == "%") ? 1.0f : 0.0f;
        values.push_back(pct);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        return true;
    }
    // 0x15d3 [weakness]（action 变体）
    if (out == "[weakness]")
    {
        values.push_back(32.0f);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        return true;
    }
    // 0x15dc [then probability]
    if (out == "[then probability]")
    {
        if (!ScanFloat(&fv)) return false;
        values.push_back(fv);
        return true;
    }
    // 0x15e9 [skill data up] —— 在 info->m_skill1e8 追加一项并 importSkillDataUp
    if (out == "[skill data up]")
    {
        values.push_back(36.0f);
        unsigned sz = info->m_skill1e8.size();
        info->m_skill1e8.resize(sz + 1);
        importSkillDataUp(info->m_skill1e8[sz]);
        values.push_back((float)(int)sz);
        return true;
    }
    // 0x15f5 [speech on] —— 写 info->m_vec1f4
    if (out == "[speech on]")
    {
        values.push_back(37.0f);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        unsigned sz = info->m_vec1f4.size();
        if (!ScanStr(&token)) return false;
        info->m_vec1f4.push_back(token);
        values.push_back((float)(int)sz);
        if (!ScanType(token, true)) return false;
        if (token != "[/speech on]") return false;
        return true;
    }
    // 0x1619 [replace avatar ani] —— 写 info->m_vec468
    if (out == "[replace avatar ani]")
    {
        bool isType = false;
        for (;;)
        {
            if (!ScanTypeOrStr(isType, token, true)) return false;
            if (isType && token == "[/replace avatar ani]") break;
            int idxV = -1;
            if (!ScanInt(&idxV)) return false;
            int et = GetEquipmentTypeStringToEnum(token);
            info->m_vec468.push_back(std::make_pair(et, idxV));
        }
        if (!info->m_vec468.empty())
            values.push_back(44.0f);
        return true;
    }
    // 0x1635 [skill super armor]
    if (out == "[skill super armor]")
    {
        values.push_back(46.0f);
        if (!ScanStr(&token)) return false;
        values.push_back((float)getCharacterJobToEnumValue(token));
        if (!ScanFloat(&fv)) return false;
        values.push_back(fv);
        return true;
    }
    // 0x1642 [skill unbreakable]
    if (out == "[skill unbreakable]")
    {
        values.push_back(47.0f);
        if (!ScanStr(&token)) return false;
        values.push_back((float)getCharacterJobToEnumValue(token));
        if (!ScanFloat(&fv)) return false;
        values.push_back(fv);
        return true;
    }
    // 0x164e [skill cooltime reset]
    if (out == "[skill cooltime reset]")
    {
        values.push_back(48.0f);
        if (!ScanFloat(&fv)) return false;
        values.push_back(fv);
        if (!ScanStr(&token)) return false;
        float pct = (token == "%") ? 1.0f : 0.0f;
        values.push_back(pct);
        if (!ScanFloat(&fv)) return false;
        values.push_back(fv);
        return true;
    }
    // 0x1663 [set hp]
    if (out == "[set hp]")
    {
        values.push_back(49.0f);
        if (!ScanFloat(&fv)) return false;
        values.push_back(fv);
        return true;
    }
    // 0x166d [stat change by stat]
    if (out == "[stat change by stat]")
    {
        int t1 = 0; if (!getTargetTag(&t1)) return false;
        int v1 = -1; if (!ScanInt(&v1)) return false;
        int s1 = 0; if (!getStatusType(&s1)) return false;
        int v2 = 0; if (!ScanInt(&v2)) return false;
        int s2 = 0; if (!getStatusType(&s2)) return false;
        int sg = 0; if (!getSign(&sg)) return false;
        int v3 = 0; if (!ScanInt(&v3)) return false;
        int v4 = 0; if (!ScanInt(&v4)) return false;
        values.push_back(50.0f);
        values.push_back((float)t1);
        values.push_back((float)v1);
        values.push_back((float)s1);
        values.push_back((float)v2);
        values.push_back((float)s2);
        values.push_back((float)sg);
        values.push_back((float)v3);
        values.push_back((float)v4);
        return true;
    }
    // 0x1698 [add stat at once]
    if (out == "[add stat at once]")
    {
        int st = 0; if (!getStatusType(&st)) return false;
        int sg = 0; if (!getSign(&sg)) return false;
        int v = 0; if (!ScanInt(&v)) return false;
        values.push_back(51.0f);
        values.push_back((float)st);
        values.push_back((float)sg);
        values.push_back((float)v);
        return true;
    }
    // 0x16ac [add skill summon]
    if (out == "[add skill summon]")
    {
        values.push_back(53.0f);
        int a = 0; if (!ScanInt(&a)) return false;
        int b = 1; if (!ScanInt(&b)) return false;
        int c = -1; if (!ScanInt(&c)) return false;
        if (!ScanType(token, true)) return false;
        values.push_back((float)a);
        values.push_back((float)b);
        values.push_back((float)c);
        return true;
    }
    // 0x16d3 [swap stat] —— 嵌套 hp/mp/vp 双组交换
    if (out == "[swap stat]")
    {
        if (!ScanStr(&token)) return false;
        int lhs = 3;
        if (token == "hp") lhs = 0;
        else if (token == "mp") lhs = 1;
        else if (token == "vp") lhs = 2;
        int sg1 = 0; if (!getSign(&sg1)) return false;
        int v1 = 0; if (!ScanInt(&v1)) return false;
        if (!ScanStr(&token)) return false;
        int rhs = 3;
        if (token == "hp") rhs = 0;
        else if (token == "mp") rhs = 1;
        else if (token == "vp") rhs = 2;
        if (!ScanFloat(&fv)) return false;
        values.push_back(54.0f);
        values.push_back((float)lhs);
        values.push_back((float)rhs);
        values.push_back((float)sg1);
        values.push_back((float)v1);
        values.push_back(fv);
        return true;
    }

    // 未知关键字：ORIG 未命中 lookup → 返回 0。
    return false;
}

void readCondtion(EquipmentParameterInfo* info, std::string& line,
                  std::vector<int>& values)
{
    // ORIG 0x0899ce3b（0x4401 字节，3665 insn）。结构：对 line 做
    // if/else-if 关键字链（ORIG 另有 c_str→id 的 memo map，纯性能优化，
    // 不影响行为，此处省略）；命中后按该分支的扫描序列向 values push
    // 「tag 常量 + 参数」。以下逐分支按反汇编转写；带 [推断] 者为
    // 多段扫描分支，局部临时复用次序未逐指令核对。
    (void)info;
    std::string token;
    std::string token2;
    int v = 0;
    if (line == "[cooltime]")
    {
        values.push_back(0);
        v = ScanInt((bool*)0);
        values.push_back(v);
    }
    else if (line == "[start cooltime]")
    {
        values.push_back(1);
        v = ScanInt((bool*)0);
        values.push_back(v);
    }
    else if (line == "[character]")
    {
        values.push_back(2);
        ScanStr(&token);
        values.push_back(getCharacterJobToEnumValue(token));
    }
    else if (line == "[casting]")
    {
        if (ScanInt((bool*)0) == 1)
        {
            values.push_back(3);
        }
    }
    else if (line == "[distance]")
    {
        values.push_back(4);
        ScanStr(&token);
        int d = 2;
        if (token == "near")
        {
            d = 0;
        }
        if (token == "far")
        {
            d = 1;
        }
        values.push_back(d);
    }
    else if (line == "[my active status]")
    {
        values.push_back(5);
        ScanStr(&token);
        values.push_back(getActiveStatusToEnumValue(token));
    }
    else if (line == "[my state]")
    {
        values.push_back(6);
        ScanStr(&token);
        values.push_back(getStateToEnumValue(token));
    }
    else if (line == "[my prev state]")
    {
        values.push_back(7);
        ScanStr(&token);
        values.push_back(getStateToEnumValue(token));
    }
    else if (line == "[target prev state]")
    {
        values.push_back(8);
        ScanStr(&token);
        values.push_back(getStateToEnumValue(token));
    }
    else if (line == "[my appendage]")
    {
        v = ScanInt((bool*)0);
        values.push_back(0x28);
        values.push_back(v);
    }
    else if (line == "[target active status]")
    {
        values.push_back(9);
        ScanStr(&token);
        values.push_back(getActiveStatusToEnumValue(token));
    }
    else if (line == "[target state]")
    {
        values.push_back(0xa);
        ScanStr(&token);
        values.push_back(getStateToEnumValue(token));
    }
    else if (line == "[my active status on]")
    {
        values.push_back(0xb);
        ScanStr(&token);
        values.push_back(getActiveStatusToEnumValue(token));
    }
    else if (line == "[target active status on]")
    {
        values.push_back(0xc);
        ScanStr(&token);
        values.push_back(getActiveStatusToEnumValue(token));
    }
    else if (line == "[element]")
    {
        values.push_back(0xd);
        ScanStr(&token);
        values.push_back(getElementToEnumValue(token));
    }
    else if (line == "[attack condition]")
    {
        values.push_back(0xe);
        ScanStr(&token);
        values.push_back(getAttackConditionToEnumValue(token));
    }
    else if (line == "[attack type]")
    {
        // [推断] push tag 后 ScanStr，转换目标未逐指令核对
        values.push_back(0xf);
        ScanStr(&token);
        values.push_back(getCategoryToEnumValue(token));
    }
    else if (line == "[target type]")
    {
        values.push_back(0x10);
        ScanStr(&token);
        values.push_back(getCategoryToEnumValue(token));
    }
    else if (line == "[target grade]")
    {
        values.push_back(0x11);
        ScanStr(&token);
        values.push_back(getGradeToEnumValue(token));
    }
    else if (line == "[target check]")
    {
        values.push_back(0x12);
        bool isType = false;
        ScanTypeOrStr(isType, token, true);
        int t = 1;
        if (token == "monster")
        {
            t = 2;
        }
        if (token == "apc")
        {
            t = 3;
        }
        if (token == "passiveobject")
        {
            t = 1;
        }
        values.push_back(t);
        v = ScanInt((bool*)0);
        values.push_back(v);
    }
    else if (line == "[hit]")
    {
        v = ScanInt((bool*)0);
        values.push_back(0x13);
        values.push_back(v);
    }
    else if (line == "[attack success]")
    {
        v = ScanInt((bool*)0);
        values.push_back(0x14);
        values.push_back(v);
    }
    else if (line == "[after attack]")
    {
        v = ScanInt((bool*)0);
        values.push_back(0x2b);
        values.push_back(v);
    }
    else if (line == "[party count]")
    {
        values.push_back(0x2c);
        ScanStr(&token);
        int op = 0;
        if (token == ">")
        {
            op = 0;
        }
        if (token == "<")
        {
            op = 1;
        }
        if (token == "=")
        {
            op = 2;
        }
        if (token == ">=" || token == "=>")
        {
            op = 3;
        }
        if (token == "<=" || token == "=<")
        {
            op = 4;
        }
        values.push_back(op);
        v = ScanInt((bool*)0);
        values.push_back(v);
    }
    else if (line == "[use skill]")
    {
        values.push_back(0x15);
        ScanStr(&token);
        values.push_back(getCharacterJobToEnumValue(token));
        v = ScanInt((bool*)0);
        values.push_back(v);
    }
    else if (line == "[use command]")
    {
        v = ScanInt((bool*)0);
        values.push_back(0x29);
        values.push_back(v);
    }
    else if (line == "[skill frame trigger]" || line == "[skill frame state]")
    {
        // [推断] 多段扫描：job + int + 字符串（非空则 addString 记录）+ int，
        // 尾部 tag（trigger=0x31 / state=0x32）与 4~5 个参数入栈；
        // 参数复用次序未逐指令核对。
        bool isTrigger = (line == "[skill frame trigger]");
        ScanStr(&token);
        int job = getCharacterJobToEnumValue(token);
        int a = ScanInt((bool*)0);
        ScanStr(&token2);
        if (a != 0)
        {
            info->addString(token2);
        }
        int b = ScanInt((bool*)0);
        values.push_back(isTrigger ? 0x31 : 0x32);
        values.push_back(job);
        values.push_back(a);
        values.push_back(b);
        if (!isTrigger)
        {
            int c = ScanInt((bool*)0);
            values.push_back(c);
        }
    }
    else if (line == "[change status in range]")
    {
        // [推断] push tag 0x30 + ScanStr + 数值扫描
        values.push_back(0x30);
        ScanStr(&token);
        v = ScanInt((bool*)0);
        values.push_back(v);
    }
    else if (line == "[change status]")
    {
        // [推断] tag 0x2f + ScanStr + hp/mp/vp 选择 + 两级数值
        values.push_back(0x2f);
        ScanStr(&token);
        int kind = 0;
        if (token == "hp")
        {
            kind = 0;
        }
        else if (token == "mp")
        {
            kind = 1;
        }
        else if (token == "vp")
        {
            kind = 2;
        }
        values.push_back(kind);
        int a = ScanInt((bool*)0);
        values.push_back(a);
        int b = ScanInt((bool*)0);
        values.push_back(b);
    }
    else if (line == "[stat change]")
    {
        // [推断] tag 0x16 + ScanStr + getChangeStatusType + "end" 终止循环
        values.push_back(0x16);
        for (;;)
        {
            ScanStr(&token);
            if (token == "end")
            {
                break;
            }
            values.push_back(getChangeStatusType(token));
        }
    }
    else if (line == "[target stat]")
    {
        // [推断] tag 0x17 + ScanStr 子键选择 + getChangeStatusType
        values.push_back(0x17);
        ScanStr(&token);
        int sub = 0;
        if (token != "end")
        {
            values.push_back(getChangeStatusType(token));
        }
        values.push_back(sub);
    }
    else if (line == "[check damage]")
    {
        // [推断] tag 0x18 + ScanStr + 数值
        values.push_back(0x18);
        ScanStr(&token);
        v = ScanInt((bool*)0);
        values.push_back(v);
    }
    else if (line == "[set my state]")
    {
        if (ScanInt((bool*)0) == 1)
        {
            values.push_back(0x19);
        }
    }
    else if (line == "[target death]")
    {
        if (ScanInt((bool*)0) == 1)
        {
            values.push_back(0x1a);
        }
    }
    else if (line == "[target cooltime]")
    {
        values.push_back(0x2a);
        v = ScanInt((bool*)0);
        values.push_back(v);
    }
    else if (line == "[party death]")
    {
        if (ScanInt((bool*)0) == 1)
        {
            values.push_back(0x27);
        }
    }
    else if (line == "[revive]")
    {
        if (ScanInt((bool*)0) == 1)
        {
            values.push_back(0x1b);
        }
    }
    else if (line == "[combo]")
    {
        values.push_back(0x1c);
        v = ScanInt((bool*)0);
        values.push_back(v);
    }
    else if (line == "[aerial]")
    {
        if (ScanInt((bool*)0) == 1)
        {
            values.push_back(0x1d);
        }
    }
    else if (line == "[overkill]")
    {
        if (ScanInt((bool*)0) == 1)
        {
            values.push_back(0x1e);
        }
    }
    else if (line == "[disable pvp]")
    {
        if (ScanInt((bool*)0) == 1)
        {
            values.push_back(0x1f);
        }
    }
    else if (line == "[time]")
    {
        // [推断] 三段整数扫描 + tag 0x20，push 次序 EO/ERK 交错未逐条核对
        values.push_back(0x20);
        int t1 = ScanInt((bool*)0);
        values.push_back(t1);
        int t2 = ScanInt((bool*)0);
        values.push_back(t2);
        int t3 = ScanInt((bool*)0);
        values.push_back(t3);
    }
    else if (line == "[skill]")
    {
        // [推断] tag 0x21 + ScanTypeOrStr + job 转换 + int
        values.push_back(0x21);
        bool isType = false;
        ScanTypeOrStr(isType, token, true);
        values.push_back(getCharacterJobToEnumValue(token));
        v = ScanInt((bool*)0);
        values.push_back(v);
    }
    else if (line == "[attacker]")
    {
        values.push_back(0x22);
        ScanStr(&token);
        values.push_back(getObjectTypeToEnumValue(token));
        v = ScanInt((bool*)0);
        values.push_back(v);
    }
    else if (line == "[event use skill]")
    {
        values.push_back(0x23);
        ScanStr(&token);
        values.push_back(getCharacterJobToEnumValue(token));
        v = ScanInt((bool*)0);
        values.push_back(v);
    }
    else if (line == "[event hit]")
    {
        v = ScanInt((bool*)0);
        values.push_back(0x24);
        values.push_back(v);
    }
    else if (line == "[event attack success]")
    {
        v = ScanInt((bool*)0);
        values.push_back(0x25);
        values.push_back(v);
    }
    else if (line == "[module]")
    {
        values.push_back(0x26);
        scanModuleType();
        // [推断] ORIG 将 scanModuleType() 的内部产出 push（经静态区），
        // 具体值由 scanModuleType 维护，此处不再显式 push。
    }
    else if (line == "[cooltime group]")
    {
        v = ScanInt((bool*)0);
        values.push_back(0x2d);
        values.push_back(v);
    }
    else if (line == "[attack success effect animation]")
    {
        // [推断] ORIG 构造 EffectAnimationInfo（W 0x89c2d3a）扫描若干字段后
        // 销毁；tag 0x2e。EffectAnimationInfo 类型尚未在源码树建模，
        // 此处仅消费扫描流并压 tag。
        bool flag = false;
        ScanInt(&flag);
        ScanStr(&token);
        ScanInt((bool*)0);
        values.push_back(0x2e);
    }
    else if (line == "[dungeon check]")
    {
        values.push_back(0x33);
        // [推断] ORIG 调 ScanTypeOrInt(bool&, string&, string&, bool) 双出参
        // 形式；当前 DNFLexWrapperHelpers 为旧签名，链接 ABI 待统一。
        bool isType = false;
        ScanTypeOrStr(isType, token, true);
        v = ScanInt((bool*)0);
        values.push_back(v);
    }
    else if (line == "[skill cooltime]")
    {
        // [推断] tag 0x34 + 固定常量 0x1c/3/0 + 多段扫描
        values.push_back(0x34);
        values.push_back(0x1c);
        values.push_back(3);
        values.push_back(0);
        v = ScanInt((bool*)0);
        values.push_back(v);
        int w = ScanInt((bool*)0);
        values.push_back(w);
        ScanStr(&token);
    }
    else if (line == "[slot option]")
    {
        // [推断] tag 0x35 + ScanStr + getStateToEnumValue + int 对
        values.push_back(0x35);
        ScanStr(&token);
        values.push_back(getStateToEnumValue(token));
        int a = ScanInt((bool*)0);
        values.push_back(a);
        int b = ScanInt((bool*)0);
        values.push_back(b);
    }
    else if (line == "[keep my state]")
    {
        values.push_back(0x36);
    }
    else if (line == "[is stat]")
    {
        // [推断] ORIG 经 static getTargetTag/getStatusType（0x89a7413/0x89a74a2）
        // 解析 target/status 子键；此处按其逻辑内联：
        //   getTargetTag: ScanStr→getTargetToEnumValue，==0xa 视为失败(0)
        //   getStatusType: ScanStr→getChangeStatusType，==0x4d 视为失败(0)
        values.push_back(0);
        int target = 0;
        if (ScanStr(&token))
        {
            target = getTargetToEnumValue(token);
            if (target == 0xa)
            {
                target = 0;
            }
        }
        int tag = 0;
        int statType = 0;
        if (ScanInt(&tag) != 0 && tag != 0)
        {
            values.push_back(tag);
        }
        if (ScanStr(&token2))
        {
            statType = getChangeStatusType(token2);
            if (statType != 0x4d)
            {
                values.push_back(statType);
            }
        }
        values.push_back(target);
        values.push_back(statType);
    }
    else if (line == "[equipment upgrade]")
    {
        // [推断] tag 0x37 + ScanStr（upgrade/separate upgrade/amplify → 0/1/2）
        values.push_back(0x37);
        ScanStr(&token);
        int k = 0;
        if (token == "separate upgrade")
        {
            k = 1;
        }
        else if (token == "amplify")
        {
            k = 2;
        }
        values.push_back(k);
    }
    else if (line == "[remove equip]")
    {
        values.push_back(0x38);
    }
    else if (line == "[is awakening state]")
    {
        values.push_back(0x39);
    }
    else if (line == "[timing]")
    {
        values.push_back(0x3a);
    }
}

bool scanRandomOptionValue(EquipmentParameterInfo* info, std::string& line)
{
    // ORIG 0x89bf67f：info==0 直接返回 true；否则以 NULL dir 调
    // importEquipmentParameterInfo(info, line, 0, a, b) 并透传其返回值。
    if (info == 0)
    {
        return true;
    }
    int a = 0;
    int b = 0;
    return importEquipmentParameterInfo(info, line, 0, a, b);
}

void scanIf(EquipmentParameterInfo* info, int idx)
{
    // ORIG 0x0899cd6c：循环 ScanType(token,true)；token=="[/if]" 终止；
    // 否则 readCondtion(info, token, values)（values 复位后仅作容器）。
    (void)idx;
    std::vector<int> values;
    std::string token;
    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;
        }
        values.clear();
        if (token == "[/if]")
        {
            break;
        }
        readCondtion(info, token, values);
        values.empty();
    }
}


bool scanThen(std::string line, EquipmentParameterInfo* info, int idx)
{
    // ORIG 0x89a123c：循环 ScanType(token,true)；"[/then]" 返回 true；
    // readAction 失败或 EOF 返回 false；values 非空时收集到局部
    // vector<vector<float>>（ORIG 局部，未外传）。
    std::vector<std::vector<float> > collected;
    std::string token;
    for (;;)
    {
        if (!ScanType(token, true))
        {
            return false;
        }
        if (token == "[/then]")
        {
            return true;
        }
        std::vector<float> values;
        if (!readAction(line, info, token, values, idx))
        {
            return false;
        }
        if (!values.empty())
        {
            collected.push_back(values);
        }
    }
}

// ---- SASPROPERTY 子解析 helper（ORIG static，本 TU） ----
static int GetItemGrowtype(EquipmentParameterInfo::SASPROPERTY* sp)
{
    if (sp == 0) return 0;
    std::string s;
    if (!ScanStr(&s)) return 0;
    sp->m_head[0] = getCharacterJob(s.c_str());
    if (!ScanInt(&sp->m_head[1])) return 0;
    return 1;
}

// [skill apply condition] 块直至 "[/skill apply condition]"；子键语义（ORIG
// 惰性 map 分派实证）：
//   [lower bound level]→m_head[3]、[upper bound level]→m_head[4]、
//   [value]→m_head[5]；[skill group] 后 ScanStr 查 passive/active/all →
//   m_head[2]（1/0/-1）；[extra condition] 后 ScanStr 循环查
//   [normal skill]→bit1、[consume item skill]→bit2、[ex skill]→bit4、
//   [except for ex skill]→bit1|bit2 位或入 m_head[6]，flags==0 返回 0。
// 未知标签（惰性 map 已建）返回 0；ScanType/ScanInt/ScanStr 失败返回 0。
static int GetSkillApplyCondition(EquipmentParameterInfo::SASPROPERTY* sp)
{
    if (sp == 0) return 0;
    std::string token;
    for (;;)
    {
        if (!ScanType(token, true)) return 0;
        if (token == "[/skill apply condition]") return 1;
        if (token == "[lower bound level]")
        {
            if (!ScanInt(&sp->m_head[3])) return 0;
        }
        else if (token == "[upper bound level]")
        {
            if (!ScanInt(&sp->m_head[4])) return 0;
        }
        else if (token == "[value]")
        {
            if (!ScanInt(&sp->m_head[5])) return 0;
        }
        else if (token == "[skill group]")
        {
            std::string str;
            if (!ScanStr(&str)) return 0;
            if      (str == "[passive]") sp->m_head[2] = 1;
            else if (str == "[active]")  sp->m_head[2] = 0;
            else if (str == "[all]")     sp->m_head[2] = -1;
            else                         return 0;   // map2 已建未命中
        }
        else if (token == "[extra condition]")
        {
            int flags = 0;
            std::string str;
            while (ScanStr(&str))
            {
                if      (str == "[normal skill]")      flags |= 1;
                else if (str == "[consume item skill]") flags |= 2;
                else if (str == "[ex skill]")           flags |= 4;
                else if (str == "[except for ex skill]") flags |= 3;
                else    return 0;   // map3 已建未命中
            }
            if (flags == 0) return 0;
            sp->m_head[6] = flags;
        }
        else
        {
            return 0;   // map1 已建未命中
        }
    }
}

// GetSkillPhase —— ORIG 0x89be70c（static local）。消费 [skill phase] 块：
// 惰性初始化 static map（"[all]"→0x2f8c … "[fourth]"→0x2f9b），ScanStr 读
// token 查表，命中写 sp->m_head[7] = phase（0..5）并返回 1，未命中返回 0。
static int GetSkillPhase(EquipmentParameterInfo::SASPROPERTY* sp)
{
    if (!sp) return 0;
    static std::map<const char*, int, less_TCHAR> s_phaseMap;
    static SpinLock s_lock;
    static bool s_init = false;
    std::string token;
    for (;;)
    {
        if (!ScanStr(&token)) return 0;
        if (!s_init)
        {
            s_lock.enter();
            if (s_init) { s_lock.leave(); continue; }
            s_phaseMap.insert(std::make_pair("[all]", 0x2f8c));
            s_phaseMap.insert(std::make_pair("[common]", 0x2f8f));
            s_phaseMap.insert(std::make_pair("[first]", 0x2f92));
            s_phaseMap.insert(std::make_pair("[second]", 0x2f95));
            s_phaseMap.insert(std::make_pair("[third]", 0x2f98));
            s_phaseMap.insert(std::make_pair("[fourth]", 0x2f9b));
            s_lock.leave();
            s_init = true;
            continue;   // ORIG 置 init 后重新 ScanStr（89bebf2 jmp 89be755）
        }
        std::map<const char*, int, less_TCHAR>::const_iterator it =
            s_phaseMap.find(token.c_str());
        int id = (it != s_phaseMap.end()) ? it->second : -1;
        switch (id)
        {
        case 0x2f8c: sp->m_head[7] = 5; return 1;
        case 0x2f8f: sp->m_head[7] = 4; return 1;
        case 0x2f92: sp->m_head[7] = 0; return 1;
        case 0x2f95: sp->m_head[7] = 1; return 1;
        case 0x2f98: sp->m_head[7] = 2; return 1;
        case 0x2f9b: sp->m_head[7] = 3; return 1;
        default: return 0;
        }
    }
}

// ORIG：static map<const char*,int> 懒初始化——首次调用丢弃第一个 token（仅
// 触发初始化），其后未知 token 一律返回 0（default 分支）；[skill levelup] 的
// GetSkillLevelUp 直接填充 sp.m_vec20（+0x20），非局部 vector。
static int importAllSkillItem(EquipmentParameterInfo* info)
{
    EquipmentParameterInfo::SASPROPERTY sp;
    sp.m_head[0] = 0xb; sp.m_head[1] = 0; sp.m_head[2] = 0xffffffff;
    sp.m_head[3] = 0;   sp.m_head[4] = 0; sp.m_head[5] = 0;
    sp.m_head[6] = 7;   sp.m_head[7] = 5;
    std::string token;
    bool terminated = false;
    static bool s_first = true;   // ORIG static map 懒初始化：首次调用丢弃首 token
    while (ScanType(token, true))
    {
        if (s_first)
        {
            s_first = false;
            continue;
        }
        if (token == "[/all skill item]")
        {
            terminated = true;
            break;
        }
        if (token == "[item growtype]")
        {
            if (!GetItemGrowtype(&sp)) return 0;
        }
        else if (token == "[skill apply condition]")
        {
            if (!GetSkillApplyCondition(&sp)) return 0;
        }
        else if (token == "[skill phase]")
        {
            if (!GetSkillPhase(&sp)) return 0;
        }
        else if (token == "[skill levelup]")
        {
            if (!GetSkillLevelUp(&sp.m_vec20)) return 0;
        }
        else
        {
            return 0;             // ORIG default：未知 token → 失败
        }
    }
    if (terminated)
    {
        if ((sp.m_head[0] == 0xb && sp.m_head[7] != 5) && sp.m_head[7] != 4) return 0;
        if ((unsigned)sp.m_head[3] < 0x47 && (unsigned)sp.m_head[4] < 0x47 &&
            sp.m_head[3] <= sp.m_head[4])
        {
            if (sp.m_head[5] < 1) return 0;
            info->m_119.m_418.push_back(sp);
            return 1;
        }
        return 0;
    }
    return 0;
}

// ---- importItemOverpowerDesc（ORIG 0x089bf290，static local） ----
// 循环 ScanType 读 token；"[stat]"→局部 vector+GetStat（失败不 push）后 push
// 到 info->m_428；"[/item overpower desc]"→结束。ORIG 结尾语义（与直觉相反，
// 忠实保留）：token 耗尽（未 terminated）返回 0；terminated 且 m_428 为**空**
// 时置 m_424=m_425=1 返回 1；terminated 且 m_428 非空返回 0。未知 token 返回
// 0（default）；首次调用丢弃首 token（static map 懒初始化）。
static int importItemOverpowerDesc(EquipmentParameterInfo* info)
{
    std::string token;
    bool terminated = false;
    static bool s_first = true;
    while (ScanType(token, true))
    {
        if (s_first)
        {
            s_first = false;
            continue;
        }
        if (token == "[/item overpower desc]")
        {
            terminated = true;
            break;
        }
        if (token == "[stat]")
        {
            // ORIG：GetStat（push 12.0f + GetStatValue）后以 eax 残留的
            // GetStatValue 返回值判定，成功才 push 到 m_428。
            std::vector<float> v;
            v.push_back(12.0f);
            if (GetStatValue(&v))
                info->m_119.m_428.push_back(v);
        }
        else
        {
            return 0;
        }
    }
    if (terminated && info->m_119.m_428.empty())
    {
        info->m_119.m_424 = 1;
        info->m_119.m_425 = 1;
        return 1;
    }
    return 0;
}

// ============================================================================
// importEquipmentParameterInfo —— ORIG 0x089a75ea（_Z28importEquipmentParameterInfo...
//   PKcRiS4_，单 token 分派，非文件加载）。param_2(path) 为当前关键字 token；
//   param_3(dir) 供 [then]/[multiple then] 递归 scanThen(dir,...) 用；
//   param_4(a)/param_5(b) 为 [if]/[then] 嵌套深度计数。每个命中分支写对应
//   具名字段并消费 token；结尾对非 [if]/[then] token 调 scanRandomOptionValue
//   （ORIG 经 param_1+0x434 子指针，字段未建模则跳过），返回 1。
//   本版覆盖已具名字段关键字；其余关键字落入结尾（返回 1 不中断脚本）。
// ============================================================================
bool importEquipmentParameterInfo(EquipmentParameterInfo* info,
                                  std::string& path, const char* dir,
                                  int& a, int& b)
{
    int iv;
    float fv;
    std::string s;

    if (path == "[HP MAX]")               { if (ScanInt(&iv)) info->m_hpMax = iv; goto done; }
    if (path == "[HP MAX rate]")          { if (ScanFloat(&fv)) info->m_hpMaxRate = fv; goto done; }
    if (path == "[MP MAX]")               { if (ScanInt(&iv)) info->m_mpMax = iv; goto done; }
    if (path == "[MP MAX rate]")          { if (ScanFloat(&fv)) info->m_mpMaxRate = fv; goto done; }
    if (path == "[physical attack]")      { if (ScanInt(&iv)) info->m_physicalAttack = iv; goto done; }
    if (path == "[physical defense]")     { if (ScanInt(&iv)) info->m_physicalDefense = iv; goto done; }
    if (path == "[magical attack]")       { if (ScanInt(&iv)) info->m_magicalAttack = iv; goto done; }
    if (path == "[magical defense]")      { if (ScanInt(&iv)) info->m_magicalDefense = iv; goto done; }

    if (path == "[equipment physical attack]")
    { if (ScanInt(&iv)) info->getEquipmentPhysicalAttack(0) = iv;
      if (ScanInt(&iv)) info->getEquipmentPhysicalAttack(1) = iv; goto done; }
    if (path == "[equipment physical defense]")
    { if (ScanInt(&iv)) info->getEquipmentPhysicalDefense(0) = iv;
      if (ScanInt(&iv)) info->getEquipmentPhysicalDefense(1) = iv; goto done; }
    if (path == "[equipment magical attack]")
    { if (ScanInt(&iv)) info->getEquipmentMagicalAttack(0) = iv;
      if (ScanInt(&iv)) info->getEquipmentMagicalAttack(1) = iv; goto done; }
    if (path == "[equipment magical defense]")
    { if (ScanInt(&iv)) info->getEquipmentMagicalDefense(0) = iv;
      if (ScanInt(&iv)) info->getEquipmentMagicalDefense(1) = iv; goto done; }

    if (path == "[separate attack]")      { if (ScanInt(&iv)) info->m_388[0] = iv;
                                            if (ScanInt(&iv)) info->m_388[1] = iv; goto done; }

    if (path == "[physical absolute damage]") { if (ScanInt(&iv)) info->m_physicalAbsoluteDamage = iv; goto done; }
    if (path == "[physical damage reduce]")   { if (ScanInt(&iv)) info->m_physicalDamageReduce = iv; goto done; }
    if (path == "[physical absolute defense]"){ if (ScanFloat(&fv)) info->m_physicalAbsoluteDefense = fv; goto done; }
    if (path == "[magical absolute damage]")  { if (ScanInt(&iv)) info->m_magicalAbsoluteDamage = iv; goto done; }
    if (path == "[magical damage reduce]")    { if (ScanInt(&iv)) info->m_magicalDamageReduce = iv; goto done; }
    if (path == "[magical absolute defense]") { if (ScanFloat(&fv)) info->m_magicalAbsoluteDefense = fv; goto done; }

    if (path == "[fire resistance]")    { if (ScanInt(&iv)) info->m_fireResistance = iv; goto done; }
    if (path == "[water resistance]")   { if (ScanInt(&iv)) info->m_waterResistance = iv; goto done; }
    if (path == "[dark resistance]")    { if (ScanInt(&iv)) info->m_darkResistance = iv; goto done; }
    if (path == "[light resistance]")   { if (ScanInt(&iv)) info->m_lightResistance = iv; goto done; }
    if (path == "[all elemental resistance]"){ if (ScanInt(&iv)) info->m_allElementalResistance = iv; goto done; }
    if (path == "[slow resistance]")    { if (ScanInt(&iv)) info->m_slowResistance = iv; goto done; }
    if (path == "[freeze resistance]")  { if (ScanInt(&iv)) info->m_freezeResistance = iv; goto done; }
    if (path == "[poison resistance]")  { if (ScanInt(&iv)) info->m_poisonResistance = iv; goto done; }
    if (path == "[stun resistance]")    { if (ScanInt(&iv)) info->m_stunResistance = iv; goto done; }
    if (path == "[curse resistance]")   { if (ScanInt(&iv)) info->m_curseResistance = iv; goto done; }
    if (path == "[blind resistance]")   { if (ScanInt(&iv)) info->m_blindResistance = iv; goto done; }
    if (path == "[lightning resistance]"){ if (ScanInt(&iv)) info->m_lightningResistance = iv; goto done; }
    if (path == "[stone resistance]")   { if (ScanInt(&iv)) info->m_stoneResistance = iv; goto done; }
    if (path == "[sleep resistance]")   { if (ScanInt(&iv)) info->m_sleepResistance = iv; goto done; }
    if (path == "[burn resistance]")    { if (ScanInt(&iv)) info->m_burnResistance = iv; goto done; }
    if (path == "[weapon break resistance]"){ if (ScanInt(&iv)) info->m_weaponBreakResistance = iv; goto done; }
    if (path == "[bleeding resistance]"){ if (ScanInt(&iv)) info->m_bleedingResistance = iv; goto done; }
    if (path == "[piercing resistance]"){ if (ScanInt(&iv)) info->m_piercingResistance = iv; goto done; }
    if (path == "[stuck resistance]")   { if (ScanInt(&iv)) info->m_stuckResistance = iv; goto done; }
    if (path == "[confuse resistance]") { if (ScanInt(&iv)) info->m_confuseResistance = iv; goto done; }
    if (path == "[hold resistance]")    { if (ScanInt(&iv)) info->m_holdResistance = iv; goto done; }
    if (path == "[armor break resistance]"){ if (ScanInt(&iv)) info->m_armorBreakResistance = iv; goto done; }
    if (path == "[all activestatus resistance]"){ if (ScanInt(&iv)) info->m_allActiveStatusResistance = iv; goto done; }

    if (path == "[inventory limit]")    { if (ScanInt(&iv)) info->m_inventoryLimit = iv; goto done; }
    if (path == "[HP regen speed]")     { if (ScanInt(&iv)) info->m_hpRegenSpeed = iv; goto done; }
    if (path == "[MP regen speed]")     { if (ScanInt(&iv)) info->m_mpRegenSpeed = iv; goto done; }
    if (path == "[MP regen speed rate]"){ if (ScanFloat(&fv)) info->m_mpRegenSpeedRate = fv; goto done; }
    if (path == "[move speed]")         { if (ScanInt(&iv)) info->m_moveSpeed = iv; goto done; }
    if (path == "[x move dash speed]")  { if (ScanFloat(&fv)) info->m_xMoveDashSpeed = fv; goto done; }
    if (path == "[y move dash speed]")  { if (ScanFloat(&fv)) info->m_yMoveDashSpeed = fv; goto done; }
    if (path == "[attack speed]")       { if (ScanInt(&iv)) info->m_attackSpeed = iv; goto done; }
    if (path == "[cast speed]")         { if (ScanInt(&iv)) info->m_castSpeed = iv; goto done; }
    if (path == "[hit recovery]")       { if (ScanInt(&iv)) info->m_hitRecovery = iv; goto done; }
    if (path == "[jump power]")         { if (ScanInt(&iv)) info->m_jumpPower = iv; goto done; }
    if (path == "[physical critical hit]"){ if (ScanFloat(&fv)) info->m_physicalCriticalHit = fv; goto done; }
    if (path == "[magical critical hit]"){ if (ScanFloat(&fv)) info->m_magicalCriticalHit = fv; goto done; }

    if (path == "[elemental property]")
    { if (ScanStr(&s)) info->m_elementalProperty = getElement(s.c_str()); goto done; }

    if (path == "[active status]")
    {
        if (ScanStr(&s)) info->m_activeStatusType = getActiveStatusType(s);
        if (ScanFloat(&fv)) info->m_activeStatusValue = fv;
        if (ScanInt(&iv)) info->m_activeStatusLevel = iv;
        if (ScanInt(&iv)) info->m_activeStatusFlag = (iv != 0);
        if (ScanInt(&iv)) info->m_activeStatusValue2 = iv;
        int cnt = 0;
        if (s == "[blind]") cnt = 2;
        else if (s == "[slow]") cnt = 2;
        else if (s == "[haste]") cnt = 2;
        else if (s == "[stone]") cnt = 1;
        else if (s == "[confuse]") cnt = 1;
        else if (s == "[armor break]") cnt = 1;
        else if (s == "[weapon break]") cnt = 1;
        else if (s == "[poison]") cnt = 1;
        else if (s == "[lightning]") cnt = 1;
        else if (s == "[bleeding]") cnt = 1;
        else if (s == "[burn]") cnt = 4;
        else if (s == "[bless]") cnt = 4;
        else if (s == "[curse]") cnt = 4;
        for (int k = 0; k < cnt; ++k)
            if (ScanInt(&iv)) info->m_vec158.push_back(iv);
        goto done;
    }

    if (path == "[additional effect index]")
    { if (ScanInt(&iv)) info->m_vec194.push_back(iv); goto done; }

    if (path == "[skill data up]")
    { importSkillDataUp(info->m_skill1b8); goto done; }

    if (path == "[attacked active status]")
    {
        if (ScanStr(&s)) info->m_attackedActiveStatusType = getActiveStatusType(s);
        if (ScanFloat(&fv)) info->m_attackedActiveStatusValue = fv;
        if (ScanInt(&iv)) info->m_attackedActiveStatusLevel = iv;
        if (ScanInt(&iv)) info->m_attackedActiveStatusFlag = (iv != 0);
        if (ScanInt(&iv)) info->m_attackedActiveStatusValue2 = iv;
        int cnt = 0;
        if (s == "[blind]") cnt = 2;
        else if (s == "[slow]") cnt = 2;
        else if (s == "[haste]") cnt = 2;
        else if (s == "[burn]") cnt = 4;
        else if (s == "[bless]") cnt = 4;
        else if (s == "[curse]") cnt = 4;
        else cnt = 1;
        for (int k = 0; k < cnt; ++k)
            if (ScanInt(&iv)) info->m_vec17c.push_back(iv);
        goto done;
    }

    if (path == "[add absolute damage]")
    {
        if (ScanStr(&s)) info->m_addAbsoluteDamageType = (s == "[magical]") ? 1 : 0;
        if (ScanFloat(&fv)) info->m_addAbsoluteDamageValue = fv;
        if (ScanInt(&iv)) info->m_addAbsoluteDamageExtra = iv;
        goto done;
    }
    if (path == "[add absolute defense]")
    {
        if (ScanStr(&s)) info->m_addAbsoluteDefenseType = (s == "[magical]") ? 3 : 2;
        if (ScanFloat(&fv)) info->m_addAbsoluteDefenseValue = fv;
        if (ScanInt(&iv)) info->m_addAbsoluteDefenseExtra = iv;
        goto done;
    }
    if (path == "[item aura]")
    {
        ItemAuraInfo aura;
        if (ScanStr(&s)) aura.m_field0 = getChangeStatusType(s);
        if (ScanStr(&s)) aura.m_field0 = (aura.m_field0 == 0) ? 0 : aura.m_field0; // % 标志
        if (ScanFloat(&fv)) info->m_aura188.push_back(aura);
        if (ScanInt(&iv)) {}
        goto done;
    }

    if (path == "[push aside]")        { if (ScanInt(&iv)) info->m_pushAside = iv; goto done; }
    if (path == "[lift up]")           { if (ScanInt(&iv)) info->m_liftUp = iv; goto done; }
    if (path == "[stuck]")             { if (ScanFloat(&fv)) info->m_stuck = fv; goto done; }
    if (path == "[ignore defense]")    { if (ScanInt(&iv)) info->m_ignoreDefense = (iv != 0); goto done; }
    if (path == "[human damage rate]") { if (ScanFloat(&fv)) info->m_humanDamageRate = fv; goto done; }
    if (path == "[fort damage rate]")  { if (ScanFloat(&fv)) info->m_fortDamageRate = fv; goto done; }
    if (path == "[monster damage rate]"){ if (ScanFloat(&fv)) info->m_monsterDamageRate = fv; goto done; }
    if (path == "[HP drain]")          { if (ScanInt(&iv)) info->m_hpDrain = (iv != 0); goto done; }
    if (path == "[MP drain]")          { if (ScanInt(&iv)) info->m_mpDrain = (iv != 0); goto done; }

    if (path == "[room list move speed rate]"){ if (ScanFloat(&fv)) info->m_roomListMoveSpeedRate = fv; goto done; }
    if (path == "[anti evil]")         { if (ScanInt(&iv)) info->m_antiEvil = iv; goto done; }
    if (path == "[rigidity]")          { if (ScanInt(&iv)) info->m_rigidity = iv; goto done; }
    if (path == "[fire attack]")       { if (ScanInt(&iv)) info->m_fireAttack = iv; goto done; }
    if (path == "[water attack]")      { if (ScanInt(&iv)) info->m_waterAttack = iv; goto done; }
    if (path == "[dark attack]")       { if (ScanInt(&iv)) info->m_darkAttack = iv; goto done; }
    if (path == "[light attack]")      { if (ScanInt(&iv)) info->m_lightAttack = iv; goto done; }
    if (path == "[all elemental attack]"){ if (ScanInt(&iv)) info->m_allElementalAttack = iv; goto done; }

    if (path == "[decrease critical damage]")
    {
        if (ScanStr(&s)) { bool pct = (s == "%"); if (ScanInt(&iv)) { if (!pct) info->m_210 = iv; else info->m_decreaseCriticalDamage = (float)iv / 10.0f; } }
        goto done;
    }
    if (path == "[hardcoding parameter]")
    {
        bool ok = false;
        int v;
        while (ScanInt(&v)) { info->m_vec214.push_back(v); ok = true; }
        (void)ok;
        goto done;
    }
    if (path == "[assault cost discount]"){ if (ScanFloat(&fv)) info->m_assaultCostDiscount = fv; goto done; }
    if (path == "[rebirth]")
    {
        std::string t;
        while (ScanType(t, true))
        {
            if (t == "[probability]")      { if (ScanInt(&iv)) info->m_rebirth450.m_field0 = iv; }
            else if (t == "[consume item]"){ if (ScanInt(&iv)) info->m_rebirth450.m_pair4.first = iv;
                                             if (ScanInt(&iv)) info->m_rebirth450.m_pair4.second = iv; }
            else if (t == "[hp recovery]") { if (ScanInt(&iv)) info->m_rebirth450.m_fieldc = iv; }
            else if (t == "[cooltime]")    { if (ScanInt(&iv)) info->m_rebirth450.m_field14 = iv; }
            else if (t == "[mp recovery]") { if (ScanInt(&iv)) info->m_rebirth450.m_field10 = iv; }
            else break;
        }
        goto done;
    }

    if (path == "[all skill item]")
    { if (importAllSkillItem(info) != 1) return false; goto done; }
    if (path == "[variable stat]")
    {
        std::vector<VariableStat> vs;
        VariableStat v;
        while (scanVariableStat(v)) vs.push_back(v);
        info->m_var474 = vs;
        goto done;
    }

    if (path == "[skill levelup]")
    {
        info->m_levelUpSkills[0].clear();
        while (ScanStr(&s))
        {
            LevelUpSkill lu;
            lu.m_job = (s == "[common]") ? 0xb : getCharacterJob(s.c_str());
            lu.m_field4 = ScanInt((bool*)0);
            lu.m_field8 = ScanInt((bool*)0);
            info->m_levelUpSkills[0].push_back(lu);
        }
        goto done;
    }
    if (path == "[avatar select ability]")
    {
        // ORIG 0x089ab451：先清空 m_avatar264，再按 47 项能力名表（0x93f4840）
        // 匹配解析；命中则填 STAvatarAbilityVariation（m_field0=表下标、
        // m_field4='+'→0 / '*'→1 标志、m_field8=float 数值；[SKILL_LEVEL](43)
        // 特殊：job 标签 + 两 int），size<=99 时 push_back；未命中项 push 默认值。
        info->m_avatar264.clear();
        static const char* const kAvatarAbilityNames[] = {
            "[none]", "[HP MAX]", "[MP MAX]", "[PHYSICAL_ATTACK]",
            "[PHYSICAL_DEFENSE]", "[MAGICAL_ATTACK]", "[MAGICAL_DEFENSE]",
            "[EQUIPMENT_PHYSICAL_ATTACK]", "[EQUIPMENT_PHYSICAL_DEFENSE]",
            "[EQUIPMENT_MAGICAL_ATTACK]", "[EQUIPMENT_MAGICAL_DEFENSE]",
            "[ELEMENT_TOLERANCE_FIRE]", "[ELEMENT_TOLERANCE_WATER]",
            "[ELEMENT_TOLERANCE_DARK]", "[ELEMENT_TOLERANCE_LIGHT]",
            "[ELEMENT_TOLERANCE_ALL]", "[ACTIVESTATUS_TOLERANCE_SLOW]",
            "[ACTIVESTATUS_TOLERANCE_FREEZE]", "[ACTIVESTATUS_TOLERANCE_POISON]",
            "[ACTIVESTATUS_TOLERANCE_STUN]", "[ACTIVESTATUS_TOLERANCE_CURSE]",
            "[ACTIVESTATUS_TOLERANCE_BLIND]", "[ACTIVESTATUS_TOLERANCE_LIGHTNING]",
            "[ACTIVESTATUS_TOLERANCE_STONE]", "[ACTIVESTATUS_TOLERANCE_SLEEP]",
            "[ACTIVESTATUS_TOLERANCE_BURN]", "[ACTIVESTATUS_TOLERANCE_WEAPON_BREAK]",
            "[ACTIVESTATUS_TOLERANCE_BLEEDING]", "[ACTIVESTATUS_TOLERANCE_PIERCE]",
            "[ACTIVESTATUS_TOLERANCE_STUCK]", "[ACTIVESTATUS_TOLERANCE_CONFUSE]",
            "[ACTIVESTATUS_TOLERANCE_HOLD]", "[ACTIVESTATUS_TOLERANCE_ALL]",
            "[INVENTORY_MAX_WEIGHT]", "[HP_REGENRATE]", "[MP_REGENRATE]",
            "[MP_REGENRATE_PERCENT_UP]", "[MOVE_SPEED]", "[ATTACK_SPEED]",
            "[CAST_SPEED]", "[HIT_RECOVERY]", "[JUMP_POWER]", "[CRITICAL]",
            "[SKILL_LEVEL]", "[PHYSICAL ABSOLUTE DEFENSE]",
            "[MAGICAL ABSOLUTE DEFENSE]", "[STUCK ON ATTACK]"
        };
        while (ScanStr(&s))
        {
            STAvatarAbilityVariation var;
            int i = 0;
            while (i <= 0x2e)
            {
                if (s == kAvatarAbilityNames[i])
                {
                    var.m_field0 = i;
                    if (i == 0x2b)
                    {
                        var.m_field4 = 0;
                        ScanStr(&s);
                        if (s == "[common]")
                        {
                            var.m_skill.m_job = 0xb;
                        }
                        else
                        {
                            int job = getCharacterJob(s.c_str());
                            if (job == 0xb)
                            {
                                LogManager::logFormat(0, "../RDARScriptItem.cpp",
                                    __FUNCTION__, 0x199e,
                                    "ImportEquipmentScript - file %s, invalid job tag %s",
                                    dir, s.c_str());
                                return false;
                            }
                            var.m_skill.m_job = job;
                        }
                        var.m_skill.m_field4 = ScanInt((bool*)0);
                        var.m_skill.m_field8 = ScanInt((bool*)0);
                    }
                    else
                    {
                        ScanStr(&s);
                        if (s == "+")
                        {
                            var.m_field4 = 0;
                        }
                        else if (s == "*")
                        {
                            var.m_field4 = 1;
                        }
                        var.m_field8 = ScanFloat((bool*)0);
                    }
                    break;
                }
                ++i;
            }
            if (info->m_avatar264.size() <= 0x63)
                info->m_avatar264.push_back(var);
        }
        goto done;
    }

    if (path == "[if]")
    { int i = a; a = a + 1; scanIf(info, i); goto done; }
    if (path == "[then]")
    {
        if (b < a)
        {
            int i = b;
            b = b + 1;
            if (scanThen(dir, info, i)) goto done;
        }
        else
        {
            a = -a;
            b = -b;
        }
        return false;
    }
    if (path == "[multiple then]")
    {
        std::string t;
        bool ok = false;
        while (ScanType(t, true))
        {
            if (t == "[/multiple then]") { b = b + 1; ok = true; break; }
            if (t == "[then]")
            {
                int i = b;
                b = b + 1;
                if (!scanThen(dir, info, i)) { ok = false; break; }
            }
        }
        if (!ok) return false;
        goto done;
    }

done:
    // ORIG 结尾对非 [if]/[then] token 调 scanRandomOptionValue（经 param_1+0x434
    // 子对象指针；该字段未建模，此处以 info 近似，语义见扫描流消费）。
    if (path != "[if]" && path != "[then]")
        scanRandomOptionValue(info, path);
    return true;
}

void exportEquipmentParameterScript(FILE* file, EquipmentParameterInfo* info)
{
    if (!file || !info)
    {
        return;
    }
    // 基础属性（ORIG 0x089acf78：非 0 才输出）
    if (info->m_hpMax != 0)            fprintf(file, "[HP MAX] %+d\n", info->m_hpMax);
    if (info->m_mpMax != 0)            fprintf(file, "[MP MAX] %+d\n", info->m_mpMax);
    if (info->m_physicalAttack != 0)   fprintf(file, "[physical attack] %+d\n", info->m_physicalAttack);
    if (info->m_physicalDefense != 0)  fprintf(file, "[physical defense] %+d\n", info->m_physicalDefense);
    if (info->m_magicalAttack != 0)    fprintf(file, "[magical attack] %+d\n", info->m_magicalAttack);
    if (info->m_magicalDefense != 0)   fprintf(file, "[magical defense] %+d\n", info->m_magicalDefense);

    // 装备能力数组（经访问器，非 0 才输出）
    if (info->getEquipmentPhysicalAttack(0) != 0 || info->getEquipmentPhysicalAttack(1) != 0)
        fprintf(file, "[equipment physical attack] %+d %+d\n",
                info->getEquipmentPhysicalAttack(0), info->getEquipmentPhysicalAttack(1));
    if (info->getEquipmentPhysicalDefense(0) != 0 || info->getEquipmentPhysicalDefense(1) != 0)
        fprintf(file, "[equipment physical defense] %+d %+d\n",
                info->getEquipmentPhysicalDefense(0), info->getEquipmentPhysicalDefense(1));
    if (info->getEquipmentMagicalAttack(0) != 0 || info->getEquipmentMagicalAttack(1) != 0)
        fprintf(file, "[equipment magical attack] %+d %+d\n",
                info->getEquipmentMagicalAttack(0), info->getEquipmentMagicalAttack(1));
    if (info->getEquipmentMagicalDefense(0) != 0 || info->getEquipmentMagicalDefense(1) != 0)
        fprintf(file, "[equipment magical defense] %+d %+d\n",
                info->getEquipmentMagicalDefense(0), info->getEquipmentMagicalDefense(1));

    if (info->m_388[0] != 0 || info->m_388[1] != 0)
        fprintf(file, "[separate attack] %+d %+d\n", info->m_388[0], info->m_388[1]);

    if (info->m_physicalAbsoluteDamage != 0)   fprintf(file, "[physical absolute damage] %+d\n", info->m_physicalAbsoluteDamage);
    if (info->m_physicalDamageReduce != 0)     fprintf(file, "[physical damage reduce] %+d\n", info->m_physicalDamageReduce);
    if (info->m_physicalAbsoluteDefense != 0.0f) fprintf(file, "[physical absolute defense] %+.2f\n", (double)info->m_physicalAbsoluteDefense);
    if (info->m_magicalAbsoluteDamage != 0)    fprintf(file, "[magical absolute damage] %+d\n", info->m_magicalAbsoluteDamage);
    if (info->m_magicalDamageReduce != 0)      fprintf(file, "[magical damage reduce] %+d\n", info->m_magicalDamageReduce);
    if (info->m_magicalAbsoluteDefense != 0.0f) fprintf(file, "[magical absolute defense] %+.2f\n", (double)info->m_magicalAbsoluteDefense);

    // 抗性
    if (info->m_fireResistance != 0)        fprintf(file, "[fire resistance] %+d\n", info->m_fireResistance);
    if (info->m_waterResistance != 0)       fprintf(file, "[water resistance] %+d\n", info->m_waterResistance);
    if (info->m_darkResistance != 0)        fprintf(file, "[dark resistance] %+d\n", info->m_darkResistance);
    if (info->m_lightResistance != 0)       fprintf(file, "[light resistance] %+d\n", info->m_lightResistance);
    if (info->m_allElementalResistance != 0) fprintf(file, "[all elemental resistance] %+d\n", info->m_allElementalResistance);
    if (info->m_slowResistance != 0)        fprintf(file, "[slow resistance] %+d\n", info->m_slowResistance);
    if (info->m_freezeResistance != 0)      fprintf(file, "[freeze resistance] %+d\n", info->m_freezeResistance);
    if (info->m_poisonResistance != 0)      fprintf(file, "[poison resistance] %+d\n", info->m_poisonResistance);
    if (info->m_stunResistance != 0)        fprintf(file, "[stun resistance] %+d\n", info->m_stunResistance);
    if (info->m_curseResistance != 0)       fprintf(file, "[curse resistance] %+d\n", info->m_curseResistance);
    if (info->m_blindResistance != 0)       fprintf(file, "[blind resistance] %+d\n", info->m_blindResistance);
    if (info->m_lightningResistance != 0)   fprintf(file, "[lightning resistance] %+d\n", info->m_lightningResistance);
    if (info->m_stoneResistance != 0)       fprintf(file, "[stone resistance] %+d\n", info->m_stoneResistance);
    if (info->m_sleepResistance != 0)       fprintf(file, "[sleep resistance] %+d\n", info->m_sleepResistance);
    if (info->m_burnResistance != 0)        fprintf(file, "[burn resistance] %+d\n", info->m_burnResistance);
    if (info->m_weaponBreakResistance != 0) fprintf(file, "[weapon break resistance] %+d\n", info->m_weaponBreakResistance);
    if (info->m_bleedingResistance != 0)    fprintf(file, "[bleeding resistance] %+d\n", info->m_bleedingResistance);
    if (info->m_piercingResistance != 0)    fprintf(file, "[piercing resistance] %+d\n", info->m_piercingResistance);
    if (info->m_stuckResistance != 0)       fprintf(file, "[stuck resistance] %+d\n", info->m_stuckResistance);
    if (info->m_confuseResistance != 0)     fprintf(file, "[confuse resistance] %+d\n", info->m_confuseResistance);
    if (info->m_holdResistance != 0)        fprintf(file, "[hold resistance] %+d\n", info->m_holdResistance);
    if (info->m_armorBreakResistance != 0)  fprintf(file, "[armor break resistance] %+d\n", info->m_armorBreakResistance);
    if (info->m_allActiveStatusResistance != 0) fprintf(file, "[all activestatus resistance] %+d\n", info->m_allActiveStatusResistance);
    if (info->m_inventoryLimit != 0)        fprintf(file, "[inventory limit] %+d\n", info->m_inventoryLimit);
    if (info->m_hpRegenSpeed != 0)          fprintf(file, "[HP regen speed] %+d\n", info->m_hpRegenSpeed);
    if (info->m_mpRegenSpeed != 0)          fprintf(file, "[MP regen speed] %+d\n", info->m_mpRegenSpeed);
    if (info->m_mpRegenSpeedRate != 0.0f)   fprintf(file, "[MP regen speed rate] %+.2f\n", (double)info->m_mpRegenSpeedRate);
    if (info->m_moveSpeed != 0)             fprintf(file, "[move speed] %+d\n", info->m_moveSpeed);
    if (info->m_attackSpeed != 0)           fprintf(file, "[attack speed] %+d\n", info->m_attackSpeed);
    if (info->m_castSpeed != 0)             fprintf(file, "[cast speed] %+d\n", info->m_castSpeed);
    if (info->m_hitRecovery != 0)           fprintf(file, "[hit recovery] %+d\n", info->m_hitRecovery);
    if (info->m_jumpPower != 0)             fprintf(file, "[jump power] %+d\n", info->m_jumpPower);
    if (info->m_physicalCriticalHit != 0.0f) fprintf(file, "[physical critical hit] %+.2f\n", (double)info->m_physicalCriticalHit);
    if (info->m_magicalCriticalHit != 0.0f) fprintf(file, "[magical critical hit] %+.2f\n", (double)info->m_magicalCriticalHit);

    if (info->m_elementalProperty != 4)
        fprintf(file, "[elemental property] `%s`\n",
                getElementName(static_cast<ENUM_ELEMENT>(info->m_elementalProperty)));

    // active status（ORIG：0x144 类型名 + 值/等级）
    if (info->m_activeStatusType != 0x12)
    {
        static const char* const kActiveStatusName[] = {
            "slow", "freeze", "poison", "stun", "curse", "blind", "lightning",
            "stone", "sleep", "burn", "weapon break", "armor break", "bleeding",
            "haste", "bless", "element", "", "", "none"
        };
        const char* name = "none";
        int t = info->m_activeStatusType;
        if (t >= 0 && t <= 0x11) name = kActiveStatusName[t];
        fprintf(file, "[active status] `[%s]` %.2f %d\n",
                name, (double)info->m_activeStatusValue, info->m_activeStatusLevel);
    }

    // skill levelup
    if (!info->m_levelUpSkills[0].empty())
    {
        fprintf(file, "[skill levelup]\n");
        for (std::vector<LevelUpSkill>::iterator it = info->m_levelUpSkills[0].begin();
             it != info->m_levelUpSkills[0].end(); ++it)
        {
            if (it->m_job == 0xb)
                fwrite("[common]", 1, 8, file);
            else
                fprintf(file, "`%d`", it->m_job);
            fprintf(file, " %d %d\n", it->m_field4, it->m_field8);
        }
        fwrite("[/skill levelup]\n", 1, 0x11, file);
    }
}

// ============================================================================
// importSkillDataUp —— ORIG 0x088c049c（_Z17importSkillDataUpRSt6vectorI20EquipmentSkillDataUpSaIS0_EE）。
// 从当前词法流逐条解析 equipment skill 数据。每条记录（ORIG 按偏移写入）：
//   job(+0x00)=getCharacterJob(名字)、value(+0x04)=ScanInt、module(+0x18)=scanModuleType、
//   下一 token 为标签；按标签（[static]/[level]/[cooltime]/[mp]/[maintain mp]/
//   [skill consume item]/[casting time]）分派设置 key(+0x14)=0..6 并消费一个 ScanInt 到
//   level(+0x08)；再读下一 token 置 percent(+0x0c)=（token!="+"），ScanInt 到 value(+0x10)。
//   ORIG 的 __lookup 静态 map + __inited 惰性初始化仅为编译期优化（每条首遍注册标签），
//   行为等价于按标签字符串 if/else（同本文件 readAction 注释约定）。对象在函数顶部构造
//   一次、逐条覆写复用；未知标签不消费 level token，key/level 保持上一轮/ctor 默认。
// ============================================================================
void importSkillDataUp(std::vector<EquipmentSkillDataUp>& out)
{
    out.reserve(0x14);
    EquipmentSkillDataUp e;   // ORIG 在函数顶部构造一次，逐条覆写复用
    std::string line;
    while (ScanStr(&line))
    {
        e.m_job = getCharacterJob(line.c_str());
        e.m_v4 = ScanInt((bool*)0);
        e.m_module = scanModuleType();
        ScanStr(&line);   // 标签 token

        if (line == "[static]")
        {
            e.m_key = 0; e.m_v8 = ScanInt((bool*)0);
        }
        else if (line == "[level]")
        {
            e.m_key = 1; e.m_v8 = ScanInt((bool*)0);
        }
        else if (line == "[cooltime]")
        {
            e.m_key = 2; ScanInt((bool*)0); e.m_v8 = 0;   // ORIG 丢弃 ScanInt 结果
        }
        else if (line == "[mp]")
        {
            e.m_key = 3; ScanInt((bool*)0); e.m_v8 = 0;   // ORIG 丢弃 ScanInt 结果
        }
        else if (line == "[maintain mp]")
        {
            e.m_key = 4; ScanInt((bool*)0); e.m_v8 = 0;   // ORIG 丢弃 ScanInt 结果
        }
        else if (line == "[skill consume item]")
        {
            e.m_key = 5; e.m_v8 = ScanInt((bool*)0);
        }
        else if (line == "[casting time]")
        {
            e.m_key = 6; ScanInt((bool*)0); e.m_v8 = 0;   // ORIG 丢弃 ScanInt 结果
        }
        // 未知标签：不消费 level token，key/level 保持默认

        ScanStr(&line);
        e.m_percent = (line == "+") ? 0 : 1;
        e.m_v10 = ScanInt((bool*)0);
        out.push_back(e);
    }
}
