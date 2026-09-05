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
#include <limits>   // ORIG export @0x89adeab 尾区：numeric_limits<float>::epsilon() 调用两处

#include "DNFLexWrapper.h"
#include "LogManager.h"
#include "ScriptValue.h"
#include "SyncScriptDeps.h"   // toTString / getCharacterJob
#include "CreatureScript.h"  // ENUM_ELEMENT（getElementName 形参）
#include "Quest.h"           // ENUM_CHARACTERJOB 的权威声明
#include "CEnvironment.h"    // less_TCHAR / SpinLock（GetSkillPhase static map）
#include "EffectAnimationInfo.h"  // readCondtion [attack success effect animation] 局部对象

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
// 所有已解析的数组/标量均通过 EquipmentParameterInfo 具名字段访问；重复清零
// 保留 ORIG 的最终状态，字段类型与偏移以 EquipmentParameterInfo.h 为准。
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
    m_growthLevelBegin = 0;
    m_growthLevelEnd = 0;
    for (int i = 0; i <= 1; ++i)
    {
        m_linPhyAtk[i] = 0;
        m_linPhyDef[i] = 0;
        m_linMagAtk[i] = 0;
        m_linMagDef[i] = 0;
    }
    m_50 = 0;
    for (int i = 0; i <= 1; ++i)
    {
        m_phyAtt[i] = 0;
        m_phyDef[i] = 0;
        m_magAtt[i] = 0;
        m_magDef[i] = 0;
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
    m_fireResistance = 0;
    m_waterResistance = 0;
    m_darkResistance = 0;
    m_lightResistance = 0;
    m_allElementalResistance = 0;
    m_slowResistance = 0;
    m_freezeResistance = 0;
    m_poisonResistance = 0;
    m_stunResistance = 0;
    m_curseResistance = 0;
    m_blindResistance = 0;
    m_lightningResistance = 0;
    m_stoneResistance = 0;
    m_sleepResistance = 0;
    m_burnResistance = 0;
    m_weaponBreakResistance = 0;
    m_bleedingResistance = 0;
    m_piercingResistance = 0;
    m_stuckResistance = 0;
    m_confuseResistance = 0;
    m_holdResistance = 0;
    m_armorBreakResistance = 0;
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
    m_humanDamageRate = 0.0f;
    m_fortDamageRate = 0.0f;
    m_monsterDamageRate = 0.0f;
    m_hpDrain = 0;
    m_mpDrain = 0;
    for (int i = 0; i <= 1; ++i)
        m_levelUpSkills[i].clear();
    m_avatar264.clear();
    m_270 = -1;
    m_roomListMoveSpeedRate = 0;
    m_str278 = "";
    m_str27c = "";
    m_antiEvil = 0;
    m_rigidity = 0;
    m_fireAttack = 0;
    m_waterAttack = 0;
    m_darkAttack = 0;
    m_lightAttack = 0;
    for (int i = 0; i <= 17; ++i)
    {
        m_activeStatusProbability[i] = 0;
        m_activeStatusLevelTable[i] = 0;
        m_activeStatusCtrl[i] = 0;
    }
    for (int i = 0; i <= 4; ++i)
        m_activeStatusControlInfo[i] = 0;
    m_separateAttack[0] = 0;
    m_separateAttack[1] = 0;
    m_separateRate[0] = 1.0f;
    m_separateRate[1] = 1.0f;
    m_398 = 0;
    m_39c = 0;
    m_414 = 0;
    m_growthGradeBegin = 0;
    m_growthGradeEnd = 0;
    for (int i = 0; i <= 3; ++i)
    {
        m_linEqPhyAtk[i] = 0;
        m_linEqPhyDef[i] = 0;
        m_linEqMagAtk[i] = 0;
        m_linEqMagDef[i] = 0;
        m_linSepAtk[i] = 0;
        m_linSepRate[i] = 1.0f;
    }
    m_growthRepairBegin = 0;
    m_growthRepairEnd = 0;
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
    m_upgradeProbIncrease = 0;
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
            it++;
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
int getEquipmentEventTiming(const std::string& s);     // 0x89c2a81（OnlyOrigBatch11.cpp，"jump attack"→0 否则 1）
// ORIG 0x088be60：职业枚举 → 作业标签名（STMapScript.cpp 全局 T）。
const char* getCharacterJobName(ENUM_CHARACTERJOB job);
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
{    // [N类豁免] 残余 DIFF 归因（范式库 §7 跨版本组）：本函数指令多重集已与 ORIG
    // 对齐（逐 insert !__inited 守卫 / switch 决策树 cmp$imm,%eax+je/jg / push
    // RKi-RKf 物化位点核对，报告 ORIG 地址见各 case 注释）；剩余差异为
    // gcc4.1.2 BB 布局（init 链与 dispatch body 交插块序、leaf 体排布）与栈槽
    // 编号（ORIG 帧 0x79c/0xacc vs 4.4.7 分配序不同），源码不可控，标 N 类。

    // ORIG 0x89a13e5：主分派以函数局部静态 map<char const*,int,less_TCHAR>
    // （报告 0x89a13e5 实证：一组 __cxa_guard/SpinLock/__inited 双检 + 64 处
    // pair<char const(&)[N],int>::insert）查 out.c_str() 得分支 id；未命中 id==-1
    // 落到函数尾返回。原重建用 std::string operator== 线性比较，此处转写为查表。
    // 注：ORIG 另有 3 张子表（__lookup_0/1/2，[active status]/[stat] 内层 token），
    // 对应内层比较仍保留字符串比较，见交付说明。
    int id;
    {
        static std::map<const char*, int, less_TCHAR> __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;
        // ORIG 0x899d3a2..+0x573 等：惰性注册链每 tag 一个 `if (!__inited)`
        // 守卫块（movzbl __inited; xor $1; test; je <body>），insert 链尾统一
        // leave + 置位。逐条守卫保持 ORIG 检查形态（报告 0x899ce3b 汇编）。
        if (!__inited)
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
            }
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[consume item]", 0x1248));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[target]", 0x1257));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[duration]", 0x126a));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[equipment duration]", 0x127b));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[skill duration]", 0x128c));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[reduce duration at pvp type module]", 0x12a0));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[reduce duration at pvp module]", 0x12ac));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[reduce duration to human armor]", 0x12b8));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[reduce duration to human armor at pvp type module]", 0x12c4));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[reduce duration to human armor at pvp module]", 0x12d0));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[delay]", 0x12dc));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[probability]", 0x12e7));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[reduce probability at pvp type module]", 0x12f2));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[reduce probability at pvp module]", 0x12fe));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[reduce probability to human armor]", 0x130a));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[reduce probability to human armor at pvp type module]", 0x1316));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[reduce probability to human armor at pvp module]", 0x1322));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[active status]", 0x132e));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[disenchant]", 0x136b));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[buff]", 0x1374));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[appendage]", 0x137e));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[appendage unique]", 0x138a));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[aura]", 0x1396));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[add absolute damage]", 0x13a1));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[add absolute defense]", 0x13c8));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[add absolute defense percent]", 0x13e6));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[aura active]", 0x13fe));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[apply status in range]", 0x1419));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[stat by condition]", 0x141e));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[stat level replace]", 0x1423));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[stat]", 0x1432));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[active status resistance]", 0x1435));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[element resistance]", 0x144a));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[damage rate]", 0x1461));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[active status duration]", 0x146e));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[active status buff event type]", 0x1478));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[passive object]", 0x14af));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[summon monster]", 0x14ec));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[event summon monster]", 0x1502));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[summon apc]", 0x150e));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[restore]", 0x1523));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[skill]", 0x153f));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[perform skill]", 0x1553));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[end skill]", 0x156b));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[thorn]", 0x157a));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[knockback]", 0x1585));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[reduction]", 0x158f));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[elemental weapon]", 0x15a0));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[skill levelup]", 0x15ab));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[increase damage]", 0x15b1));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[increase critical damage]", 0x15c2));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[weakness]", 0x15d3));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[then probability]", 0x15dc));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[skill data up]", 0x15e9));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[speech on]", 0x15f5));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[replace avatar ani]", 0x1619));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[skill super armor]", 0x1635));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[skill unbreakable]", 0x1642));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[skill cooltime reset]", 0x164e));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[set hp]", 0x1663));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[stat change by stat]", 0x166d));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[add stat at once]", 0x1698));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[add skill summon]", 0x16ac));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[swap stat]", 0x16d3));
            if (!__inited)
            {
                __init_lock.leave();
                __inited = true;
            }
        }
        std::map<const char*, int, less_TCHAR>::const_iterator it =
            __lookup.find(out.c_str());
        id = (it != __lookup.end()) ? it->second : -1;
    }
    (void)idx;
    std::string token;
    std::string token2;
    int iv;
    float fv;

    // 0x1248 [consume item]
    switch (id)
    {
    case 0x1248:
    {
        values.push_back(0.0f);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        return true;
    }
    // 0x1257 [target]
    case 0x1257:
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
    case 0x126a:
    {
        values.push_back(2.0f);
        if (!scanScriptValue(values)) return false;
        return true;
    }
    // 0x127b [equipment duration]
    case 0x127b:
    {
        values.push_back(3.0f);
        if (!scanScriptValue(values)) return false;
        return true;
    }
    // 0x128c [skill duration]
    case 0x128c:
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
    case 0x12a0:
    {
        values.push_back(26.0f);
        values.push_back(1.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        return true;
    }
    // 0x12ac [reduce duration at pvp module]
    case 0x12ac:
    {
        values.push_back(26.0f);
        values.push_back(2.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        return true;
    }
    // 0x12b8 [reduce duration to human armor]
    case 0x12b8:
    {
        values.push_back(26.0f);
        values.push_back(3.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        return true;
    }
    // 0x12c4 [reduce duration to human armor at pvp type module]
    case 0x12c4:
    {
        values.push_back(26.0f);
        values.push_back(4.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        return true;
    }
    // 0x12d0 [reduce duration to human armor at pvp module]
    case 0x12d0:
    {
        values.push_back(26.0f);
        values.push_back(4.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        return true;
    }
    // 0x12dc [delay]
    case 0x12dc:
    {
        values.push_back(4.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        return true;
    }
    // 0x12e7 [probability]
    case 0x12e7:
    {
        values.push_back(5.0f);
        if (!ScanFloat(&fv)) return false;
        values.push_back(fv);
        return true;
    }
    // 0x12f2 [reduce probability at pvp type module]
    case 0x12f2:
    {
        values.push_back(27.0f);
        values.push_back(1.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        return true;
    }
    // 0x12fe [reduce probability at pvp module]
    case 0x12fe:
    {
        values.push_back(27.0f);
        values.push_back(2.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        return true;
    }
    // 0x130a [reduce probability to human armor]
    case 0x130a:
    {
        values.push_back(27.0f);
        values.push_back(0.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        return true;
    }
    // 0x1316 [reduce probability to human armor at pvp type module]
    case 0x1316:
    {
        values.push_back(27.0f);
        values.push_back(3.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        return true;
    }
    // 0x1322 [reduce probability to human armor at pvp module]
    case 0x1322:
    {
        values.push_back(27.0f);
        values.push_back(4.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        return true;
    }
    // 0x132e [active status] —— 先压 tag/状态/数值，再扫描附加状态子键（armor break…curse）
    case 0x132e:
    {
        values.push_back(6.0f);
        if (!ScanStr(&token)) return false;
        fv = (float)getActiveStatusToEnumValue(token);
        values.push_back(fv);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        // 嵌套子键循环：ScanTypeOrInt 得 token，查表分派附加状态参数
        bool isType = false;
        bool isInt = false;
        for (;;)
        {
            if (ScanTypeOrInt(isType, token, isInt, true) == 0 && !isType && !isInt)
                return false;
            if (!isType) break;
            // ORIG 0x89a13e5 __lookup_0：附加状态名子表（14 项，less_TCHAR 查表）。
            int sid;
            {
                static std::map<const char*, int, less_TCHAR> __lookup;
                static SpinLock __init_lock;
                static bool __inited = false;
                if (!__inited)
                {
                    __init_lock.enter();
                    if (__inited)
                    {
                        __init_lock.leave();
                    }
                        if (!__inited)
                        __lookup.insert(std::pair<const char* const, int>("blind", 0x1344));
                        if (!__inited)
                        __lookup.insert(std::pair<const char* const, int>("slow", 0x1345));
                        if (!__inited)
                        __lookup.insert(std::pair<const char* const, int>("haste", 0x1346));
                        if (!__inited)
                        __lookup.insert(std::pair<const char* const, int>("stone", 0x134f));
                        if (!__inited)
                        __lookup.insert(std::pair<const char* const, int>("confuse", 0x1350));
                        if (!__inited)
                        __lookup.insert(std::pair<const char* const, int>("weakness", 0x1351));
                        if (!__inited)
                        __lookup.insert(std::pair<const char* const, int>("armor break", 0x1352));
                        if (!__inited)
                        __lookup.insert(std::pair<const char* const, int>("weapon break", 0x1353));
                        if (!__inited)
                        __lookup.insert(std::pair<const char* const, int>("poison", 0x1354));
                        if (!__inited)
                        __lookup.insert(std::pair<const char* const, int>("lightning", 0x1355));
                        if (!__inited)
                        __lookup.insert(std::pair<const char* const, int>("bleeding", 0x1356));
                        if (!__inited)
                        __lookup.insert(std::pair<const char* const, int>("burn", 0x135c));
                        if (!__inited)
                        __lookup.insert(std::pair<const char* const, int>("bless", 0x135d));
                        if (!__inited)
                        __lookup.insert(std::pair<const char* const, int>("curse", 0x135e));
                    if (!__inited)
                    {
                        __init_lock.leave();
                        __inited = true;
                    }
                }
                std::map<const char*, int, less_TCHAR>::const_iterator it =
                    __lookup.find(token.c_str());
                sid = (it != __lookup.end()) ? it->second : -1;
            }
            if (sid == -1) break;
            {
                // 附加状态浮点参数计数（ORIG 0x1344..0x135e 子键分派链）：
                // blind/slow/haste 2 个；burn/bless/curse 4 个；其余 1 个。
                int cnt;
                if (sid == 0x1344 || sid == 0x1345 || sid == 0x1346)
                    cnt = 2;
                else if (sid == 0x135c || sid == 0x135d || sid == 0x135e)
                    cnt = 4;
                else
                    cnt = 1;
                for (int k = 0; k < cnt; ++k)
                {
                    if (!ScanFloat(&fv)) return false;
                    values.push_back(fv);
                }
            }
        }
        return true;
    }
    // 0x136b [disenchant]
    case 0x136b:
    {
        if (!ScanInt(&iv)) return false;
        if (iv == 1)
            values.push_back(7.0f);
        return true;
    }
    // 0x1374 [buff]
    case 0x1374:
    {
        values.push_back(8.0f);
        if (!ScanStr(&token)) return false;
        fv = (float)getBuffToEnumValue(token);
        values.push_back(fv);
        return true;
    }
    // 0x137e [appendage]
    case 0x137e:
    {
        values.push_back(33.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        return true;
    }
    // 0x138a [appendage unique]
    case 0x138a:
    {
        values.push_back(34.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        return true;
    }
    // 0x1396 [aura]
    case 0x1396:
    {
        values.push_back(35.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        return true;
    }
    // 0x13a1 [add absolute damage]
    case 0x13a1:
    {
        values.push_back(9.0f);
        if (!ScanStr(&token)) return false;
        if (!ScanStr(&token)) return false;
        float pct = (token == "%") ? 1.0f : 0.0f;
        if (!ScanInt(&iv)) return false;
        values.push_back(pct);
        fv = (float)iv;
        values.push_back(fv);
        return true;
    }
    // 0x13c8 [add absolute defense]
    case 0x13c8:
    {
        values.push_back(10.0f);
        if (!ScanStr(&token)) return false;
        if (!ScanStr(&token)) return false;
        float pct = (token == "%") ? 1.0f : 0.0f;
        if (!ScanInt(&iv)) return false;
        values.push_back(pct);
        fv = (float)iv;
        values.push_back(fv);
        return true;
    }
    // 0x13e6 [add absolute defense percent]
    case 0x13e6:
    {
        values.push_back(43.0f);
        if (!ScanStr(&token)) return false;
        if (!ScanFloat(&fv)) return false;
        values.push_back(fv);
        return true;
    }
    // 0x13fe [aura active]
    case 0x13fe:
    {
        values.push_back(11.0f);
        if (!ScanStr(&token)) return false;
        fv = (float)getChangeStatusType(token);
        values.push_back(fv);
        if (!ScanStr(&token)) return false;
        if (!ScanFloat(&fv)) return false;
        if (!ScanInt(&iv)) return false;
        float pct = (token == "%") ? 1.0f : 0.0f;
        values.push_back(pct);
        values.push_back(fv);
        fv = (float)iv;
        values.push_back(fv);
        return true;
    }
    // 0x1419 [apply status in range]
    case 0x1419:
    {
        GetApplyStatusInRange(&values);
        return true;
    }
    // 0x141e [stat by condition]
    case 0x141e:
    {
        GetStatByCondition(&values);
        return true;
    }
    // 0x1423 [stat level replace] —— ORIG 0x899a3dd3：push 40.0 后两次 ScanStr
    // 与一次 ScanInt 均无成功检查（失败时值取 0/默认继续，返回 true）。
    case 0x1423:
    {
        values.push_back(40.0f);
        ScanStr(&token);
        fv = (float)getChangeStatusType(token);
        values.push_back(fv);
        ScanStr(&token);
        float pct = (token == "%") ? 1.0f : 0.0f;
        values.push_back(pct);
        fv = (float)ScanInt((bool*)0);
        values.push_back(fv);
        return true;
    }
    // 0x1432 [stat]
    case 0x1432:
    {
        GetStat(&values);
        return true;
    }
    // 0x1435 [active status resistance]
    case 0x1435:
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
        fv = (float)v;
        (void)iv;  // ORIG 0x89a34xx leaf [1461]：ScanInt 结果不 push（Ghidra 丢弃）
        return true;
    }
    // 0x144a [element resistance]
    case 0x144a:
    {
        values.push_back(14.0f);
        if (!ScanStr(&token)) return false;
        fv = (float)getElementToEnumValue(token);
        values.push_back(fv);
        if (!ScanStr(&token)) return false;
        float pct = (token == "%") ? 1.0f : 0.0f;
        if (!ScanInt(&iv)) return false;
        values.push_back(pct);
        fv = (float)iv;
        values.push_back(fv);
        return true;
    }
    // 0x1461 [damage rate]
    case 0x1461:
    {
        values.push_back(15.0f);
        if (!ScanStr(&token)) return false;
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        return true;
    }
    // 0x146e [active status duration]
    case 0x146e:
    {
        values.push_back(41.0f);
        if (!ScanFloat(&fv)) return false;
        values.push_back(fv);
        return true;
    }
    // 0x1478 [active status buff event type]
    case 0x1478:
    {
        values.push_back(42.0f);
        if (!ScanFloat(&fv)) return false;
        values.push_back(fv);
        return true;
    }
    // 0x14af [passive object] —— ORIG 0x899a44eb：push 16.0 + 组数占位后循环；
    // 每组以 ScanTypeOrInt 取首值，随后 5 个 ScanInt 与 1 个 ScanStr（字符串仅
    // 校验成功不 push），push 6 个数值并递增组数；任一步扫描失败返回 false。
    // ScanTypeOrInt 读到 type 且 token=="[/passive object]" 时写回组数占位结束。
    case 0x14af:
    {
        std::string tok;   // ORIG 0x89a4569：body 局部串槽 -0x984
        values.push_back(16.0f);
        std::string tok2;  // ORIG 0x89a4765：body 局部串槽 -0x9a0
        int cnt = 0;
        values.push_back(0.0f);
        bool isType = false;
        bool isInt = false;
        for (;;)
        {
            int r = ScanTypeOrInt(isType, tok, isInt, true);
            if (isType && tok == "[/passive object]")
            {
                if (values.size() > 1)
                    values.at(1) = (float)cnt;   // ORIG 0x89a485b：vector<float>::at（越界抛异常路径）
                return true;
            }
            fv = (float)r;
            values.push_back(fv);
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
            fv = (float)b;
            values.push_back(fv);
            fv = (float)c;
            values.push_back(fv);
            fv = (float)d;
            values.push_back(fv);
            fv = (float)e;
            values.push_back(fv);
            fv = (float)f;
            values.push_back(fv);
            cnt = cnt + 1;
        }
    }
    // 0x14ec [summon monster]
    case 0x14ec:
    {
        values.push_back(17.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        return true;
    }
    // 0x1502 [event summon monster] —— ORIG 0x899a4a0b：4×ScanInt 均无成功检查
    //（ScanInt((bool*)0)），失败亦返回 true。
    case 0x1502:
    {
        values.push_back(17.0f);
        fv = (float)ScanInt((bool*)0);
        values.push_back(fv);
        fv = (float)ScanInt((bool*)0);
        values.push_back(fv);
        fv = (float)ScanInt((bool*)0);
        values.push_back(fv);
        fv = (float)ScanInt((bool*)0);
        values.push_back(fv);
        return true;
    }
    // 0x150e [summon apc]
    case 0x150e:
    {
        values.push_back(18.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        return true;
    }
    // 0x1523 [restore]
    case 0x1523:
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
        fv = (float)v1;
        values.push_back(fv);
        fv = (float)iv;
        values.push_back(fv);
        return true;
    }
    // 0x153f [skill]
    case 0x153f:
    {
        values.push_back(19.0f);
        if (!ScanStr(&token)) return false;
        fv = (float)getCharacterJobToEnumValue(token);
        values.push_back(fv);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        return true;
    }
    // 0x1553 [perform skill]
    case 0x1553:
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
    case 0x156b:
    {
        values.push_back(21.0f);
        if (!ScanStr(&token)) return false;
        fv = (float)getCharacterJobToEnumValue(token);
        values.push_back(fv);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        return true;
    }
    // 0x157a [thorn]
    case 0x157a:
    {
        values.push_back(22.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        return true;
    }
    // 0x1585 [knockback]
    case 0x1585:
    {
        int en = 0;
        if (!ScanInt(&en)) return false;
        if (en != 0)
            values.push_back(23.0f);
        return true;
    }
    // 0x158f [reduction]
    case 0x158f:
    {
        values.push_back(24.0f);
        if (!ScanStr(&token)) return false;
        fv = (float)getEquipmentTypeToEnumValue(token);
        values.push_back(fv);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        return true;
    }
    // 0x15a0 [elemental weapon]
    case 0x15a0:
    {
        values.push_back(28.0f);
        if (!ScanStr(&token)) return false;
        values.push_back((float)getElementToEnumValue(token));
        return true;
    }
    // 0x15ab [skill levelup]
    case 0x15ab:
    {
        if (!GetSkillLevelUp(&values)) return false;
        return true;
    }
    // 0x15b1 [increase damage]
    case 0x15b1:
    {
        values.push_back(30.0f);
        if (!ScanStr(&token)) return false;
        float pct = (token == "%") ? 1.0f : 0.0f;
        values.push_back(pct);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        return true;
    }
    // 0x15c2 [increase critical damage]
    case 0x15c2:
    {
        values.push_back(31.0f);
        if (!ScanStr(&token)) return false;
        float pct = (token == "%") ? 1.0f : 0.0f;
        values.push_back(pct);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        return true;
    }
    // 0x15d3 [weakness]（action 变体）—— ORIG 0x899a5928：ScanInt 无成功检查
    //（ScanInt((bool*)0) 值直接入栈，解析失败亦返回 true）。
    case 0x15d3:
    {
        values.push_back(32.0f);
        fv = (float)ScanInt((bool*)0);
        values.push_back(fv);
        return true;
    }
    // 0x15dc [then probability] —— ORIG 0x899a59dd：仅 ScanFloat 消费一个数值
    // 校验成功即返回（不向 values push 任何内容）。
    case 0x15dc:
    {
        if (!ScanFloat(&fv)) return false;
        values.push_back(fv);   // ORIG 0x89a4a?? leaf 0x15dc：SF+Rkf push
        return true;
    }
    // 0x15e9 [skill data up] —— 在 info->m_skill1e8 追加一项并 importSkillDataUp
    case 0x15e9:
    {
        values.push_back(36.0f);
        unsigned sz = info->m_skill1e8.size();
        info->m_skill1e8.resize(sz + 1);
        importSkillDataUp(info->m_skill1e8[sz]);
        fv = (float)(int)sz;
        values.push_back(fv);
        return true;
    }
    // 0x15f5 [speech on] —— 写 info->m_vec1f4
    case 0x15f5:
    {
        std::string t;     // ORIG 0x89a5d28：body 局部串槽 -0xa30
        values.push_back(37.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        unsigned sz = info->m_vec1f4.size();
        if (!ScanStr(&token)) return false;
        info->m_vec1f4.push_back(token);
        values.push_back((float)(int)sz);
        if (!ScanType(t, true)) return false;
        if (t != "[/speech on]") return false;
        return true;
    }
    // 0x1619 [replace avatar ani] —— 写 info->m_vec468
    case 0x1619:
    {
        bool isType = false;
        for (;;)
        {
            if (!ScanTypeOrStr(isType, token, true)) return false;
            if (isType && token == "[/replace avatar ani]") break;
            int idxV = -1;
            if (!ScanInt(&idxV)) return false;
            // ORIG 0x89a5ee4：std::make_pair(ENUM_EQUIPMENTTYPE&, int&) —— 两实参
            // 均为命名左值（P61/P81），首个槽为枚举类型（-0xa3c enum）。
            ENUM_EQUIPMENTTYPE et = (ENUM_EQUIPMENTTYPE)GetEquipmentTypeStringToEnum(token);
            info->m_vec468.push_back(std::make_pair(et, idxV));
        }
        if (!info->m_vec468.empty())
            values.push_back(44.0f);
        return true;
    }
    // 0x1635 [skill super armor]
    case 0x1635:
    {
        values.push_back(46.0f);
        if (!ScanStr(&token)) return false;
        values.push_back((float)getCharacterJobToEnumValue(token));
        if (!ScanFloat(&fv)) return false;
        values.push_back(fv);
        return true;
    }
    // 0x1642 [skill unbreakable]
    case 0x1642:
    {
        values.push_back(47.0f);
        if (!ScanStr(&token)) return false;
        values.push_back((float)getCharacterJobToEnumValue(token));
        if (!ScanFloat(&fv)) return false;
        values.push_back(fv);
        return true;
    }
    // 0x164e [skill cooltime reset]
    case 0x164e:
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
    case 0x1663:
    {
        values.push_back(49.0f);
        if (!ScanFloat(&fv)) return false;
        values.push_back(fv);
        return true;
    }
    // 0x166d [stat change by stat]
    case 0x166d:
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
    case 0x1698:
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
    case 0x16ac:
    {
        std::string t;     // ORIG 0x89a68c2：body 局部串槽 -0xa80
        values.push_back(53.0f);
        int a = 0; if (!ScanInt(&a)) return false;
        int b = 1; if (!ScanInt(&b)) return false;
        int c = -1; if (!ScanInt(&c)) return false;
        if (!ScanType(t, true)) return false;
        values.push_back((float)a);
        values.push_back((float)b);
        values.push_back((float)c);
        return true;
    }
    // 0x16d3 [swap stat] —— 嵌套 hp/mp/vp 双组交换
    case 0x16d3:
    {
        std::string s;     // ORIG 0x89a690e：body 局部串槽 -0xa90
        if (!ScanStr(&s)) return false;
        // ORIG 0x89a13e5 __lookup_1：lhs 侧 hp/mp/vp 子表（未命中默认 3）。
        int lid;
        {
            static std::map<const char*, int, less_TCHAR> __lookup;
            static SpinLock __init_lock;
            static bool __inited = false;
            if (!__inited)
            {
                __init_lock.enter();
                if (__inited)
                {
                    __init_lock.leave();
                }
                    if (!__inited)
                    __lookup.insert(std::pair<const char* const, int>("hp", 0x16da));
                    if (!__inited)
                    __lookup.insert(std::pair<const char* const, int>("mp", 0x16df));
                    if (!__inited)
                    __lookup.insert(std::pair<const char* const, int>("vp", 0x16e4));
                if (!__inited)
                {
                    __init_lock.leave();
                    __inited = true;
                }
            }
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(token.c_str());
            lid = (it != __lookup.end()) ? it->second : -1;
        }
        int lhs = 3;
        if (lid == 0x16da) lhs = 0;
        else if (lid == 0x16df) lhs = 1;
        else if (lid == 0x16e4) lhs = 2;
        int sg1 = 0; if (!getSign(&sg1)) return false;
        int v1 = 0; if (!ScanInt(&v1)) return false;
        if (!ScanStr(&token)) return false;
        // ORIG 0x89a13e5 __lookup_2：rhs 侧 hp/mp/vp 子表（未命中默认 3）。
        int rid;
        {
            static std::map<const char*, int, less_TCHAR> __lookup;
            static SpinLock __init_lock;
            static bool __inited = false;
            if (!__inited)
            {
                __init_lock.enter();
                if (__inited)
                {
                    __init_lock.leave();
                }
                    if (!__inited)
                    __lookup.insert(std::pair<const char* const, int>("hp", 0x16fa));
                    if (!__inited)
                    __lookup.insert(std::pair<const char* const, int>("mp", 0x16ff));
                    if (!__inited)
                    __lookup.insert(std::pair<const char* const, int>("vp", 0x1704));
                if (!__inited)
                {
                    __init_lock.leave();
                    __inited = true;
                }
            }
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(token.c_str());
            rid = (it != __lookup.end()) ? it->second : -1;
        }
        int rhs = 3;
        if (rid == 0x16fa) rhs = 0;
        else if (rid == 0x16ff) rhs = 1;
        else if (rid == 0x1704) rhs = 2;
        if (!ScanFloat(&fv)) return false;
        values.push_back(54.0f);
        values.push_back((float)lhs);
        values.push_back((float)rhs);
        values.push_back((float)sg1);
        values.push_back((float)v1);
        values.push_back(fv);
        return true;
    }
    default:
        break;
    }

    // 未知关键字：ORIG 未命中 lookup → 返回 0。
    return false;
}

// ORIG 0x0899ce3b（0x4401 字节）。真实结构：static map<const char*,int> 惰性
// 注册各关键字 → 分支 id，另以 __init_lock/__inited 双检锁保护首次注册；
// 注册完成后按分支 id 逐条 if 分派。注册/加锁仅为编译期优化，行为等价于
// 按关键字字符串 if/else-if 链（与本文件 readAction/importSkillDataUp 注释
// 约定一致），此处省略 map/锁直接分派。每个命中分支按 ORIG 反编译顺序
// Scan* 消费词法流并向 values push「tag 常量 + 参数」；失败的分支按其
// ORIG 语义处理（多数分支不检查扫描结果；[is stat]/[equipment upgrade]
// 任一步失败即直接 return，不再 push）。
void readCondtion(EquipmentParameterInfo* info, std::string& line,
                  std::vector<int>& values)
{    // [N类豁免] 残余 DIFF 归因（范式库 §7 跨版本组）：本函数指令多重集已与 ORIG
    // 对齐（逐 insert !__inited 守卫 / switch 决策树 cmp$imm,%eax+je/jg / push
    // RKi-RKf 物化位点核对，报告 ORIG 地址见各 case 注释）；剩余差异为
    // gcc4.1.2 BB 布局（init 链与 dispatch body 交插块序、leaf 体排布）与栈槽
    // 编号（ORIG 帧 0x79c/0xacc vs 4.4.7 分配序不同），源码不可控，标 N 类。

    // ORIG 0x899ce3b：函数局部静态 map<char const*,int,less_TCHAR>（报告
    // 0x899ce3b 实证：一组 __cxa_guard/SpinLock/__inited 双检 + 60 处
    // pair<char const(&)[N],int>::insert）。以 line.c_str() 查表得分支 id，
    // 未命中（id==-1）不产生任何 push。原重建用 std::string operator== 线性
    // 比较，与 ORIG 表查找语义不同，此处转写为查表分派。
    int id;
    {
        static std::map<const char*, int, less_TCHAR> __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;
        // ORIG 0x899d3a2..+0x573 等：惰性注册链每 tag 一个 `if (!__inited)`
        // 守卫块（movzbl __inited; xor $1; test; je <body>），insert 链尾统一
        // leave + 置位。逐条守卫保持 ORIG 检查形态（报告 0x899ce3b 汇编）。
        if (!__inited)
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
            }
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[cooltime]", 0xf22));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[start cooltime]", 0xf28));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[character]", 0xf2e));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[casting]", 0xf34));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[distance]", 0xf39));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[my active status]", 0xf45));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[my state]", 0xf4b));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[my prev state]", 0xf52));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[target prev state]", 0xf58));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[my appendage]", 0xf5f));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[target active status]", 0xf64));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[target state]", 0xf6a));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[my active status on]", 0xf70));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[target active status on]", 0xf76));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[element]", 0xf7c));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[attack condition]", 0xf82));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[attack type]", 0xf88));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[target type]", 0xf92));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[target grade]", 0xf98));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[target check]", 0xf9f));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[hit]", 0xfb6));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[attack success]", 0xfbb));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[after attack]", 0xfc0));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[party count]", 0xfc5));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[use skill]", 0xfd7));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[use command]", 0xfdf));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[skill frame trigger]", 0xfe4));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[skill frame state]", 0xff3));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[change status in range]", 0x1005));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[change status]", 0x1016));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[stat change]", 0x103c));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[target stat]", 0x105a));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[check damage]", 0x1076));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[set my state]", 0x107d));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[target death]", 0x1082));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[target cooltime]", 0x1087));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[party death]", 0x108c));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[revive]", 0x1091));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[combo]", 0x1096));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[aerial]", 0x109b));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[overkill]", 0x10a0));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[disable pvp]", 0x10a5));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[time]", 0x10aa));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[skill]", 0x10bc));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[attacker]", 0x10cc));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[event use skill]", 0x10d4));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[event hit]", 0x10dc));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[event attack success]", 0x10e1));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[module]", 0x10e6));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[cooltime group]", 0x10f1));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[attack success effect animation]", 0x10fb));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[dungeon check]", 0x110c));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[skill cooltime]", 0x112c));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[slot option]", 0x113e));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[keep my state]", 0x116b));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[is stat]", 0x1176));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[equipment upgrade]", 0x11a1));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[remove equip]", 0x11dc));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[is awakening state]", 0x11df));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[timing]", 0x11e4));
            if (!__inited)
            {
                __init_lock.leave();
                __inited = true;
            }
        }
        std::map<const char*, int, less_TCHAR>::const_iterator it =
            __lookup.find(line.c_str());
        id = (it != __lookup.end()) ? it->second : -1;
    }
    std::string token;
    std::string token2;
    int v = 0;
    int k = 0;
    bool isType = false;
    bool isInt = false;
    switch (id)
    {
    case 0xf22:
    {
        values.push_back(0);
        values.push_back(ScanInt((bool*)0));
        break;
    }
    case 0xf28:
    {
        values.push_back(1);
        values.push_back(ScanInt((bool*)0));
        break;
    }
    case 0xf2e:
    {
        values.push_back(2);
        ScanStr(&token);
        values.push_back(getCharacterJobToEnumValue(token));
        break;
    }
    case 0xf34:
    {
        if (ScanInt((bool*)0) == 1)
        {
            values.push_back(3);
        }
        break;
    }
    case 0xf39:
    {
        values.push_back(4);
        ScanStr(&token);
        k = 2;
        if (token == "near")
        {
            k = 0;
        }
        else if (token == "far")
        {
            k = 1;
        }
        values.push_back(k);
        break;
    }
    case 0xf45:
    {
        values.push_back(5);
        ScanStr(&token);
        values.push_back(getActiveStatusToEnumValue(token));
        break;
    }
    case 0xf4b:
    {
        values.push_back(6);
        ScanStr(&token);
        values.push_back(getStateToEnumValue(token));
        break;
    }
    case 0xf52:
    {
        values.push_back(7);
        ScanStr(&token);
        values.push_back(getStateToEnumValue(token));
        break;
    }
    case 0xf58:
    {
        values.push_back(8);
        ScanStr(&token);
        values.push_back(getStateToEnumValue(token));
        break;
    }
    case 0xf5f:
    {
        v = ScanInt((bool*)0);
        values.push_back(0x28);
        values.push_back(v);
        break;
    }
    case 0xf64:
    {
        values.push_back(9);
        ScanStr(&token);
        values.push_back(getActiveStatusToEnumValue(token));
        break;
    }
    case 0xf6a:
    {
        values.push_back(0xa);
        ScanStr(&token);
        values.push_back(getStateToEnumValue(token));
        break;
    }
    case 0xf70:
    {
        values.push_back(0xb);
        ScanStr(&token);
        values.push_back(getActiveStatusToEnumValue(token));
        break;
    }
    case 0xf76:
    {
        values.push_back(0xc);
        ScanStr(&token);
        values.push_back(getActiveStatusToEnumValue(token));
        break;
    }
    case 0xf7c:
    {
        values.push_back(0xd);
        ScanStr(&token);
        values.push_back(getElementToEnumValue(token));
        break;
    }
    case 0xf82:
    {
        values.push_back(0xe);
        ScanStr(&token);
        values.push_back(getAttackConditionToEnumValue(token));
        break;
    }
    case 0xf88:
    {
        // ORIG 0x899df17：push tag 0xf 后仅 ScanStr 消费一个 token（结果丢弃）。
        values.push_back(0xf);
        ScanStr(&token);
        break;
    }
    case 0xf92:
    {
        values.push_back(0x10);
        ScanStr(&token);
        values.push_back(getCategoryToEnumValue(token));
        break;
    }
    case 0xf98:
    {
        values.push_back(0x11);
        ScanStr(&token);
        values.push_back(getGradeToEnumValue(token));
        break;
    }
    case 0xf9f:
    {
        // ORIG 0x899e116：push 0x12 后 while 循环 ScanTypeOrStr：每次读到
        // 非 type token（monster→2/apc→3/passiveobject→1/其它→4）push 该值
        // 并 push 一个 ScanInt；读到 type（下一标签）即退出循环。
        values.push_back(0x12);
        isType = false;
        while (!isType)
        {
            ScanTypeOrStr(isType, token, true);
            if (!isType)
            {
                k = 4;
                if (token == "monster")
                {
                    k = 2;
                }
                else if (token == "apc")
                {
                    k = 3;
                }
                else if (token == "passiveobject")
                {
                    k = 1;
                }
                values.push_back(k);
                values.push_back(ScanInt((bool*)0));
            }
        }
        break;
    }
    case 0xfb6:
    {
        v = ScanInt((bool*)0);
        values.push_back(0x13);
        values.push_back(v);
        break;
    }
    case 0xfbb:
    {
        v = ScanInt((bool*)0);
        values.push_back(0x14);
        values.push_back(v);
        break;
    }
    case 0xfc0:
    {
        v = ScanInt((bool*)0);
        values.push_back(0x2b);
        values.push_back(v);
        break;
    }
    case 0xfc5:
    {
        // ORIG 0x899e498：push 0x2c；ScanStr 比较符 >/< /=/>=/=>/<=/=<
        // → 0/1/2/3/4（未命中保持 0），再 push 一个 ScanInt。
        values.push_back(0x2c);
        ScanStr(&token);
        k = 0;
        if (token == ">")
        {
            k = 0;
        }
        else if (token == "<")
        {
            k = 1;
        }
        else if (token == "=")
        {
            k = 2;
        }
        else if (token == ">=" || token == "=>")
        {
            k = 3;
        }
        else if (token == "<=" || token == "=<")
        {
            k = 4;
        }
        values.push_back(k);
        values.push_back(ScanInt((bool*)0));
        break;
    }
    case 0xfd7:
    {
        values.push_back(0x15);
        ScanStr(&token);
        values.push_back(getCharacterJobToEnumValue(token));
        values.push_back(ScanInt((bool*)0));
        break;
    }
    case 0xfdf:
    {
        v = ScanInt((bool*)0);
        values.push_back(0x29);
        values.push_back(v);
        break;
    }
    case 0xfe4:
    {
        // ORIG 0x899e809（tag 0x31）：ScanStr(job)→enum；ScanInt；ScanStr；
        // info 非空时该串 addString 得 key（否则 -1）；ScanInt；
        // push 0x31, job, int, key, int。
        ScanStr(&token);
        v = getCharacterJobToEnumValue(token);
        k = ScanInt((bool*)0);
        ScanStr(&token2);
        int key = -1;
        if (info != 0)
        {
            key = info->addString(token2);
        }
        int last = ScanInt((bool*)0);
        values.push_back(0x31);
        values.push_back(v);
        values.push_back(k);
        values.push_back(key);
        values.push_back(last);
        break;
    }
    case 0xff3:
    {
        // ORIG 0x899e9a3（tag 0x32）：同 trigger，末尾多一个 ScanInt；
        // push 0x32, job, int, key, int, int。
        ScanStr(&token);
        v = getCharacterJobToEnumValue(token);
        k = ScanInt((bool*)0);
        ScanStr(&token2);
        int key = -1;
        if (info != 0)
        {
            key = info->addString(token2);
        }
        int last = ScanInt((bool*)0);
        int extra = ScanInt((bool*)0);
        values.push_back(0x32);
        values.push_back(v);
        values.push_back(k);
        values.push_back(key);
        values.push_back(last);
        values.push_back(extra);
        break;
    }
    case 0x1005:
    {
        // ORIG 0x899eb64（tag 0x30）：ScanStr hp/mp/vp→0/1/2 push，再 push
        // 两个 ScanInt。
        values.push_back(0x30);
        ScanStr(&token);
        k = 0;
        if (token == "hp")
        {
            k = 0;
        }
        else if (token == "mp")
        {
            k = 1;
        }
        else if (token == "vp")
        {
            k = 2;
        }
        values.push_back(k);
        values.push_back(ScanInt((bool*)0));
        values.push_back(ScanInt((bool*)0));
        break;
    }
    case 0x1016:
    {
        // ORIG 0x899ecd3（tag 0x2f）do-while 循环：每轮 ScanStr 状态名
        // hp/mp/vp→0/1/2、"end"→跳出、其它→getChangeStatusType；随后
        // ScanStr 比较符→0..4、ScanStr "%"→bool、ScanInt——全部 push。
        values.push_back(0x2f);
        for (;;)
        {
            ScanStr(&token);
            k = 0;
            if (token == "hp")
            {
                k = 0;
            }
            else if (token == "mp")
            {
                k = 1;
            }
            else if (token == "vp")
            {
                k = 2;
            }
            else if (token == "end")
            {
                break;
            }
            else
            {
                k = getChangeStatusType(token);
            }
            values.push_back(k);
            ScanStr(&token);
            k = 0;
            if (token == ">")
            {
                k = 0;
            }
            else if (token == "<")
            {
                k = 1;
            }
            else if (token == "=")
            {
                k = 2;
            }
            else if (token == ">=" || token == "=>")
            {
                k = 3;
            }
            else if (token == "<=" || token == "=<")
            {
                k = 4;
            }
            values.push_back(k);
            ScanStr(&token);
            values.push_back(token == "%" ? 1 : 0);
            values.push_back(ScanInt((bool*)0));
        }
        break;
    }
    case 0x103c:
    {
        // ORIG 0x899efdf（tag 0x16）：一次 ScanStr hp/mp/vp→0/1/2 + 比较符
        // + "%"bool + ScanInt（无循环）。
        values.push_back(0x16);
        ScanStr(&token);
        k = 0;
        if (token == "hp")
        {
            k = 0;
        }
        else if (token == "mp")
        {
            k = 1;
        }
        else if (token == "vp")
        {
            k = 2;
        }
        values.push_back(k);
        ScanStr(&token);
        k = 0;
        if (token == ">")
        {
            k = 0;
        }
        else if (token == "<")
        {
            k = 1;
        }
        else if (token == "=")
        {
            k = 2;
        }
        else if (token == ">=" || token == "=>")
        {
            k = 3;
        }
        else if (token == "<=" || token == "=<")
        {
            k = 4;
        }
        values.push_back(k);
        ScanStr(&token);
        values.push_back(token == "%" ? 1 : 0);
        values.push_back(ScanInt((bool*)0));
        break;
    }
    case 0x105a:
    {
        // ORIG 0x899f2b2（tag 0x17）：仅 hp/mp→0/1 + 比较符 + "%"bool + ScanInt。
        values.push_back(0x17);
        ScanStr(&token);
        k = 0;
        if (token == "hp")
        {
            k = 0;
        }
        else if (token == "mp")
        {
            k = 1;
        }
        values.push_back(k);
        ScanStr(&token);
        k = 0;
        if (token == ">")
        {
            k = 0;
        }
        else if (token == "<")
        {
            k = 1;
        }
        else if (token == "=")
        {
            k = 2;
        }
        else if (token == ">=" || token == "=>")
        {
            k = 3;
        }
        else if (token == "<=" || token == "=<")
        {
            k = 4;
        }
        values.push_back(k);
        ScanStr(&token);
        values.push_back(token == "%" ? 1 : 0);
        values.push_back(ScanInt((bool*)0));
        break;
    }
    case 0x1076:
    {
        // ORIG 0x899f55f（tag 0x18）：ScanStr "%"→bool push + ScanInt push。
        values.push_back(0x18);
        ScanStr(&token);
        values.push_back(token == "%" ? 1 : 0);
        values.push_back(ScanInt((bool*)0));
        break;
    }
    case 0x107d:
    {
        if (ScanInt((bool*)0) == 1)
        {
            values.push_back(0x19);
        }
        break;
    }
    case 0x1082:
    {
        if (ScanInt((bool*)0) == 1)
        {
            values.push_back(0x1a);
        }
        break;
    }
    case 0x1087:
    {
        values.push_back(0x2a);
        values.push_back(ScanInt((bool*)0));
        break;
    }
    case 0x108c:
    {
        if (ScanInt((bool*)0) == 1)
        {
            values.push_back(0x27);
        }
        break;
    }
    case 0x1091:
    {
        if (ScanInt((bool*)0) == 1)
        {
            values.push_back(0x1b);
        }
        break;
    }
    case 0x1096:
    {
        values.push_back(0x1c);
        values.push_back(ScanInt((bool*)0));
        break;
    }
    case 0x109b:
    {
        if (ScanInt((bool*)0) == 1)
        {
            values.push_back(0x1d);
        }
        break;
    }
    case 0x10a0:
    {
        if (ScanInt((bool*)0) == 1)
        {
            values.push_back(0x1e);
        }
        break;
    }
    case 0x10a5:
    {
        if (ScanInt((bool*)0) == 1)
        {
            values.push_back(0x1f);
        }
        break;
    }
    case 0x10aa:
    {
        // ORIG 0x899fbdf：三次 ScanInt，第二个仅保留「!=0」bool：
        // push 0x20, int1, (int2!=0), int3。
        int t1 = ScanInt((bool*)0);
        int t2 = ScanInt((bool*)0);
        int t3 = ScanInt((bool*)0);
        values.push_back(0x20);
        values.push_back(t1);
        values.push_back(t2 != 0 ? 1 : 0);
        values.push_back(t3);
        break;
    }
    case 0x10bc:
    {
        // ORIG 0x899fce4（tag 0x21）：while ScanTypeOrStr 循环：每个非 type
        // token 经 getCharacterJobToEnumValue push，随后 push 一个 ScanInt；
        // 读到 type（下一标签）退出。
        values.push_back(0x21);
        isType = false;
        while (!isType)
        {
            ScanTypeOrStr(isType, token, true);
            if (!isType)
            {
                values.push_back(getCharacterJobToEnumValue(token));
                values.push_back(ScanInt((bool*)0));
            }
        }
        break;
    }
    case 0x10cc:
    {
        values.push_back(0x22);
        ScanStr(&token);
        values.push_back(getObjectTypeToEnumValue(token));
        values.push_back(ScanInt((bool*)0));
        break;
    }
    case 0x10d4:
    {
        values.push_back(0x23);
        ScanStr(&token);
        values.push_back(getCharacterJobToEnumValue(token));
        values.push_back(ScanInt((bool*)0));
        break;
    }
    case 0x10dc:
    {
        v = ScanInt((bool*)0);
        values.push_back(0x24);
        values.push_back(v);
        break;
    }
    case 0x10e1:
    {
        v = ScanInt((bool*)0);
        values.push_back(0x25);
        values.push_back(v);
        break;
    }
    case 0x10e6:
    {
        // ORIG 0x899a011f：push 0x26 后 while (scanModuleType() != 0x11)
        // 循环 push 每个返回值；0x11 为模块流结束标记。
        values.push_back(0x26);
        while ((v = scanModuleType()) != 0x11)
        {
            values.push_back(v);
        }
        break;
    }
    case 0x10f1:
    {
        v = ScanInt((bool*)0);
        values.push_back(0x2d);
        values.push_back(v);
        break;
    }
    case 0x10fb:
    {
        // ORIG 0x899a0276：构造一个临时 EffectAnimationInfo（EAI ctor 1 + dtor
        // 2：正常出口 + ScanStr 抛出时的清理路径），随后 count = ScanInt；循环
        // count 次各消费 ScanStr + ScanInt + ScanInt 写入该对象后随作用域销毁，
        // 不 push 任何值、无外部副作用。EAI 字段私有且结果丢弃，故此处仅构造/
        // 析构该对象以复现 ORIG 的 EAI ctor/dtor 与扫描调用序列（词法消费一致）。
        EffectAnimationInfo eai;
        (void)eai;
        int count = ScanInt((bool*)0);
        for (int i = 0; i < count; ++i)
        {
            ScanStr(&token);
            ScanInt((bool*)0);
            ScanInt((bool*)0);
        }
        break;
    }
    case 0x110c:
    {
        // ORIG 0x899a0372（tag 0x2e）：push 0x2e 后 while 循环
        // ScanTypeOrInt：读到非 type token 时 push 其整数值；
        // 读到 type（下一标签）退出。
        values.push_back(0x2e);
        isType = false;
        isInt = false;
        while (!isType)
        {
            v = ScanTypeOrInt(isType, token, isInt, true);
            if (!isType)
            {
                values.push_back(v);
            }
        }
        break;
    }
    case 0x112c:
    {
        // ORIG 0x89a04c7（tag 0x33）：push 0x33；ScanStr 比较符
        // （=→0 >→1 <→2 >=/=>→3 <=/=<→4）push；push 一个 ScanInt。
        values.push_back(0x33);
        ScanStr(&token);
        k = 0;
        if (token == "=")
        {
            k = 0;
        }
        else if (token == ">")
        {
            k = 1;
        }
        else if (token == "<")
        {
            k = 2;
        }
        else if (token == ">=" || token == "=>")
        {
            k = 3;
        }
        else if (token == "<=" || token == "=<")
        {
            k = 4;
        }
        values.push_back(k);
        values.push_back(ScanInt((bool*)0));
        break;
    }
    case 0x113e:
    {
        // ORIG 0x89a06ae（tag 0x34）：ScanInt 映射 1→0/2→2/3→0xc/0→3/其它→0x1c
        // push；push 两个 ScanInt；ScanStr 比较符（=→0 >→1 <→2 >=/=>→3
        // <=/=<→4）push；push ScanInt。
        values.push_back(0x34);
        int m = ScanInt((bool*)0);
        k = 0x1c;
        if (m == 0)
        {
            k = 3;
        }
        else if (m == 1)
        {
            k = 0;
        }
        else if (m == 2)
        {
            k = 2;
        }
        else if (m == 3)
        {
            k = 0xc;
        }
        values.push_back(k);
        values.push_back(ScanInt((bool*)0));
        values.push_back(ScanInt((bool*)0));
        ScanStr(&token);
        k = 0;
        if (token == "=")
        {
            k = 0;
        }
        else if (token == ">")
        {
            k = 1;
        }
        else if (token == "<")
        {
            k = 2;
        }
        else if (token == ">=" || token == "=>")
        {
            k = 3;
        }
        else if (token == "<=" || token == "=<")
        {
            k = 4;
        }
        values.push_back(k);
        values.push_back(ScanInt((bool*)0));
        break;
    }
    case 0x116b:
    {
        // ORIG 0x89a0952（tag 0x35）：push 0x35；ScanStr→getStateToEnumValue
        // 得 v；ScanInt（int* 出参）得 ok；依次 push v、ok。
        values.push_back(0x35);
        ScanStr(&token);
        v = getStateToEnumValue(token);
        int ok = 0;
        ScanInt(&ok);
        values.push_back(v);
        values.push_back(ok);
        break;
    }
    case 0x1176:
    {
        std::string tok;   // ORIG 0x89a0a3a：局部串槽 -0x770（C1+2×D1，非共享 token）
        // ORIG 0x899a09da（tag 0x36）：全链必须成功，任一步失败立即 return：
        // getTargetTag(t) + ScanInt(tag) + getStatusType(st) + ScanStr 比较符
        // （<→1/<=/=<→2/=→3/>=/=>→4/>→5/其它失败）+ ScanInt(val)；
        // 成功 push 0x36, t, tag, st, op, val。
        int t = 0;
        if (!getTargetTag(&t))
        {
            return;
        }
        int tag = 0;
        if (!ScanInt(&tag))
        {
            return;
        }
        int st = 0;
        if (!getStatusType(&st))
        {
            return;
        }
        if (!ScanStr(&tok))
        {
            return;
        }
        k = 0;
        if (tok == "<")
        {
            k = 1;
        }
        else if (token == "<=" || token == "=<")
        {
            k = 2;
        }
        else if (token == "=")
        {
            k = 3;
        }
        else if (token == ">=" || token == "=>")
        {
            k = 4;
        }
        else if (token == ">")
        {
            k = 5;
        }
        else
        {
            return;
        }
        int val = 0;
        if (!ScanInt(&val))
        {
            return;
        }
        values.push_back(0x36);
        values.push_back(t);
        values.push_back(tag);
        values.push_back(st);
        values.push_back(k);
        values.push_back(val);
        break;
    }
    case 0x11a1:
    {
        std::string tok;   // ORIG 0x89a0d4d：局部串槽 -0x77c（while 循环消费）
        // ORIG 0x899a0ced（tag 0x37）：push 0x37 后 while ScanStr 循环每条
        // 升级规格：第一词 upgrade→0/amplify→1/separate upgrade→2（其它即
        // 失败返回）；第二词比较符 <→1/<=/=<→2/=→3/>=/=>→4/>→5（其它失败
        // 返回）；随后两个 ScanInt 必须成功；push kind, op, int, int。
        // tok 耗尽（ScanStr 失败）为正常结束。
        values.push_back(0x37);
        while (ScanStr(&tok))
        {
            k = 0;
            if (tok == "upgrade")
            {
                k = 0;
            }
            else if (tok == "amplify")
            {
                k = 1;
            }
            else if (tok == "separate upgrade")
            {
                k = 2;
            }
            else
            {
                return;
            }
            if (!ScanStr(&tok))
            {
                return;
            }
            int op = 0;
            if (tok == "<")
            {
                op = 1;
            }
            else if (tok == "<=" || tok == "=<")
            {
                op = 2;
            }
            else if (tok == "=")
            {
                op = 3;
            }
            else if (tok == ">=" || tok == "=>")
            {
                op = 4;
            }
            else if (tok == ">")
            {
                op = 5;
            }
            else
            {
                return;
            }
            int a2 = 0;
            int b2 = 0;
            if (!ScanInt(&a2) || !ScanInt(&b2))
            {
                return;
            }
            values.push_back(k);
            values.push_back(op);
            values.push_back(a2);
            values.push_back(b2);
        }
        break;
    }
    case 0x11dc:
    {
        values.push_back(0x38);
        break;
    }
    case 0x11df:
    {
        values.push_back(0x39);
        break;
    }
    case 0x11e4:
    {
        // ORIG 0x899a1128（tag 0x3a）：push 0x3a；ScanStr 后
        // getEquipmentEventTiming 转换结果 push。
        values.push_back(0x3a);
        ScanStr(&token);
        values.push_back(getEquipmentEventTiming(token));
        break;
    }
    default:
        break;
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

// [skill apply condition] 块直至 "[/skill apply condition]"。ORIG 用三张
// 函数局部静态 map<char const*,int,less_TCHAR>（主分派 __lookup、
// [skill group] 子表 __lookup_0、[extra condition] 子表 __lookup_1）以表查找
// 分派（报告 0x089bd8ea 实证：三组 __cxa_guard/SpinLock/__inited 双检 +
// 13 处 pair<char const(&)[N],int>::insert）。语义：
//   0x2f31 [/skill apply condition]→结束并返回 1；
//   0x2f35 [lower bound level]→ScanInt→m_head[3]（+0xc）；
//   0x2f39 [upper bound level]→ScanInt→m_head[4]（+0x10）；
//   0x2f3d [value]→ScanInt→m_head[5]（+0x14）；
//   0x2f41 [skill group]→ScanStr 查子表：0x2f49 passive→1 /
//     0x2f4c active→0 / 0x2f4f all→-1，写 m_head[2]（+0x8）；
//   0x2f59 [extra condition]→ScanStr 循环查子表：0x2f60 [normal skill]|1 /
//     0x2f63 [consume item skill]|2 / 0x2f66 [ex skill]|4 /
//     0x2f69 [except for ex skill]|3 位或入 flags，==0 返回 0，否则写
//     m_head[6]（+0x18）。
// 任一 ScanType/ScanInt/ScanStr 失败、或子表/主表未命中 → 返回 0。
static int GetSkillApplyCondition(EquipmentParameterInfo::SASPROPERTY* sp)
{
    if (sp == 0) return 0;
    std::string token;
    bool terminated = false;   // ORIG local_31：主循环续行标志
    bool result = false;       // ORIG local_32：仅命中 [/skill apply condition] 置 1
    while (terminated == false && ScanType(token, true))
    {
        int id;
        {
            static std::map<const char*, int, less_TCHAR> __lookup;
            static SpinLock __init_lock;
            static bool __inited = false;
            if (!__inited)
            {
                __init_lock.enter();
                if (__inited)
                {
                    __init_lock.leave();
                }
                    if (!__inited)
                    __lookup.insert(std::pair<const char* const, int>("[/skill apply condition]", 0x2f31));
                    if (!__inited)
                    __lookup.insert(std::pair<const char* const, int>("[lower bound level]", 0x2f35));
                    if (!__inited)
                    __lookup.insert(std::pair<const char* const, int>("[upper bound level]", 0x2f39));
                    if (!__inited)
                    __lookup.insert(std::pair<const char* const, int>("[value]", 0x2f3d));
                    if (!__inited)
                    __lookup.insert(std::pair<const char* const, int>("[skill group]", 0x2f41));
                    if (!__inited)
                    __lookup.insert(std::pair<const char* const, int>("[extra condition]", 0x2f59));
                if (!__inited)
                {
                    __init_lock.leave();
                    __inited = true;
                }
            }
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(token.c_str());
            id = (it != __lookup.end()) ? it->second : -1;
        }
        if (id == 0x2f39)
        {
            if (!ScanInt(&sp->m_head[4])) return 0;
        }
        else if (id < 0x2f3a)
        {
            if (id == 0x2f31)
            {
                result = true;
                terminated = true;
            }
            else if (id == 0x2f35)
            {
                if (!ScanInt(&sp->m_head[3])) return 0;
            }
            else
            {
                return 0;   // 主表未命中
            }
        }
        else
        {
            if (id == 0x2f3d)
            {
                if (!ScanInt(&sp->m_head[5])) return 0;
            }
            else if (id == 0x2f41)
            {
                std::string str;
                if (!ScanStr(&str)) return 0;
                int gid;
                {
                    static std::map<const char*, int, less_TCHAR> __lookup;
                    static SpinLock __init_lock;
                    static bool __inited = false;
                    if (!__inited)
                    {
                        __init_lock.enter();
                        if (!__inited)
                        {
                            if (!__inited)
                            __lookup.insert(std::pair<const char* const, int>("[passive]", 0x2f49));
                            if (!__inited)
                            __lookup.insert(std::pair<const char* const, int>("[active]", 0x2f4c));
                            if (!__inited)
                            __lookup.insert(std::pair<const char* const, int>("[all]", 0x2f4f));
                            __inited = true;
                        }
                        __init_lock.leave();
                    }
                    std::map<const char*, int, less_TCHAR>::const_iterator it =
                        __lookup.find(str.c_str());
                    gid = (it != __lookup.end()) ? it->second : -1;
                }
                int group;
                if (gid == 0x2f49)      group = 1;
                else if (gid == 0x2f4c) group = 0;
                else if (gid == 0x2f4f) group = -1;
                else                    return 0;   // 子表未命中
                sp->m_head[2] = group;
            }
            else if (id == 0x2f59)
            {
                unsigned int flags = 0;
                std::string str;
                while (ScanStr(&str))
                {
                    int eid;
                    {
                        static std::map<const char*, int, less_TCHAR> __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        if (!__inited)
                        {
                            __init_lock.enter();
                            if (!__inited)
                            {
                                if (!__inited)
                                __lookup.insert(std::pair<const char* const, int>("[normal skill]", 0x2f60));
                                if (!__inited)
                                __lookup.insert(std::pair<const char* const, int>("[consume item skill]", 0x2f63));
                                if (!__inited)
                                __lookup.insert(std::pair<const char* const, int>("[ex skill]", 0x2f66));
                                if (!__inited)
                                __lookup.insert(std::pair<const char* const, int>("[except for ex skill]", 0x2f69));
                                __inited = true;
                            }
                            __init_lock.leave();
                        }
                        std::map<const char*, int, less_TCHAR>::const_iterator it =
                            __lookup.find(str.c_str());
                        eid = (it != __lookup.end()) ? it->second : -1;
                    }
                    if (eid == 0x2f60)      flags |= 1;
                    else if (eid == 0x2f63) flags |= 2;
                    else if (eid == 0x2f66) flags |= 4;
                    else if (eid == 0x2f69) flags |= 3;
                    else                    return 0;   // 子表未命中
                }
                if (flags == 0) return 0;
                sp->m_head[6] = flags;
            }
            else
            {
                return 0;   // 主表未命中
            }
        }
    }
    return result ? 1 : 0;
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
            s_phaseMap.insert(std::pair<const char* const, int>("[all]", 0x2f8c));
            s_phaseMap.insert(std::pair<const char* const, int>("[common]", 0x2f8f));
            s_phaseMap.insert(std::pair<const char* const, int>("[first]", 0x2f92));
            s_phaseMap.insert(std::pair<const char* const, int>("[second]", 0x2f95));
            s_phaseMap.insert(std::pair<const char* const, int>("[third]", 0x2f98));
            s_phaseMap.insert(std::pair<const char* const, int>("[fourth]", 0x2f9b));
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
        // ORIG 0x089beeed..0x89bf247：函数局部 static map<char const*,int,less_TCHAR>
        // 5 项（[/all skill item]=0x2fc1, [item growtype]=0x2fc5,
        // [skill apply condition]=0x2fc9, [skill phase]=0x2fcd,
        // [skill levelup]=0x2fd1，@0x89bef2c/89bef8a/89beffb/89bf06c/89bf0dd
        // 逐条 !__inited 守卫+insert 链实证）→ find(token.c_str()) 分派。
        int id;
        {
            static std::map<const char*, int, less_TCHAR> __lookup;
            static SpinLock __init_lock;
            static bool __inited = false;
            if (!__inited)
            {
                __init_lock.enter();
                if (__inited)
                {
                    __init_lock.leave();
                }
                if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("[/all skill item]", 0x2fc1));
                if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("[item growtype]", 0x2fc5));
                if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("[skill apply condition]", 0x2fc9));
                if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("[skill phase]", 0x2fcd));
                if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("[skill levelup]", 0x2fd1));
                if (!__inited)
                {
                    __init_lock.leave();
                    __inited = true;
                }
            }
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(token.c_str());
            id = (it != __lookup.end()) ? it->second : -1;
        }
        if (id == 0x2fc1)
        {
            terminated = true;
            break;
        }
        if (id == 0x2fc5)
        {
            if (!GetItemGrowtype(&sp)) return 0;
        }
        else if (id == 0x2fc9)
        {
            if (!GetSkillApplyCondition(&sp)) return 0;
        }
        else if (id == 0x2fcd)
        {
            if (!GetSkillPhase(&sp)) return 0;
        }
        else if (id == 0x2fd1)
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
{    // [N类豁免] 残余 DIFF 归因（范式库 §7 跨版本组）：本函数指令多重集已与 ORIG
    // 对齐（逐 insert !__inited 守卫 / switch 决策树 cmp$imm,%eax+je/jg / push
    // RKi-RKf 物化位点核对，报告 ORIG 地址见各 case 注释）；剩余差异为
    // gcc4.1.2 BB 布局（init 链与 dispatch body 交插块序、leaf 体排布）与栈槽
    // 编号（ORIG 帧 0x79c/0xacc vs 4.4.7 分配序不同），源码不可控，标 N 类。

    // ORIG 0x89a75ea：单 token 分派以函数局部静态 map<char const*,int,less_TCHAR>
    // （报告 0x89a75ea 实证：一组 __cxa_guard/SpinLock/__inited 双检 + 105 处
    // pair<char const(&)[N],int>::insert）查 path.c_str() 得分支 id。原重建用
    // std::string operator== 线性比较（101 处），与 ORIG 表查找语义不同，此处转写。
    // 表含全部 106 项（含 [variable stat]→0x1bc1，ORIG 0x89acdf0 同为本表项，
    // pair IRA16 ctor 注册）；另 2 项 [potion physical attack]/[potion magical attack]
    // 在 ORIG 中注册后即无实体：分派目标 0x89acd24/0x89acd83 开头即
    // __inited 检查 + je 尾出口 0x89aceb7→0x89acebb，运行期命中为空体直达 done。
    int id;
    {
        static std::map<const char*, int, less_TCHAR> __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;
        // ORIG 0x899d3a2..+0x573 等：惰性注册链每 tag 一个 `if (!__inited)`
        // 守卫块（movzbl __inited; xor $1; test; je <body>），insert 链尾统一
        // leave + 置位。逐条守卫保持 ORIG 检查形态（报告 0x899ce3b 汇编）。
        if (!__inited)
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
            }
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[HP MAX]", 0x176d));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[HP MAX rate]", 0x1770));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[MP MAX]", 0x1773));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[MP MAX rate]", 0x1776));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[physical attack]", 0x1779));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[physical defense]", 0x177c));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[magical attack]", 0x177f));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[magical defense]", 0x1782));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[if]", 0x1785));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[then]", 0x1788));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[multiple then]", 0x1795));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[equipment physical attack]", 0x17a5));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[equipment physical defense]", 0x17a9));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[equipment magical attack]", 0x17ad));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[equipment magical defense]", 0x17b1));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[additional effect index]", 0x17b6));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[recovery status]", 0x17bb));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[separate attack]", 0x17cc));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[separate rate]", 0x17d0));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[physical absolute damage]", 0x17e6));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[physical damage reduce]", 0x17e9));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[physical absolute defense]", 0x17ec));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[magical absolute damage]", 0x17ef));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[magical damage reduce]", 0x17f2));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[magical absolute defense]", 0x17f5));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[fire resistance]", 0x17f8));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[water resistance]", 0x17fb));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[dark resistance]", 0x17fe));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[light resistance]", 0x1801));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[all elemental resistance]", 0x1804));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[slow resistance]", 0x1807));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[freeze resistance]", 0x180a));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[poison resistance]", 0x180d));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[stun resistance]", 0x1810));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[curse resistance]", 0x1813));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[blind resistance]", 0x1816));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[lightning resistance]", 0x1819));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[stone resistance]", 0x181c));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[sleep resistance]", 0x181f));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[burn resistance]", 0x1822));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[weapon break resistance]", 0x1825));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[bleeding resistance]", 0x1828));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[all activestatus resistance]", 0x182b));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[piercing resistance]", 0x182e));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[stuck resistance]", 0x1831));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[confuse resistance]", 0x1834));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[hold resistance]", 0x1837));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[armor break resistance]", 0x183a));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[inventory limit]", 0x183d));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[HP regen speed]", 0x1840));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[MP regen speed]", 0x1843));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[MP regen speed rate]", 0x1846));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[move speed]", 0x1849));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[x move dash speed]", 0x184c));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[y move dash speed]", 0x184f));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[attack speed]", 0x1852));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[cast speed]", 0x1855));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[hit recovery]", 0x1858));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[jump power]", 0x185b));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[elemental property]", 0x185e));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[active status]", 0x1862));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[attacked active status]", 0x189e));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[add absolute damage]", 0x18d7));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[add absolute defense]", 0x18e0));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[item aura]", 0x18e9));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[skill data up]", 0x1941));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[push aside]", 0x1944));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[lift up]", 0x1947));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[physical critical hit]", 0x194a));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[magical critical hit]", 0x194d));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[stuck]", 0x1950));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[ignore defense]", 0x1953));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[human damage rate]", 0x1956));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[fort damage rate]", 0x1959));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[monster damage rate]", 0x195c));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[HP drain]", 0x1969));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[MP drain]", 0x196c));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[skill levelup]", 0x1970));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[avatar select ability]", 0x1981));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[room list move speed rate]", 0x19c0));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[anti evil]", 0x19c4));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[rigidity]", 0x19c7));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[fire attack]", 0x19ca));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[water attack]", 0x19cd));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[dark attack]", 0x19d0));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[light attack]", 0x19d3));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[active status probability]", 0x19d6));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[active status level]", 0x19f4));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[active status control info]", 0x1a0e));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[account bind linear ability]", 0x1a5d));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[level linear ability]", 0x1a60));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[all skill item]", 0x1b39));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[item overpower desc]", 0x1b3d));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[item overpower part]", 0x1b41));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[ice appendage]", 0x1b47));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[curse appendage]", 0x1b53));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[upgrade cost discount]", 0x1b65));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[upgrade prob increase]", 0x1b68));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[assault cost discount]", 0x1b6b));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[decrease critical damage]", 0x1b70));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[hardcoding parameter]", 0x1b83));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[all elemental attack]", 0x1b90));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[rebirth]", 0x1b96));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[potion physical attack]", 0x1bbb));
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[potion magical attack]", 0x1bbd));
            // ORIG 0x89acdf0：[variable stat]（串 @0x8e181de，15 字符）同样为本表项，
            // pair<const char(&)[16],int> IRA16 ctor 注册形。
            if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("[variable stat]", 0x1bc1));
            if (!__inited)
            {
                __init_lock.leave();
                __inited = true;
            }
        }
        std::map<const char*, int, less_TCHAR>::const_iterator it =
            __lookup.find(path.c_str());
        id = (it != __lookup.end()) ? it->second : -1;
    }
    int iv;
    float fv;
    std::string s;

    switch (id)
    {
    case 0x176d:
    {
        info->m_hpMax= ScanInt((bool*)0); goto done;
    }
    case 0x1770:
    {
        info->m_hpMaxRate= ScanFloat(1, (bool*)0); goto done;
    }
    case 0x1773:
    {
        info->m_mpMax= ScanInt((bool*)0); goto done;
    }
    case 0x1776:
    {
        info->m_mpMaxRate= ScanFloat(1, (bool*)0); goto done;
    }
    case 0x1779:
    {
        info->m_physicalAttack= ScanInt((bool*)0); goto done;
    }
    case 0x177c:
    {
        info->m_physicalDefense= ScanInt((bool*)0); goto done;
    }
    case 0x177f:
    {
        info->m_magicalAttack= ScanInt((bool*)0); goto done;
    }
    case 0x1782:
    {
        info->m_magicalDefense= ScanInt((bool*)0); goto done;
    }
    case 0x1785:
    {
        int i = a; a = a + 1; scanIf(info, i); goto done;
    }
    case 0x1788:
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
    case 0x1795:
    {
        // ORIG 0x89a83c1：ScanType 流读到 EOF/失败或 "[/multiple then]" 时
        // b(=*param_5)+1 并成功返回；读到 "[then]" 时以当前 b 调 scanThen
        //（b 不递增），失败返回 false；其它 token 忽略继续读。
        std::string t;
        for (;;)
        {
        if (!ScanType(t, true))
        {
        b = b + 1;
        goto done;
        }
        if (t == "[/multiple then]")
        {
        b = b + 1;
        goto done;
        }
        if (t == "[then]")
        {
        if (!scanThen(dir, info, b)) return false;
        }
        }
    }
    case 0x17a5:
    {
        info->getEquipmentPhysicalAttack(0)= ScanInt((bool*)0);
        info->getEquipmentPhysicalAttack(1)= ScanInt((bool*)0); goto done;
    }
    case 0x17a9:
    {
        info->getEquipmentPhysicalDefense(0)= ScanInt((bool*)0);
        info->getEquipmentPhysicalDefense(1)= ScanInt((bool*)0); goto done;
    }
    case 0x17ad:
    {
        info->getEquipmentMagicalAttack(0)= ScanInt((bool*)0);
        info->getEquipmentMagicalAttack(1)= ScanInt((bool*)0); goto done;
    }
    case 0x17b1:
    {
        info->getEquipmentMagicalDefense(0)= ScanInt((bool*)0);
        info->getEquipmentMagicalDefense(1)= ScanInt((bool*)0); goto done;
    }
    case 0x17b6:
    {
        { iv = ScanInt((bool*)0); info->m_vec194.push_back(iv); } goto done;
    }
    case 0x17bb:
    {
        // ORIG 0x89a8902：清空 +0x240 vector<vector<float>>，ScanInt 次数 resize；
        // 循环 ScanFloat（bool 成功出参）：v 追加到第 (i % n) 个子向量并递增 i。
        info->m_vec240.clear();
        int n = ScanInt((bool*)0);
        info->m_vec240.resize(n);
        int i = 0;
        bool ok = false;
        float v2 = ScanFloat(&ok);
        while (ok)
        {
        info->m_vec240[i % n].push_back(v2);
        ++i;
        v2 = ScanFloat(&ok);
        }
        goto done;
    }
    case 0x17cc:
    {
        info->m_separateAttack[0]= ScanInt((bool*)0);
        info->m_separateAttack[1]= ScanInt((bool*)0); goto done;
    }
    case 0x17d0:
    {
        // ORIG 0x89a8aea：两个 ScanFloat 无检查写 +0x390/+0x394（float 位模式）。
        info->m_separateRate[0] = ScanFloat((bool*)0);
        info->m_separateRate[1] = ScanFloat((bool*)0);
        goto done;
    }
    case 0x17e6:
    {
        info->m_physicalAbsoluteDamage= ScanInt((bool*)0); goto done;
    }
    case 0x17e9:
    {
        info->m_physicalDamageReduce= ScanInt((bool*)0); goto done;
    }
    case 0x17ec:
    {
        info->m_physicalAbsoluteDefense= ScanFloat((bool*)0); goto done;
    }
    case 0x17ef:
    {
        info->m_magicalAbsoluteDamage= ScanInt((bool*)0); goto done;
    }
    case 0x17f2:
    {
        info->m_magicalDamageReduce= ScanInt((bool*)0); goto done;
    }
    case 0x17f5:
    {
        info->m_magicalAbsoluteDefense= ScanFloat((bool*)0); goto done;
    }
    case 0x17f8:
    {
        info->m_fireResistance= ScanInt((bool*)0); goto done;
    }
    case 0x17fb:
    {
        info->m_waterResistance= ScanInt((bool*)0); goto done;
    }
    case 0x17fe:
    {
        info->m_darkResistance= ScanInt((bool*)0); goto done;
    }
    case 0x1801:
    {
        info->m_lightResistance= ScanInt((bool*)0); goto done;
    }
    case 0x1804:
    {
        info->m_allElementalResistance= ScanInt((bool*)0); goto done;
    }
    case 0x1807:
    {
        info->m_slowResistance= ScanInt((bool*)0); goto done;
    }
    case 0x180a:
    {
        info->m_freezeResistance= ScanInt((bool*)0); goto done;
    }
    case 0x180d:
    {
        info->m_poisonResistance= ScanInt((bool*)0); goto done;
    }
    case 0x1810:
    {
        info->m_stunResistance= ScanInt((bool*)0); goto done;
    }
    case 0x1813:
    {
        info->m_curseResistance= ScanInt((bool*)0); goto done;
    }
    case 0x1816:
    {
        info->m_blindResistance= ScanInt((bool*)0); goto done;
    }
    case 0x1819:
    {
        info->m_lightningResistance= ScanInt((bool*)0); goto done;
    }
    case 0x181c:
    {
        info->m_stoneResistance= ScanInt((bool*)0); goto done;
    }
    case 0x181f:
    {
        info->m_sleepResistance= ScanInt((bool*)0); goto done;
    }
    case 0x1822:
    {
        info->m_burnResistance= ScanInt((bool*)0); goto done;
    }
    case 0x1825:
    {
        info->m_weaponBreakResistance= ScanInt((bool*)0); goto done;
    }
    case 0x1828:
    {
        info->m_bleedingResistance= ScanInt((bool*)0); goto done;
    }
    case 0x182b:
    {
        info->m_allActiveStatusResistance= ScanInt((bool*)0); goto done;
    }
    case 0x182e:
    {
        info->m_piercingResistance= ScanInt((bool*)0); goto done;
    }
    case 0x1831:
    {
        info->m_stuckResistance= ScanInt((bool*)0); goto done;
    }
    case 0x1834:
    {
        info->m_confuseResistance= ScanInt((bool*)0); goto done;
    }
    case 0x1837:
    {
        info->m_holdResistance= ScanInt((bool*)0); goto done;
    }
    case 0x183a:
    {
        info->m_armorBreakResistance= ScanInt((bool*)0); goto done;
    }
    case 0x183d:
    {
        info->m_inventoryLimit= ScanInt((bool*)0); goto done;
    }
    case 0x1840:
    {
        info->m_hpRegenSpeed= ScanInt((bool*)0); goto done;
    }
    case 0x1843:
    {
        info->m_mpRegenSpeed= ScanInt((bool*)0); goto done;
    }
    case 0x1846:
    {
        info->m_mpRegenSpeedRate= ScanFloat((bool*)0); goto done;
    }
    case 0x1849:
    {
        info->m_moveSpeed= ScanInt((bool*)0); goto done;
    }
    case 0x184c:
    {
        info->m_xMoveDashSpeed= ScanFloat((bool*)0); goto done;
    }
    case 0x184f:
    {
        info->m_yMoveDashSpeed= ScanFloat((bool*)0); goto done;
    }
    case 0x1852:
    {
        info->m_attackSpeed= ScanInt((bool*)0); goto done;
    }
    case 0x1855:
    {
        info->m_castSpeed= ScanInt((bool*)0); goto done;
    }
    case 0x1858:
    {
        info->m_hitRecovery= ScanInt((bool*)0); goto done;
    }
    case 0x185b:
    {
        info->m_jumpPower= ScanInt((bool*)0); goto done;
    }
    case 0x185e:
    {
        if (ScanStr(&s)) info->m_elementalProperty = getElement(s.c_str()); goto done;
    }
    case 0x1862:
    {
        if (ScanStr(&s)) info->m_activeStatusType = getActiveStatusType(s);
        info->m_activeStatusValue= ScanFloat((bool*)0);
        info->m_activeStatusLevel= ScanInt((bool*)0);
        info->m_activeStatusFlag = (ScanInt((bool*)0) != 0);
        info->m_activeStatusValue2= ScanInt((bool*)0);
        // ORIG 0x89aa...（+0x158 附加参数）：状态名逐条 UNROLL，push_back 直接内联
        // ScanInt((bool*)0) 返回值（右值 → push_back(int&&)）；blind/slow/haste 各 2、
        // stone/confuse/armor break/weapon break/poison/lightning/bleeding 各 1、
        // burn 显式 4 次、bless/curse 用 for(<4) 循环（各 1 调用点），共 19 个 push_back。
        if (s == "[blind]")
        { info->m_vec158.push_back(ScanInt((bool*)0)); info->m_vec158.push_back(ScanInt((bool*)0)); }
        else if (s == "[slow]")
        { info->m_vec158.push_back(ScanInt((bool*)0)); info->m_vec158.push_back(ScanInt((bool*)0)); }
        else if (s == "[haste]")
        { info->m_vec158.push_back(ScanInt((bool*)0)); info->m_vec158.push_back(ScanInt((bool*)0)); }
        else if (s == "[stone]")
        { info->m_vec158.push_back(ScanInt((bool*)0)); }
        else if (s == "[confuse]")
        { info->m_vec158.push_back(ScanInt((bool*)0)); }
        else if (s == "[armor break]")
        { info->m_vec158.push_back(ScanInt((bool*)0)); }
        else if (s == "[weapon break]")
        { info->m_vec158.push_back(ScanInt((bool*)0)); }
        else if (s == "[poison]")
        { info->m_vec158.push_back(ScanInt((bool*)0)); }
        else if (s == "[lightning]")
        { info->m_vec158.push_back(ScanInt((bool*)0)); }
        else if (s == "[bleeding]")
        { info->m_vec158.push_back(ScanInt((bool*)0)); }
        else if (s == "[burn]")
        { info->m_vec158.push_back(ScanInt((bool*)0)); info->m_vec158.push_back(ScanInt((bool*)0));
        info->m_vec158.push_back(ScanInt((bool*)0)); info->m_vec158.push_back(ScanInt((bool*)0)); }
        else if (s == "[bless]")
        { for (int k = 0; k < 4; ++k) info->m_vec158.push_back(ScanInt((bool*)0)); }
        else if (s == "[curse]")
        { for (int k = 0; k < 4; ++k) info->m_vec158.push_back(ScanInt((bool*)0)); }
        goto done;
    }
    case 0x189e:
    {
        // ORIG 0x899e...（0x189e 执行体）：状态附加 vector +0x17c 按状态名子键
        // 写入固定个数 int：blind/slow/haste→2，stone/confuse/armor break/
        // weapon break/poison/lightning/bleeding→1，burn/bless/curse→4，
        // 其它状态名不写。此处仅消费对应数量 ScanInt（ORIG 无成功检查）。
        if (ScanStr(&s)) info->m_attackedActiveStatusType = getActiveStatusType(s);
        info->m_attackedActiveStatusValue= ScanFloat((bool*)0);
        info->m_attackedActiveStatusLevel= ScanInt((bool*)0);
        info->m_attackedActiveStatusFlag = (ScanInt((bool*)0) != 0);
        info->m_attackedActiveStatusValue2= ScanInt((bool*)0);
        // ORIG 0x899e...（+0x17c 附加参数）：状态名逐条 UNROLL，push_back 内联
        // ScanInt((bool*)0)（右值 → push_back(int&&)）；blind/slow/haste 各 2、
        // stone/confuse/armor break/weapon break/poison/lightning/bleeding 各 1、
        // burn/bless/curse 各用 for(<4) 循环（各 1 调用点），共 16 个 push_back。
        if (s == "[blind]")
        { info->m_vec17c.push_back(ScanInt((bool*)0)); info->m_vec17c.push_back(ScanInt((bool*)0)); }
        else if (s == "[slow]")
        { info->m_vec17c.push_back(ScanInt((bool*)0)); info->m_vec17c.push_back(ScanInt((bool*)0)); }
        else if (s == "[haste]")
        { info->m_vec17c.push_back(ScanInt((bool*)0)); info->m_vec17c.push_back(ScanInt((bool*)0)); }
        else if (s == "[stone]")
        { info->m_vec17c.push_back(ScanInt((bool*)0)); }
        else if (s == "[confuse]")
        { info->m_vec17c.push_back(ScanInt((bool*)0)); }
        else if (s == "[armor break]")
        { info->m_vec17c.push_back(ScanInt((bool*)0)); }
        else if (s == "[weapon break]")
        { info->m_vec17c.push_back(ScanInt((bool*)0)); }
        else if (s == "[poison]")
        { info->m_vec17c.push_back(ScanInt((bool*)0)); }
        else if (s == "[lightning]")
        { info->m_vec17c.push_back(ScanInt((bool*)0)); }
        else if (s == "[bleeding]")
        { info->m_vec17c.push_back(ScanInt((bool*)0)); }
        else if (s == "[burn]")
        { for (int k = 0; k < 4; ++k) info->m_vec17c.push_back(ScanInt((bool*)0)); }
        else if (s == "[bless]")
        { for (int k = 0; k < 4; ++k) info->m_vec17c.push_back(ScanInt((bool*)0)); }
        else if (s == "[curse]")
        { for (int k = 0; k < 4; ++k) info->m_vec17c.push_back(ScanInt((bool*)0)); }
        goto done;
    }
    case 0x18d7:
    {
        if (ScanStr(&s)) info->m_addAbsoluteDamageType = (s == "[magical]") ? 1 : 0;
        info->m_addAbsoluteDamageValue= ScanFloat((bool*)0);
        info->m_addAbsoluteDamageExtra= ScanInt((bool*)0);
        goto done;
    }
    case 0x18e0:
    {
        if (ScanStr(&s)) info->m_addAbsoluteDefenseType = (s == "[magical]") ? 3 : 2;
        info->m_addAbsoluteDefenseValue= ScanFloat((bool*)0);
        info->m_addAbsoluteDefenseExtra= ScanInt((bool*)0);
        goto done;
    }
    case 0x18e9:
    {
        // ORIG 0x899a...（0x18e9 执行体）：ScanStr 状态名→m_field0；
        // ScanStr "%"→m_field4（0x01）；ScanFloat→m_float8；ScanInt→m_fieldc，
        // 全部无成功检查；构造完成后 push 到 +0x188 vector。
        ItemAuraInfo aura;
        ScanStr(&s);
        aura.m_field0 = getChangeStatusType(s);
        ScanStr(&s);
        aura.m_field4 = (s == "%") ? 1 : 0;
        aura.m_float8 = ScanFloat((bool*)0);
        aura.m_fieldc = ScanInt((bool*)0);
        info->m_aura188.push_back(aura);
        goto done;
    }
    case 0x1941:
    {
        importSkillDataUp(info->m_skill1b8); goto done;
    }
    case 0x1944:
    {
        info->m_pushAside= ScanInt((bool*)0); goto done;
    }
    case 0x1947:
    {
        info->m_liftUp= ScanInt((bool*)0); goto done;
    }
    case 0x194a:
    {
        info->m_physicalCriticalHit= ScanFloat((bool*)0); goto done;
    }
    case 0x194d:
    {
        info->m_magicalCriticalHit= ScanFloat((bool*)0); goto done;
    }
    case 0x1950:
    {
        info->m_stuck= ScanFloat((bool*)0); goto done;
    }
    case 0x1953:
    {
        info->m_ignoreDefense = (ScanInt((bool*)0) != 0); goto done;
    }
    case 0x1956:
    {
        info->m_humanDamageRate= ScanFloat((bool*)0); goto done;
    }
    case 0x1959:
    {
        info->m_fortDamageRate= ScanFloat((bool*)0); goto done;
    }
    case 0x195c:
    {
        info->m_monsterDamageRate= ScanFloat((bool*)0); goto done;
    }
    case 0x1969:
    {
        info->m_hpDrain = (ScanInt((bool*)0) != 0); goto done;
    }
    case 0x196c:
    {
        info->m_mpDrain = (ScanInt((bool*)0) != 0); goto done;
    }
    case 0x1970:
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
    case 0x1981:
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
    case 0x19c0:
    {
        info->m_roomListMoveSpeedRate= ScanFloat((bool*)0); goto done;
    }
    case 0x19c4:
    {
        info->m_antiEvil= ScanInt((bool*)0); goto done;
    }
    case 0x19c7:
    {
        info->m_rigidity= ScanInt((bool*)0); goto done;
    }
    case 0x19ca:
    {
        info->m_fireAttack= ScanInt((bool*)0); goto done;
    }
    case 0x19cd:
    {
        info->m_waterAttack= ScanInt((bool*)0); goto done;
    }
    case 0x19d0:
    {
        info->m_darkAttack= ScanInt((bool*)0); goto done;
    }
    case 0x19d3:
    {
        info->m_lightAttack= ScanInt((bool*)0); goto done;
    }
    case 0x19d6:
    case 0x19f4:
    {
        // ORIG 0x89ab9c6（probability）/0x89abafc（level）：ScanStr 状态名转枚举，
        // ScanInt 无检查；枚举 0x13 写满 18 槽，0..0x11 写对应槽，0x12/其它不写。
        if (ScanStr(&s))
        {
        int st = getActiveStatusToEnumValue(s);
        int val = ScanInt((bool*)0);
        if (st != 0x12)
        {
        if (st == 0x13)
        {
        for (int i = 0; i < 0x12; ++i)
        {
        if (id == 0x19d6)
        info->m_activeStatusProbability[i] = val;
        else
        info->m_activeStatusLevelTable[i] = val;
        }
        }
        else if (st >= 0 && st < 0x12)
        {
        if (id == 0x19d6)
        info->m_activeStatusProbability[st] = val;
        else
        info->m_activeStatusLevelTable[st] = val;
        }
        }
        }
        goto done;
    }
    case 0x1a0e:
    {
        // ORIG 0x89abc32：ScanStr 状态名→枚举，随后 3 个 ScanInt（无检查）分别写
        // 概率槽/等级槽/控制槽；枚举 0x13 写满 18 槽，0..0x11 写对应槽并按其类型
        // 追加特判：0/4/5/0xa/0x11 读入的数值仅消费不入表（部分仍写 control 区）。
        if (ScanStr(&s))
        {
        int st = getActiveStatusToEnumValue(s);
        int a = ScanInt((bool*)0);
        int b = ScanInt((bool*)0);
        int c = ScanInt((bool*)0);
        if (st != 0x12)
        {
        if (st == 0x13)
        {
        for (int i = 0; i < 0x12; ++i)
        {
        info->m_activeStatusProbability[i] = a;
        info->m_activeStatusLevelTable[i] = b;
        info->m_activeStatusCtrl[i] = c;
        }
        }
        else if (st >= 0 && st < 0x12)
        {
        info->m_activeStatusProbability[st] = a;
        info->m_activeStatusLevelTable[st] = b;
        info->m_activeStatusCtrl[st] = c;
        if (st == 0)
        {
        ScanInt((bool*)0);
        ScanInt((bool*)0);
        }
        else if (st == 2)
        {
        info->m_activeStatusControlInfo[0] = ScanInt((bool*)0);
        }
        else if (st == 4)
        {
        ScanInt((bool*)0);
        }
        else if (st == 5)
        {
        ScanInt((bool*)0);
        ScanInt((bool*)0);
        }
        else if (st == 6)
        {
        info->m_activeStatusControlInfo[1] = ScanInt((bool*)0);
        }
        else if (st == 7)
        {
        info->m_activeStatusControlInfo[2] = ScanInt((bool*)0);
        }
        else if (st == 9)
        {
        int v = ScanInt((bool*)0);
        ScanInt((bool*)0);
        ScanInt((bool*)0);
        info->m_activeStatusControlInfo[3] = v;
        }
        else if (st == 0xa)
        {
        ScanInt((bool*)0);
        }
        else if (st == 0xb)
        {
        info->m_activeStatusControlInfo[4] = ScanInt((bool*)0);
        }
        else if (st == 0x11)
        {
        ScanInt((bool*)0);
        }
        }
        }
        }
        goto done;
    }
    case 0x1a60:
    case 0x1a5d:
    {
        // ORIG 0x89abffa/0x89abf9b（两关键字共享执行体）：ScanInt（bool 出参）
        // 必须成功写 m_growthLevelBegin，失败即返回 0；ScanInt 无检查写
        // m_growthLevelEnd；随后 while ScanType 内层子标签循环写入成长表，
        // 未知标签忽略继续；EOF 为正常结束。
        int lv = 0;
        if (!ScanInt(&lv)) return false;
        info->m_growthLevelBegin = lv;
        info->m_growthLevelEnd = ScanInt((bool*)0);
        std::string t;
        while (ScanType(t, true))
        {
        if (t == "[physical attack]")
        {
        for (int i = 0; i < 2; ++i) info->m_linPhyAtk[i] = ScanInt((bool*)0);
        }
        else if (t == "[physical defense]")
        {
        for (int i = 0; i < 2; ++i) info->m_linPhyDef[i] = ScanInt((bool*)0);
        }
        else if (t == "[magical attack]")
        {
        for (int i = 0; i < 2; ++i) info->m_linMagAtk[i] = ScanInt((bool*)0);
        }
        else if (t == "[magical defense]")
        {
        for (int i = 0; i < 2; ++i) info->m_linMagDef[i] = ScanInt((bool*)0);
        }
        else if (t == "[equipment physical attack]")
        {
        for (int i = 0; i < 4; ++i) info->m_linEqPhyAtk[i] = ScanInt((bool*)0);
        }
        else if (t == "[equipment physical defense]")
        {
        for (int i = 0; i < 4; ++i) info->m_linEqPhyDef[i] = ScanInt((bool*)0);
        }
        else if (t == "[equipment magical attack]")
        {
        for (int i = 0; i < 4; ++i) info->m_linEqMagAtk[i] = ScanInt((bool*)0);
        }
        else if (t == "[equipment magical defense]")
        {
        for (int i = 0; i < 4; ++i) info->m_linEqMagDef[i] = ScanInt((bool*)0);
        }
        else if (t == "[grade]")
        {
        info->m_growthGradeBegin = ScanInt((bool*)0);
        info->m_growthGradeEnd = ScanInt((bool*)0);
        }
        else if (t == "[separate attack]")
        {
        for (int i = 0; i < 4; ++i)
        {
        ScanInt(&info->m_linSepAtk[i]);
        }
        }
        else if (t == "[separate rate]")
        {
        for (int i = 0; i < 4; ++i)
        {
        ScanFloat(&info->m_linSepRate[i]);
        }
        }
        else if (t == "[repair price]")
        {
        info->m_growthRepairBegin = ScanInt((bool*)0);
        info->m_growthRepairEnd = ScanInt((bool*)0);
        }
        else if (t == "[/level linear ability]")
        {
        break;
        }
        }
        goto done;
    }
    case 0x1b39:
    {
        if (importAllSkillItem(info) != 1) return false; goto done;
    }
    case 0x1b3d:
    {
        if (importItemOverpowerDesc(info) != 1) return false; goto done;
    }
    case 0x1b41:
    {
        // ORIG 0x89ac64d：`mov 0x8(%ebp),%eax; movb $0x1,0x425(%eax)` →
        // info->+0x425（m_119.m_425）置 1，随后统一尾出口 jmp 0x89acebb。
        info->m_119.m_425 = 1;
        goto done;
    }
    case 0x1b47:
    {
        // ORIG 0x89ac6b9：3× ScanInt((bool*)0) 先存独立局部 -0x3c/-0x38/-0x34，
        // 再按序写元素 -0xa74..-0xa68：{0, i1, i2, i3}（movl $0 + 3 次拷贝，
        // 第 5 个 int ORIG 不写，push_back 仍拷 0x14：0x89c6e76 lea 0x14(%eax)），
        // push_back(&info->m_aura200) @0x89ac721。
        int i1 = ScanInt((bool*)0);
        int i2 = ScanInt((bool*)0);
        int i3 = ScanInt((bool*)0);
        EquipmentParameterInfo::EquipmentAuraAppendageData e;
        e.m_field0 = 0;
        e.m_field4 = i1;
        e.m_field8 = i2;
        e.m_fieldc = i3;
        info->m_aura200.push_back(e);
        goto done;
    }
    case 0x1b53:
    {
        // ORIG 0x89ac78b：4× ScanInt((bool*)0) 先存独立局部 -0x30/-0x2c/-0x28/-0x24，
        // 再按序写元素 -0xa88..-0xa78：{1, j1, j2, j3, j4}（5 int 全写），
        // push_back(&info->m_aura200) @0x89ac80b。
        int j1 = ScanInt((bool*)0);
        int j2 = ScanInt((bool*)0);
        int j3 = ScanInt((bool*)0);
        int j4 = ScanInt((bool*)0);
        EquipmentParameterInfo::EquipmentAuraAppendageData e;
        e.m_field0 = 1;
        e.m_field4 = j1;
        e.m_field8 = j2;
        e.m_fieldc = j3;
        e.m_field10 = j4;
        info->m_aura200.push_back(e);
        goto done;
    }
    case 0x1b65:
    {
        // ORIG 0x89ac815：ScanFloat 无检查写 +0x444。
        info->m_upgradeCostDiscount = ScanFloat((bool*)0);
        goto done;
    }
    case 0x1b68:
    {
        // ORIG 0x89ac88c：ScanFloat 无检查写 +0x448。
        info->m_upgradeProbIncrease = ScanFloat((bool*)0);
        goto done;
    }
    case 0x1b6b:
    {
        info->m_assaultCostDiscount= ScanFloat((bool*)0); goto done;
    }
    case 0x1b70:
    {
        // ORIG 0x89aca1d：ScanStr "%" 判定；ScanInt 无检查转 float；
        // 非 %：值位模式存 +0x210（汇编 mov 32 位）；%：+0x20c = 值 / 10.0。
        ScanStr(&s);
        float f = (float)ScanInt((bool*)0);
        if (s == "%")
        info->m_decreaseCriticalDamage = f / 10.0f;
        else
        info->m_210 = f;
        goto done;
    }
    case 0x1b83:
    {
        bool ok = false;
        int v;
        while (ScanInt(&v)) { info->m_vec214.push_back(v); ok = true; }
        (void)ok;
        goto done;
    }
    case 0x1b90:
    {
        info->m_allElementalAttack= ScanInt((bool*)0); goto done;
    }
    case 0x1b96:
    {
        std::string t;
        while (ScanType(t, true))
        {
        if (t == "[probability]")      { info->m_rebirth450.m_field0= ScanInt((bool*)0); }
        else if (t == "[consume item]"){ info->m_rebirth450.m_pair4.first= ScanInt((bool*)0);
        info->m_rebirth450.m_pair4.second= ScanInt((bool*)0); }
        else if (t == "[hp recovery]") { info->m_rebirth450.m_fieldc= ScanInt((bool*)0); }
        else if (t == "[cooltime]")    { info->m_rebirth450.m_field14= ScanInt((bool*)0); }
        else if (t == "[mp recovery]") { info->m_rebirth450.m_field10= ScanInt((bool*)0); }
        else break;
        }
        goto done;
    }
    case 0x1bbb:
    case 0x1bbd:
    {
        // ORIG 0x89acd24（0x1bbb [potion physical attack]，串 @0x8e181ad，
        // pair IRA25 注册 @0x89acd5b）/0x89acd83（0x1bbd [potion magical attack]，
        // 串 @0x8e181c6，pair IRA24 注册 @0x89acdba）：分派目标即注册块本身，
        // 块首 `movzbl 0x94fe044; xor $0x1; test; je 89aceb7/89aceba` —— 运行期
        // __inited 已置位，je 直接落统一尾出口 0x89acebb（scanRandomOptionValue
        // + return true）。ORIG 对该二 id 无任何解析实体，仅注册 + 命中不处理。
        goto done;
    }
    case 0x1bc1:
    {
        // ORIG 0x89ace3f：VariableStat 局部 ctor（0x89c2dfc）后
        // while (scanVariableStat(v))（0x88bdb27）成功即 info->m_var474（+0x474）
        // push_back（0x89ace65）直接逐条追加；结束 jmp 尾出口 0x89acebb。
        VariableStat v;
        while (scanVariableStat(v)) info->m_var474.push_back(v);
        goto done;
    }
    default:
        break;
    }

    if (id == 0x1970)
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
    if (id == 0x1981)
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

    if (id == 0x1785)
    { int i = a; a = a + 1; scanIf(info, i); goto done; }
    if (id == 0x1788)
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
    if (id == 0x1795)
    {
        // ORIG 0x89a83c1：ScanType 流读到 EOF/失败或 "[/multiple then]" 时
        // b(=*param_5)+1 并成功返回；读到 "[then]" 时以当前 b 调 scanThen
        //（b 不递增），失败返回 false；其它 token 忽略继续读。
        std::string t;
        for (;;)
        {
            if (!ScanType(t, true))
            {
                b = b + 1;
                goto done;
            }
            if (t == "[/multiple then]")
            {
                b = b + 1;
                goto done;
            }
            if (t == "[then]")
            {
                if (!scanThen(dir, info, b)) return false;
            }
        }
    }

    // 未命中任何关键字：ORIG 全链未匹配（LAB_089ace7e）→ return 0。
    return false;

done:
    // ORIG 0x89acebb：成功路径对非 [if]/[then] token 经 param_1->m_434 子指针
    // 调 scanRandomOptionValue（+0x434 指针可能为空，scanRandomOptionValue 空
    // 指针直接返回 true）。
    if (path != "[if]" && path != "[then]")
        scanRandomOptionValue(info->m_434, path);
    return true;
}

// exportEquipmentParameterScript —— ORIG 0x089acf78（对照报告 1527-2007 行逐段转写）。
// 各段条件与标签文本照 ORIG：无名称的状态类型（如 0x12 默认）不输出任何内容。
// 注：ORIG 的 %.2f/%.1f 传参经 fistpl 取整后按 int 传入（Ghidra ROUND 伪影，原
// 生即为未定义 vararg 用法）；此处以可读 double 直出（数值舍入差异记录于报告）。
// avatar 能力标签表与 importEquipmentParameterInfo 内一致（AVTVARIATION_TAG_NAME）。
void exportEquipmentParameterScript(FILE* file, EquipmentParameterInfo* info)
{
    if (!file || !info)
    {
        return;
    }
    // 状态名表（无括号标签，索引 = 状态类型 0..0x11；15/16 无名占位空串）
    static const char* const kActStatNoBox[18] = {
        "slow", "freeze", "poison", "stun", "curse", "blind", "lightning",
        "stone", "sleep", "burn", "weapon break", "bleeding", "haste",
        "bless", "element", "", "", "armor break"
    };
    // avatar 能力标签表（47 项，与 import [avatar select ability] 同序）
    static const char* const kAvatarTag[47] = {
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
    // 基础属性（ORIG：非 0 才输出）
    if (info->m_hpMax != 0)            fprintf(file, "[HP MAX] %+d\n", info->m_hpMax);
    if (info->m_mpMax != 0)            fprintf(file, "[MP MAX] %+d\n", info->m_mpMax);
    if (info->m_physicalAttack != 0)   fprintf(file, "[physical attack] %+d\n", info->m_physicalAttack);
    if (info->m_physicalDefense != 0)  fprintf(file, "[physical defense] %+d\n", info->m_physicalDefense);
    if (info->m_magicalAttack != 0)    fprintf(file, "[magical attack] %+d\n", info->m_magicalAttack);
    if (info->m_magicalDefense != 0)   fprintf(file, "[magical defense] %+d\n", info->m_magicalDefense);

    // 装备能力数组（经访问器）
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
    if (info->m_separateAttack[0] != 0 || info->m_separateAttack[1] != 0)
        fprintf(file, "[separate attack] %+d %+d\n", info->m_separateAttack[0], info->m_separateAttack[1]);

    // [separate rate]（+0x390/+0x394 float）：与 1.0 的偏差超过 epsilon 才输出
    {
        float sa = info->m_separateRate[0];
        float sb = info->m_separateRate[1];
        float da = sa - 1.0f;
        float db = sb - 1.0f;
        // ORIG @0x89ad...：两条件各调一次 numeric_limits<float>::epsilon()
        if ((std::numeric_limits<float>::epsilon() * (sa < 0.0f ? -sa : sa) < (da < 0.0f ? -da : da)) ||
            (std::numeric_limits<float>::epsilon() * (sb < 0.0f ? -sb : sb) < (db < 0.0f ? -db : db)))
            fprintf(file, "[separate rate] %+f %+f\n",
                    (double)sa, (double)sb);
    }

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
    if (info->m_allActiveStatusResistance != 0) fprintf(file, "[all activestatus resistance] %+d\n", info->m_allActiveStatusResistance);
    if (info->m_piercingResistance != 0)    fprintf(file, "[piercing resistance] %+d\n", info->m_piercingResistance);
    if (info->m_stuckResistance != 0)       fprintf(file, "[stuck resistance] %+d\n", info->m_stuckResistance);
    if (info->m_confuseResistance != 0)     fprintf(file, "[confuse resistance] %+d\n", info->m_confuseResistance);
    if (info->m_holdResistance != 0)        fprintf(file, "[hold resistance] %+d\n", info->m_holdResistance);
    if (info->m_armorBreakResistance != 0)  fprintf(file, "[armor break resistance] %+d\n", info->m_armorBreakResistance);
    if (info->m_inventoryLimit != 0)        fprintf(file, "[inventory limit] %+d\n", info->m_inventoryLimit);
    if (info->m_hpRegenSpeed != 0)          fprintf(file, "[HP regen speed] %+d\n", info->m_hpRegenSpeed);
    if (info->m_mpRegenSpeed != 0)          fprintf(file, "[MP regen speed] %+d\n", info->m_mpRegenSpeed);
    if (info->m_mpRegenSpeedRate != 0.0f)   fprintf(file, "[MP regen speed rate] %+.2f\n", (double)info->m_mpRegenSpeedRate);
    if (info->m_moveSpeed != 0)             fprintf(file, "[move speed] %+d\n", info->m_moveSpeed);
    if (info->m_attackSpeed != 0)           fprintf(file, "[attack speed] %+d\n", info->m_attackSpeed);
    if (info->m_castSpeed != 0)             fprintf(file, "[cast speed] %+d\n", info->m_castSpeed);
    if (info->m_hitRecovery != 0)           fprintf(file, "[hit recovery] %+d\n", info->m_hitRecovery);
    if (info->m_jumpPower != 0)             fprintf(file, "[jump power] %+d\n", info->m_jumpPower);

    if (info->m_elementalProperty != 4)
        fprintf(file, "[elemental property] `%s`\n",
                getElementName(static_cast<ENUM_ELEMENT>(info->m_elementalProperty)));

    if (info->m_allElementalAttack != 0)
        fprintf(file, "[all elemental attack] %+d\n", info->m_allElementalAttack);

    // [active status]：名称段 16 条常量 fwrite（ORIG 0x89adb49..0x89ade98：
    // switch(m_activeStatusType)+逐条 fwrite(str,1,len,file)，无参数字符串不
    // 经 fprintf）＋ 值/等级共享尾（0x12 不输出名称段）。
    {
        int t = info->m_activeStatusType;
        if (t == 0)
            fwrite("[active status] `[slow]`", 1, 0x18, file);
        if (t == 1)
            fwrite("[active status] `[freeze]`", 1, 0x1a, file);
        if (t == 2)
            fwrite("[active status] `[poison]`", 1, 0x1a, file);
        if (t == 3)
            fwrite("[active status] `[stun]`", 1, 0x18, file);
        if (t == 4)
            fwrite("[active status] `[curse]`", 1, 0x19, file);
        if (t == 5)
            fwrite("[active status] `[blind]`", 1, 0x19, file);
        if (t == 6)
            fwrite("[active status] `[lightning]`", 1, 0x1d, file);
        if (t == 7)
            fwrite("[active status] `[stone]`", 1, 0x19, file);
        if (t == 8)
            fwrite("[active status] `[sleep]`", 1, 0x19, file);
        if (t == 9)
            fwrite("[active status] `[burn]`", 1, 0x18, file);
        if (t == 10)
            fwrite("[active status] `[weapon break]`", 1, 0x20, file);
        if (t == 11)
            fwrite("[active status] `[armor break]`", 1, 0x1f, file);
        if (t == 12)
            fwrite("[active status] `[bleeding]`", 1, 0x1c, file);
        if (t == 13)
            fwrite("[active status] `[haste]`", 1, 0x19, file);
        if (t == 14)
            fwrite("[active status] `[bless]`", 1, 0x19, file);
        if (t == 15)
            fwrite("[active status] `[element]`", 1, 0x1b, file);
        if (t != 0x12)   // ORIG 0x89adea6：共享值尾（0x12 无名段亦无值段）
            fprintf(file, " %.2f %d\n",
                    (double)info->m_activeStatusValue, info->m_activeStatusLevel);
    }

    if (info->m_pushAside != 0)          fprintf(file, "[push aside] %+d\n", info->m_pushAside);
    if (info->m_liftUp != 0)             fprintf(file, "[lift up] %+d\n", info->m_liftUp);
    if (info->m_physicalCriticalHit != 0.0f) fprintf(file, "[physical critical hit] %+.2f\n", (double)info->m_physicalCriticalHit);
    if (info->m_magicalCriticalHit != 0.0f) fprintf(file, "[magical critical hit] %+.2f\n", (double)info->m_magicalCriticalHit);
    if (info->m_humanDamageRate != 0.0f) fprintf(file, "[human damage rate] %+.1f\n", (double)info->m_humanDamageRate);
    if (info->m_fortDamageRate != 0.0f)  fprintf(file, "[fort damage rate] %+.1f\n", (double)info->m_fortDamageRate);
    if (info->m_monsterDamageRate != 0.0f) fprintf(file, "[monster damage rate] %+.1f\n", (double)info->m_monsterDamageRate);
    if (info->m_stuck != 0.0f)           fprintf(file, "[stuck] %+.2f\n", (double)info->m_stuck);
    if (info->m_hpDrain)                 fwrite("[HP drain] 1\n", 1, 0xd, file);
    if (info->m_mpDrain)                 fwrite("[MP drain] 1\n", 1, 0xd, file);
    if (info->m_ignoreDefense)           fwrite("[ignore defense] 1\n", 1, 0x13, file);

    // skill levelup（job 名经 getCharacterJobName，common 输出 [common]）
    if (!info->m_levelUpSkills[0].empty())
    {
        fprintf(file, "[skill levelup]\n");
        for (std::vector<LevelUpSkill>::iterator it = info->m_levelUpSkills[0].begin();
             it != info->m_levelUpSkills[0].end(); ++it)
        {
            if ((*it).m_job == 0xb)
                fwrite("[common]", 1, 8, file);
            else
                fprintf(file, "`%s`", getCharacterJobName(
                    static_cast<ENUM_CHARACTERJOB>((*it).m_job)));
            fprintf(file, " %d %d\n", (*it).m_field4, (*it).m_field8);
        }
        fwrite("[/skill levelup]\n", 1, 0x11, file);
    }

    // avatar select ability（空时才输出 room list move speed rate）
    if (info->m_avatar264.empty())
    {
        if (info->m_roomListMoveSpeedRate != 0.0f)
            fprintf(file, "[room list move speed rate] %.2f\n",
                    (double)info->m_roomListMoveSpeedRate);
    }
    else
    {
        fprintf(file, "[avatar select ability]\n");
        if (info->m_avatar264[0].m_field0 != 0)
        {
            int n = (int)info->m_avatar264.size();
            if (n > 100) n = 100;
            // ORIG 0x89ae3xx..：逐条先 snprintf 进 0x100 栈缓冲再 fprintf(file,buf)；
            // 字段读取逐次重读 operator[]（P157），job 名经临时 string c_str()。
            for (int i = 0; i < n; ++i)
            {
                char buf[0x100];
                int idx = info->m_avatar264[i].m_field0;
                if (idx == 0x2b)
                {
                    std::string job;
                    if (info->m_avatar264[i].m_skill.m_job == 0xb)
                        job = "[common]";
                    else
                        job = getCharacterJobName(
                            static_cast<ENUM_CHARACTERJOB>(info->m_avatar264[i].m_skill.m_job));
                    snprintf(buf, 0x100, "`%s` `%s` %d %d", kAvatarTag[idx], job.c_str(),
                             info->m_avatar264[i].m_skill.m_field4, info->m_avatar264[i].m_skill.m_field8);
                }
                else
                {
                    const char* sign = (info->m_avatar264[i].m_field4 == 0) ? "+" : "*";
                    snprintf(buf, 0x100, "`%s` `%s` %.2f", kAvatarTag[idx], sign,
                             (double)info->m_avatar264[i].m_field8);
                }
                fprintf(file, buf);
            }
        }
        fwrite("[/avatar select ability]\n", 1, 0x19, file);
    }

    if (info->m_rigidity != 0)       fprintf(file, "[rigidity] %d\n", info->m_rigidity);
    if (info->m_fireAttack != 0)     fprintf(file, "[fire attack] %d\n", info->m_fireAttack);
    if (info->m_waterAttack != 0)    fprintf(file, "[water attack] %d\n", info->m_waterAttack);
    if (info->m_darkAttack != 0)     fprintf(file, "[dark attack] %d\n", info->m_darkAttack);
    if (info->m_lightAttack != 0)    fprintf(file, "[light attack] %d\n", info->m_lightAttack);

    // active status probability / level（18 项表，非 0 输出）
    for (int i = 0; i < 0x12; ++i)
    {
        if (info->m_activeStatusProbability[i] != 0)
            fprintf(file, "[active status probability] `%s` %d\n",
                    kActStatNoBox[i], info->m_activeStatusProbability[i]);
    }
    for (int i = 0; i < 0x12; ++i)
    {
        if (info->m_activeStatusLevelTable[i] != 0)
            fprintf(file, "[active status level] `%s` %d\n",
                    kActStatNoBox[i], info->m_activeStatusLevelTable[i]);
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
        // ORIG 0x088c077c..0x88c0a4f：函数局部 static map<char const*,int,less_TCHAR>
        // 7 项（[static]=0xa47 [level]=0xa4b [cooltime]=0xa4f [mp]=0xa54
        // [maintain mp]=0xa59 [skill consume item]=0xa5e [casting time]=0xa6d，
        // insert 链 @0x88c078e..0x88c0a6b 逐条 !__inited 守卫实证）→ find 分派。
        int id;
        {
            static std::map<const char*, int, less_TCHAR> __lookup;
            static SpinLock __init_lock;
            static bool __inited = false;
            if (!__inited)
            {
                __init_lock.enter();
                if (__inited)
                {
                    __init_lock.leave();
                }
                if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("[static]", 0xa47));
                if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("[level]", 0xa4b));
                if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("[cooltime]", 0xa4f));
                if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("[mp]", 0xa54));
                if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("[maintain mp]", 0xa59));
                if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("[skill consume item]", 0xa5e));
                if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("[casting time]", 0xa6d));
                if (!__inited)
                {
                    __init_lock.leave();
                    __inited = true;
                }
            }
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(line.c_str());
            id = (it != __lookup.end()) ? it->second : -1;
        }
        if (id == 0xa47)
        {
            e.m_key = 0; e.m_v8 = ScanInt((bool*)0);
        }
        else if (id == 0xa4b)
        {
            e.m_key = 1; e.m_v8 = ScanInt((bool*)0);
        }
        else if (id == 0xa4f)
        {
            e.m_key = 2; ScanInt((bool*)0); e.m_v8 = 0;   // ORIG 丢弃 ScanInt 结果
        }
        else if (id == 0xa54)
        {
            e.m_key = 3; ScanInt((bool*)0); e.m_v8 = 0;   // ORIG 丢弃 ScanInt 结果
        }
        else if (id == 0xa59)
        {
            e.m_key = 4; ScanInt((bool*)0); e.m_v8 = 0;   // ORIG 丢弃 ScanInt 结果
        }
        else if (id == 0xa5e)
        {
            e.m_key = 5; e.m_v8 = ScanInt((bool*)0);
        }
        else if (id == 0xa6d)
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
