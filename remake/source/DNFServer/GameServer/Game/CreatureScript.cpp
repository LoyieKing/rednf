// ============================================================================
// df_game_r 还原 —— native CreatureScript / AttackInfo 类型实现 + 两个解析函数
//   importCreatureScript / ImportAttackInfoScript 及 g_creatureScriptFileList 全局。
//   依据 docs/class_func_reports/_lt_global_gt_/importCreatureScript.md、
//   ImportAttackInfoScript.md 与 AttackInfo/*.md。
// ============================================================================
#include "CreatureScript.h"

#include <string.h>
#include <cstdio>
#include <utility>

#include "DNFLexWrapper.h"
#include "GameScriptEnumHelpers.h"
#include "LogManager.h"
#include "NpcScript.h"
#include "STComputeDamageVariable.h"
#include "CEnvironment.h"     // class SpinLock / struct less_TCHAR

// processModuleTagSet ORIG T 0x08a97637（RDARScriptSkill.cpp 模块标签域；实现于
// LinkMiscSecuTimerSolo.cpp）。ImportAttackInfoScript 每行先送其辨认 module 开/闭
// 边界行（返回 0 表示已消费，跳过该行数据解析）。
bool processModuleTagSet(int& moduleId, int& subId, std::string* tag,
                         int type, bool flag);

// ---- 全局（ORIG 属 RDARScriptObject.cpp/RDARScriptAttackInfo.cpp）----
char g_creatureScriptBaseDirectory[0x104];

// ORIG g_creatureScriptFileList（STScriptFileList，+0x00 即 map<int,string>）。
std::map<int, std::string> g_creatureScriptFileList;

// 模块名表 MODULE_NAME_TAG（processModuleTagSet 用；+8 步长 = 开/闭 string 对）。
std::string g_moduleNameTag[17][2];  // ORIG MODULE_NAME_TAG B 0x88 字节（17 对 = 34 string）

// ---- 原生类型生命周期 ----

selectiveGrowCreatureInfo::selectiveGrowCreatureInfo()
{
    clear();
}

void selectiveGrowCreatureInfo::clear()
{
    // ORIG 0x833df64：growLevel/AType/BTypeItem/AType/BTypeCreature 置 -1，growItemCount 置 1。
    m_usingExpItem = 0;
    m_growLevel = -1;
    m_growATypeItem = -1;
    m_growBTypeItem = -1;
    m_growATypeCreature = -1;
    m_growBTypeCreature = -1;
    m_growItemCount = 1;
}

void CreatureScriptSkillInfo::clear()
{
    // ORIG 0x833dff0：m_fieldc=1、m_field10=-1，string 从 +0x14 起 5 个
    m_str0.clear();
    m_str4.clear();
    m_field8 = 0;
    m_fieldc = 1;
    m_field10 = -1;
    m_str14.clear();
    m_str18.clear();
    m_str1c.clear();
    m_str20.clear();
    m_str24.clear();
}

// ORIG 0x833e5ac：构造全部字符串/向量/映射成员（自动成员构造）后调 clear()。
CreatureScript::CreatureScript()
    : m_name(), m_basicMotion(), m_walkMotion(), m_runMotion(), m_flyMotion(),
      m_getExpMotion(), m_getExpOverMotion(), m_getExpUnderMotion(),
      m_skillMotion(), m_overSkillMotion(), m_responseMotion(), m_specialMotion(),
      m_equipMotion(), m_equipOverMotion(), m_equipUnderMotion(),
      m_basicOverEffect(), m_walkOverEffect(), m_runOverEffect(),
      m_defaultOverEffect(),
      m_skillOverEffect(), m_overSkillOverEffect(), m_responseOverEffect(),
      m_specialOverEffect(), m_basicUnderEffect(), m_walkUnderEffect(),
      m_runUnderEffect(), m_defaultUnderEffect(), m_skillUnderEffect(),
      m_overSkillUnderEffect(), m_responseUnderEffect(), m_specialUnderEffect(),
      m_additionalAnimations(), m_defaultParticle(), m_responseSoundTag(),
      m_wingSoundTag(), m_walkSoundTag(), m_runSoundTag(),
      m_etcMotion(), m_attackInfo(), m_skillString(), m_skillLevelValues(),
      m_overSkillLevelValues(), m_piercing(), m_intData(), m_stringData(),
      m_directory(), m_evolutionMultyId(), m_selectiveGrow(), m_randomMotion(),
      m_itemEvolCreatureIds(), m_skillInfo(), m_overSkillInfo(), m_skillList(),
      m_overSkillList(), m_eventEvlouteCreature()
{
    // ORIG 0x833e5ac：仅构造类类型成员（标量未初始化，由 clear() 复位）。
    clear();
}

// ORIG 0x8340ca4：逆序析构容器成员（成员自动析构）。
CreatureScript::~CreatureScript()
{
}

// ORIG 0x833dfac：复位全部标量 + clear 各容器。
void CreatureScript::clear()
{
    m_evolutionMultyId.clear();
    m_id = 0;
    m_name = "";
    m_field8 = 0;
    m_fieldc = 0;
    m_field10 = 0;
    m_layer = 1;
    m_field18 = (int)0xffffffce;
    m_field1c = 0;
    m_field20 = 0;
    m_directionCorrect = 1;
    // ORIG 0x833dfac：循环 i=0..1 清 m_moveSpeed[i] 与 m_moveSpeed[i+2]
    // （覆盖 +0x28..+0x34），再单独清 +0x38（moveSpeed[4]）。
    for (int i = 0; i <= 1; ++i)
    {
        m_moveSpeed[i] = 0;
        m_moveSpeed[i + 2] = 0;
    }
    m_moveSpeed[4] = 0;
    m_gravity = 1500;
    m_startLevel = 1;
    m_permissionLevel = 1;
    m_maxLevel = 1;
    m_parentMaxLevel = 1;
    // ORIG 0x833dfac：循环 i=0..2 清 m_artifactSlotRed/Blue/Green（+0x50..+0x58）。
    for (int i = 0; i <= 2; ++i)
        (&m_artifactSlotRed)[i] = 0;
    m_learnOverSkillLevel = 1;
    m_skillMP = 0;
    m_overSkillMP = 0;
    m_skillType = 0;
    m_overSkillType = 0;
    m_basicMotion = "";
    m_walkMotion = "";
    m_runMotion = "";
    // ORIG clear 不复位 m_flyMotion（+0x7c）
    m_getExpMotion = "";
    m_getExpOverMotion = "";
    m_getExpUnderMotion = "";
    m_skillMotion = "";
    m_overSkillMotion = "";
    m_responseMotion = "";
    m_specialMotion = "";
    m_equipMotion = "";
    m_equipOverMotion = "";
    m_equipUnderMotion = "";
    m_basicOverEffect = "";
    m_walkOverEffect = "";
    m_runOverEffect = "";
    // ORIG clear 不复位 m_defaultOverEffect（+0xb4）
    m_skillOverEffect = "";
    m_overSkillOverEffect = "";
    m_responseOverEffect = "";
    m_specialOverEffect = "";
    m_basicUnderEffect = "";
    m_walkUnderEffect = "";
    m_runUnderEffect = "";
    // ORIG clear 不复位 m_defaultUnderEffect（+0xd4）
    m_skillUnderEffect = "";
    m_overSkillUnderEffect = "";
    m_responseUnderEffect = "";
    m_specialUnderEffect = "";
    m_additionalAnimations.clear();
    m_defaultParticle = "";
    m_responseSoundTag = "";
    m_wingSoundTag = "";
    m_walkSoundTag = "";
    m_runSoundTag = "";
    m_revisionX = 0;
    m_revisionY = 0;
    m_etcMotion.clear();
    m_attackInfo.clear();
    m_skillString.clear();
    m_skillLevelValues.clear();
    m_overSkillLevelValues.clear();
    m_piercing.clear();
    m_intData.clear();
    m_stringData.clear();
    m_directory = "";
    m_evolutionQuestId = 0;
    m_evolutionCreatureId = 0;
    m_evolutionLevel = 0;
    m_selectiveGrow.clear();
    m_randomMotion.clear();
    m_defaultRandomMotionRate = 0;
    m_increaseRandomMotionRate = 0;
    m_itemEvolution = 0;
    m_itemEvolCreatureIds.clear();
    m_skillInfo.clear();
    m_overSkillInfo.clear();
    m_skillList.clear();
    m_overSkillList.clear();
    m_usingRandomSkill = 0;
    m_eventEvlouteCreature.clear();
}

// ---- AttackInfo 生命周期与方法 ----

// ORIG 0x889037c：构造 string/set/向量 后调 clear()。
// ORIG 0x0889037c：只构造成员容器（string/set/vector<int>[3]/vector<CatchObjectInfo>）
// 后调 clear()，标量值全部由 clear() 设定（ctor 不预置 0）。
AttackInfo::AttackInfo()
    : m_hitWav(), m_elements(), m_activeStatusVectors(), m_catchObjects()
{
    clear();
}

AttackInfo::~AttackInfo()
{
}

// ORIG 0x88904b8：复位全部标量 + 各容器 clear。
void AttackInfo::clear()
{
    m_damage = 0.0f;
    m_absoluteDamage = 0;
    m_damageBonus = 0;
    m_attackType = 0;
    m_weaponDamageApply = 0;
    m_humanActiveStatusRate = 1.0f;
    m_damageReaction = 1;
    m_pushAside = 0;
    m_liftUp = 0;
    m_criticalHit = 0.0f;
    m_stuck = 0.0f;
    m_stuckProbIncrease = 0;
    m_knuckBack = 0;
    m_knuckBackAngle = 0;
    m_weakness = 0.0f;
    m_weakness2 = 0.0f;
    m_bounce = 0;
    m_ignoreDefense = 0;
    m_hitWav = "";
    m_attackDirection = 1;
    m_hitInfo = 5;
    m_blood = 0;
    m_field5c = -1;
    m_field60 = -1.0f;
    m_isUseWeaponHitDelayRate = 0;
    m_hitDirection = 0;
    m_ignoreSuperArmor = 0;
    m_ignoreWeight = 0;
    m_hpDrain = 0;
    m_mpDrain = 0;
    m_field80 = 0.0f;    // ORIG 0x88904e3 mov $0x0,%edx;mov %edx,0x80（float 0 中转）
    m_field84 = 100.0f;  // ORIG 0x88904f2 mov $0x42c80000,%edx;mov %edx,0x84（float 100）
    m_isHitNumberIncreasing = 1;
    m_forceHitStunTime = 0;
    m_moduleTag = 3;
    m_ignoreDiehard = 0;
    m_hitCheckByScreenPos = 0;
    // ORIG 循环1（0x98+i*4 = (i==2)：m_attackFlags[0..2]）
    for (int i = 0; i < 3; ++i)
        m_attackFlags[i] = (i == 2);
    m_elements.clear();
    // ORIG 循环2：type/value/int/int2/int3 + vector 清空
    for (int i = 0; i < 3; ++i)
    {
        m_activeStatusType[i] = 0x12;
        m_activeStatusValue[i] = 0.0f;
        m_activeStatusInt[i] = 0;
        m_activeStatusInt2[i] = 0;
        m_activeStatusInt3[i] = 0;
        m_activeStatusVectors[i].clear();
    }
    // ORIG 循环3：damageRate = 100.0f
    for (int i = 0; i < 3; ++i)
        m_damageRate[i] = 100.0f;
    m_maxTargetHitNumber = -1;
    m_catchObjects.clear();
}

// ORIG 0x8890760：+0x14 != 0 或 damage>0 或 absoluteDamage>0 → true。
bool AttackInfo::isIntendDamage()
{
    if (m_weaponDamageApply != 0 || m_damage > 0.0f || m_absoluteDamage > 0)
    {
        return true;
    }
    return false;
}

// ORIG 0x889079a：param==4(无属性) 清空集合，否则 erase+insert。
void AttackInfo::setElement(ENUM_ELEMENT element)
{
    if (element == ENUM_ELEMENT_NONE)
    {
        m_elements.clear();
    }
    else
    {
        // ORIG 0x88907ba：movl $4,-0x14; erase(-0x14) → 固定删 NONE(4) 再 insert element
        m_elements.erase(ENUM_ELEMENT_NONE);
        m_elements.insert(element);
    }
}

// ORIG 0x809576e：SDC 元素合法性（STComputeDamageVariable* 版本，定义于 SDCLink.cpp）。
bool isValidAttackInfoElement(STComputeDamageVariable* v, int elem);

// ORIG 0x88907fe：对集合中每个元素（除 4）在 STComputeDamageVariable::m_elemEnable
// 置 is-valid 标记，调用 isValidAttackInfoElement(STComputeDamageVariable*, int)。
bool AttackInfo::isValidElement(ENUM_ELEMENT element)
{
    STComputeDamageVariable v;
    for (std::set<ENUM_ELEMENT>::const_iterator it = m_elements.begin();
         it != m_elements.end(); it++)
    {
        ENUM_ELEMENT e = *it;
        if (e == ENUM_ELEMENT_NONE)
            continue;
        v.m_elemEnable[e] = 1;
    }
    return isValidAttackInfoElement(&v, element);
}

// ORIG 0x88908b8：m_attackType(+0x10) = type。
void AttackInfo::setAttackType(ENUM_ATTACKTYPE type)
{
    m_attackType = type;
}

// ORIG 0x88908c6：按 "attack type physical/magical/absolute" 返回 0/1/2。
int AttackInfo::getAttackType(std::string& tag)
{
    if (tag == "attack type physical")
        return 0;
    if (tag == "attack type magical")
        return 1;
    if (tag == "attack type absolute")
        return 2;
    return 7;
}

// ---- 辅助自由函数 ----

// ORIG 0x88bfe5b：字符串→ENUM_ELEMENT（fire/water/dark/light，默认 4）。
int getElement(const char* s)
{
    if (strcmp(s, "[fire element]") == 0) return 0;
    if (strcmp(s, "[water element]") == 0) return 1;
    if (strcmp(s, "[dark element]") == 0) return 2;
    if (strcmp(s, "[light element]") == 0) return 3;
    return 4;
}

// ORIG 0x88be12d：draw layer 字符串→0..7。
int getDrawLayer(const char* s)
{
    if (strcmp(s, "[close]") == 0) return 0;
    if (strcmp(s, "[normal]") == 0) return 1;
    if (strcmp(s, "[bottom]") == 0) return 2;
    if (strcmp(s, "[closeback]") == 0) return 3;
    if (strcmp(s, "[middleback]") == 0) return 4;
    if (strcmp(s, "[distantback]") == 0) return 5;
    if (strcmp(s, "[cover]") == 0) return 6;
    return 7;
}

// ORIG 0x88944fb：active status 字符串→0..0x11，默认 0x12。
int getActiveStatusType(const std::string& s)
{
    static const char* kTag[] = {
        "slow", "freeze", "poison", "stun", "curse", "blind", "lightning",
        "stone", "sleep", "burn", "weapon break", "bleeding", "haste",
        "bless", "element", "confuse", "hold", "armor break",
    };
    static const char* kNoBox[] = {
        "[slow]", "[freeze]", "[poison]", "[stun]", "[curse]", "[blind]",
        "[lightning]", "[stone]", "[sleep]", "[burn]", "[weapon break]",
        "[bleeding]", "[haste]", "[bless]", "[element]", "[confuse]",
        "[hold]", "[armor break]",
    };
    for (int i = 0; i <= 0x11; ++i)
    {
        if (s == kTag[i] || s == kNoBox[i])
            return i;
    }
    return 0x12;
}

// ORIG 0x809576e：SDC 元素合法性（AttackInfo 简化为 4 元素位掩码视图）。
bool isValidAttackInfoElement(const void* sdc, int element)
{
    const char* p = (const char*)sdc;
    if (element == 4)
    {
        return (p[0] == 0 && p[1] == 0 && p[2] == 0 && p[3] == 0);
    }
    return p[element] != 0;
}

// ============================================================================
// importCreatureScript —— ORIG 0x8a35e47（RDARScriptObject.cpp）
// 逐行 ScanType 分派；标签→字段写回。loadRDARScriptFile(g_creatureScriptBaseDirectory,
// path) 装载；末尾以 STScriptFileList::FindIndexByFullScan 写回 m_id。
// ============================================================================
namespace
{
// [skill list] 子域查找表 tagId（ORIG 汇编 0x8a3776f 区 dispatch 树 +
// 12 段 pair 注册的 movl $0xNNN 常量实证）。
enum CreatureSkillTag
{
    CSTAG_NODE_END = 0x25f6,             // [/skill node]
    CSTAG_SKILL_TYPE = 0x260c,           // [skill type]
    CSTAG_SKILL_NAME = 0x2612,           // [skill name]
    CSTAG_SKILL_EXPLAIN = 0x2617,        // [skill explain]
    CSTAG_SKILL_RECOVERY = 0x261c,       // [skill recovery time]
    CSTAG_LEARN_SKILL_LEVEL = 0x2623,    // [learn skill level]
    CSTAG_SKILL_MP = 0x262a,             // [skill mp]
    CSTAG_SKILL_MOTION = 0x2631,         // [skill motion]
    CSTAG_SKILL_UNDER_EFFECT = 0x2636,   // [skill under effect]
    CSTAG_SKILL_OVER_EFFECT = 0x263b,    // [skill over effect]
    CSTAG_SKILL_INFO = 0x2640,           // [skill info]
    CSTAG_ATTACK_INFO = 0x2645,          // [attack info]
};
}  // namespace

bool importCreatureScript(CreatureScript* script, const char* path)
{
    if (!loadRDARScriptFile(g_creatureScriptBaseDirectory, path))
    {
        return false;
    }
    script->clear();
    getDirectoryPath(script->m_directory, "%s%s", g_creatureScriptBaseDirectory, path);

    std::string tag;
    std::string token;
    while (ScanType(tag, true))
    {
        if (tag == "[name]")
        {
            ScanStr(&script->m_name);
        }
        else if (tag == "[character span]")
        {
            script->m_field18 = ScanInt((bool*)0);
        }
        else if (tag == "[width]")
        {
            script->m_field8 = ScanInt((bool*)0);
            script->m_fieldc = ScanInt((bool*)0);
        }
        else if (tag == "[floating height]")
        {
            script->m_field10 = ScanInt((bool*)0);
        }
        else if (tag == "[layer]")
        {
            ScanStr(&token);
            script->m_layer = getDrawLayer(token.c_str());
        }
        else if (tag == "[skill recovery time]")
        {
            script->m_field1c = ScanInt((bool*)0);
        }
        else if (tag == "[over skill recovery time]")
        {
            script->m_field20 = ScanInt((bool*)0);
        }
        else if (tag == "[direction correct]")
        {
            script->m_directionCorrect = (ScanInt((bool*)0) != 0) ? 1 : 0;
        }
        else if (tag == "[move speed]")
        {
            // ORIG 0x8a360d2..0x8a3611a：五段独立调用点（非线性循环）。
            script->m_moveSpeed[0] = ScanInt((bool*)0);
            script->m_moveSpeed[1] = ScanInt((bool*)0);
            script->m_moveSpeed[2] = ScanInt((bool*)0);
            script->m_moveSpeed[3] = ScanInt((bool*)0);
            script->m_moveSpeed[4] = ScanInt((bool*)0);
        }
        else if (tag == "[gravity]")
        {
            script->m_gravity = ScanInt((bool*)0);
        }
        else if (tag == "[start level]")
        {
            script->m_startLevel = ScanInt((bool*)0);
        }
        else if (tag == "[permission level]")
        {
            script->m_permissionLevel = ScanInt((bool*)0);
        }
        else if (tag == "[max level]")
        {
            script->m_maxLevel = ScanInt((bool*)0);
        }
        else if (tag == "[parent max level]")
        {
            script->m_parentMaxLevel = ScanInt((bool*)0);
        }
        else if (tag == "[artifact slot]")
        {
            while (ScanStr(&token))
            {
                if (token == "red")
                    script->m_artifactSlotRed = 1;
                else if (token == "blue")
                    script->m_artifactSlotBlue = 1;
                else if (token == "green")
                    script->m_artifactSlotGreen = 1;
            }
        }
        else if (tag == "[learn overskill level]")
        {
            script->m_learnOverSkillLevel = ScanInt((bool*)0);
        }
        else if (tag == "[skill MP]")
        {
            script->m_skillMP = ScanInt((bool*)0);
        }
        else if (tag == "[over skill MP]")
        {
            script->m_overSkillMP = ScanInt((bool*)0);
        }
        else if (tag == "[skill type]")
        {
            ScanStr(&token);
            if (token == "[active]")
                script->m_skillType = 0;
            else if (token == "[passive]")
                script->m_skillType = 1;
        }
        else if (tag == "[over skill type]")
        {
            ScanStr(&token);
            if (token == "[active]")
                script->m_overSkillType = 0;
            else if (token == "[passive]")
                script->m_overSkillType = 1;
        }
        else if (tag == "[basic motion]")
        {
            ScanStr(&script->m_basicMotion);
        }
        else if (tag == "[walk motion]")
        {
            ScanStr(&script->m_walkMotion);
        }
        else if (tag == "[run motion]")
        {
            ScanStr(&script->m_runMotion);
        }
        else if (tag == "[fly motion]")
        {
            ScanStr(&script->m_flyMotion);
        }
        else if (tag == "[get exp motion]")
        {
            ScanStr(&script->m_getExpMotion);
        }
        else if (tag == "[get exp over motion]")
        {
            ScanStr(&script->m_getExpOverMotion);
        }
        else if (tag == "[get exp under motion]")
        {
            ScanStr(&script->m_getExpUnderMotion);
        }
        else if (tag == "[skill motion]")
        {
            ScanStr(&script->m_skillMotion);
        }
        else if (tag == "[over skill motion]")
        {
            ScanStr(&script->m_overSkillMotion);
        }
        else if (tag == "[response motion]")
        {
            ScanStr(&script->m_responseMotion);
        }
        else if (tag == "[special motion]")
        {
            ScanStr(&script->m_specialMotion);
        }
        else if (tag == "[equip motion]")
        {
            ScanStr(&script->m_equipMotion);
        }
        else if (tag == "[equip over motion]")
        {
            ScanStr(&script->m_equipOverMotion);
        }
        else if (tag == "[equip under motion]")
        {
            ScanStr(&script->m_equipUnderMotion);
        }
        else if (tag == "[basic under effect]")
        {
            ScanStr(&script->m_basicUnderEffect);
        }
        else if (tag == "[walk under effect]")
        {
            ScanStr(&script->m_walkUnderEffect);
        }
        else if (tag == "[run under effect]")
        {
            ScanStr(&script->m_runUnderEffect);
        }
        else if (tag == "[skill under effect]")
        {
            ScanStr(&script->m_skillUnderEffect);
        }
        else if (tag == "[over skill under effect]")
        {
            ScanStr(&script->m_overSkillUnderEffect);
        }
        else if (tag == "[response under effect]")
        {
            ScanStr(&script->m_responseUnderEffect);
        }
        else if (tag == "[special under effect]")
        {
            ScanStr(&script->m_specialUnderEffect);
        }
        else if (tag == "[basic over effect]")
        {
            ScanStr(&script->m_basicOverEffect);
        }
        else if (tag == "[walk over effect]")
        {
            ScanStr(&script->m_walkOverEffect);
        }
        else if (tag == "[run over effect]")
        {
            ScanStr(&script->m_runOverEffect);
        }
        else if (tag == "[skill over effect]")
        {
            ScanStr(&script->m_skillOverEffect);
        }
        else if (tag == "[over skill over effect]")
        {
            ScanStr(&script->m_overSkillOverEffect);
        }
        else if (tag == "[response over effect]")
        {
            ScanStr(&script->m_responseOverEffect);
        }
        else if (tag == "[special over effect]")
        {
            ScanStr(&script->m_specialOverEffect);
        }
        else if (tag == "[additional animation]")
        {
            // ORIG 0x08a36991：-0x40 常量 -1（本 case 内无任何写回，cmp 恒
            // 不等分支不可达——ORIG 死代码，照录）；两 string 均复用 token
            // 槽（-0x228）。命中时临时对象 materialize（-0x220）→
            // vector<CAA>::push_back(T&&)（EO 0x8a40a76）→ dtor。
            int animField0 = -1;
            ScanStr(&token);
            int animField4 = ScanInt((bool*)0);
            ScanStr(&token);
            if (animField0 != -1)
            {
                script->m_additionalAnimations.push_back(
                    CreatureAdditionalAnimation(animField4, animField0, token.c_str()));
            }
        }
        else if (tag == "[default particle]")
        {
            ScanStr(&script->m_defaultParticle);
        }
        else if (tag == "[response sound tag]")
        {
            ScanStr(&script->m_responseSoundTag);
        }
        else if (tag == "[wing sound tag]")
        {
            ScanStr(&script->m_wingSoundTag);
        }
        else if (tag == "[walk sound tag]")
        {
            ScanStr(&script->m_walkSoundTag);
        }
        else if (tag == "[run sound tag]")
        {
            ScanStr(&script->m_runSoundTag);
        }
        else if (tag == "[revision x]")
        {
            script->m_revisionX = ScanInt((bool*)0);
        }
        else if (tag == "[revision y]")
        {
            script->m_revisionY = ScanInt((bool*)0);
        }
        else if (tag == "[default random motion rate]")
        {
            script->m_defaultRandomMotionRate = ScanInt((bool*)0);
        }
        else if (tag == "[increase random motion rate]")
        {
            script->m_increaseRandomMotionRate = ScanInt((bool*)0);
        }
        else if (tag == "[etc motion]")
        {
            script->m_etcMotion.clear();
            while (ScanStr(&token))
                script->m_etcMotion.push_back(token);
        }
        else if (tag == "[random motion]")
        {
            script->m_randomMotion.clear();
            while (ScanStr(&token))
                script->m_randomMotion.push_back(token);
        }
        else if (tag == "[attack info]")
        {
            script->m_attackInfo.clear();
            while (ScanStr(&token))
                script->m_attackInfo.push_back(token);
        }
        else if (tag == "[skill string]")
        {
            script->m_skillString.clear();
            while (ScanStr(&token))
                script->m_skillString.push_back(token);
        }
        else if (tag == "[skill level values]")
        {
            script->m_skillLevelValues.clear();
            bool ok = false;                       // ORIG 循环头单调用点
            int v;
            for (;;)
            {
                v = ScanInt(&ok);
                if (ok != true)
                    break;
                script->m_skillLevelValues.push_back(v);
            }
        }
        else if (tag == "[over skill level values]")
        {
            script->m_overSkillLevelValues.clear();
            bool ok = false;                       // ORIG 循环头单调用点
            int v;
            for (;;)
            {
                v = ScanInt(&ok);
                if (ok != true)
                    break;
                script->m_overSkillLevelValues.push_back(v);
            }
        }
        else if (tag == "[piercing]")
        {
            script->m_piercing.clear();
            bool ok = false;                       // ORIG 循环头单调用点
            int v;
            for (;;)
            {
                v = ScanInt(&ok);
                if (ok != true)
                    break;
                script->m_piercing.push_back(v);
            }
        }
        else if (tag == "[int data]")
        {
            script->m_intData.clear();
            bool ok = false;                       // ORIG 循环头单调用点
            int v;
            for (;;)
            {
                v = ScanInt(&ok);
                if (ok != true)
                    break;
                script->m_intData.push_back(v);
            }
        }
        else if (tag == "[string data]")
        {
            script->m_stringData.clear();
            while (ScanStr(&token))
                script->m_stringData.push_back(token);
        }
        else if (tag == "[evolution creature multy id]")
        {
            script->m_evolutionMultyId.clear();
            int count = ScanInt((bool*)0);
            for (int i = 0; i < count; ++i)
            {
                bool ok = false;
                int v = ScanInt(&ok);
                if (!ok)
                    break;
                // ORIG 0x8a37019：先 materialize prvalue 临时（-0x214）再
                // push_back(int&&)（EO 0x0808e1ee）——写 int(v) 复现。
                script->m_evolutionMultyId.push_back(int(v));
            }
        }
        else if (tag == "[evolution quest]")
        {
            script->m_evolutionQuestId = ScanInt((bool*)0);
        }
        else if (tag == "[evolution creature id]")
        {
            script->m_evolutionCreatureId = ScanInt((bool*)0);
        }
        else if (tag == "[evolution level]")
        {
            script->m_evolutionLevel = ScanInt((bool*)0);
        }
        else if (tag == "[selective grow creature]")
        {
            // ORIG 0x8a3710d：数据先写栈上 selectiveGrowCreatureInfo（-0x24c，
            // ctor 0x0833df50），仅命中闭合标签时整体 7 字拷回 script+0x18c；
            // 文件截断（ScanType 失败）直接回主循环、不落盘。
            selectiveGrowCreatureInfo grow;
            bool growActive = true;
            while (growActive)
            {
                if (ScanType(tag, true) != 1)
                    break;
                if (tag == "[/selective grow creature]")
                {
                    script->m_selectiveGrow = grow;
                    growActive = false;
                }
                else if (tag == "[item count for levelup]")
                    grow.m_growItemCount = ScanInt((bool*)0);
                else if (tag == "[using exp item]")
                    grow.m_usingExpItem = (ScanInt((bool*)0) != 0) ? 1 : 0;
                else if (tag == "[grow Atype item index]")
                    grow.m_growATypeItem = ScanInt((bool*)0);
                else if (tag == "[grow Btype item index]")
                    grow.m_growBTypeItem = ScanInt((bool*)0);
                else if (tag == "[grow Atype creature id]")
                    grow.m_growATypeCreature = ScanInt((bool*)0);
                else if (tag == "[grow Btype creature id]")
                    grow.m_growBTypeCreature = ScanInt((bool*)0);
                else if (tag == "[grow level]")
                    grow.m_growLevel = ScanInt((bool*)0);
            }
        }
        else if (tag == "[event evolute creature]")
        {
            script->m_eventEvlouteCreature.clear();
            bool ok = false;                       // ORIG 循环头单调用点
            int v;
            for (;;)
            {
                v = ScanInt(&ok);
                if (ok != true)
                    break;
                script->m_eventEvlouteCreature.push_back(v);
            }
        }
        else if (tag == "[item evolution]")
        {
            // ORIG 0x8a373a2：首 int 失败 → log 0x259e → return false；
            // 循环内 first(-0x25c)/second(-0x258) 相邻 = st_crature_id_item_id
            // 两字段直写栈；first 读尽 → log 0x25a9(size) 后走收尾；
            // second 缺 → log 0x25af(size) → return false；first==-1 → 走收尾。
            bool ok = false;
            int itemId = ScanInt(&ok);
            if (ok != true)
            {
                LogManager::logFormat(0, "../RDARScriptObject.cpp", __FUNCTION__,
                                      0x259e,
                                      "creature script loading  %s, evolution_item_id_",
                                      path);
                return false;
            }
            script->m_itemEvolution = itemId;
            script->m_itemEvolCreatureIds.clear();
            st_crature_id_item_id entry;
            for (;;)
            {
                entry.m_creatureId = ScanInt(&ok);
                if (ok != true)
                {
                    LogManager::logFormat(0, "../RDARScriptObject.cpp", __FUNCTION__,
                                          0x25a9,
                                          "creature script loading  %s, item_evolution_creature_id_ size : %d",
                                          path, script->m_itemEvolCreatureIds.size());
                    break;
                }
                entry.m_itemId = ScanInt(&ok);
                if (ok != true)
                {
                    LogManager::logFormat(0, "../RDARScriptObject.cpp", __FUNCTION__,
                                          0x25af,
                                          "creature script loading  %s, item_evolution_creature_id_ size : %d",
                                          path, script->m_itemEvolCreatureIds.size());
                    return false;
                }
                if (entry.m_creatureId == -1)
                    break;
                script->m_itemEvolCreatureIds.push_back(entry);
            }
            // ORIG 0x8a3754b：收尾多消费一行——EOF/闭合标签 → 函数收束；
            // 否则回主循环头（该行被重扫覆盖，ORIG 固有 quirk，照录）。
            if (ScanType(tag, true) != 1)
                break;
            if (tag == "[/item evolution]")
                break;
            continue;
        }
        else if (tag == "[skill info]")
        {
            ScanStr(&token);
            script->m_skillInfo = token;
        }
        else if (tag == "[over skill info]")
        {
            ScanStr(&token);
            script->m_overSkillInfo = token;
        }
        else if (tag == "[skill list]")
        {
            // ORIG 0x8a3776f：函数局部静态三件套（_ZZ..importCreatureScript..
            // __lookup/__init_lock 各带 __cxa_guard+__cxa_atexit；__inited 为
            // 零初始化 plain bool 无 guard）。子标签经 map<const char*,int,
            // less_TCHAR> 查找分派（非线性 string==）；!__inited 时首行进
            // 注册链：case 0 起逐 case insert 后贯穿（0x8a37a6b→0x8a37d16→
            // …→0x8a38219），公共尾 leave+__inited=1 再重查同一行。
            typedef std::map<const char*, int, less_TCHAR> LookupMap;
            static LookupMap __lookup;
            static SpinLock __init_lock;
            static bool __inited = false;

            bool listActive = true;              // -0x25
            while (listActive)
            {
                if (ScanType(tag, true) != 1)
                    break;                       // 0x8a37664 → 主循环头
                int key = -1;                    // -0x260
                int typeFlag = -1;               // -0x24（0xffffffff 初值）
                CreatureScriptSkillInfo info;    // -0x28c（0x8a3dd26 每轮构造）
                if (tag == "[/skill list]")
                {
                    listActive = false;          // 0x8a376a2
                    continue;                    // 0x8a3824d → info dtor → 尾检
                }
                if (tag == "[skill node]")      // 未知行 → 0x8a3824d 跳过（ORIG
                {                               // 为两次 == 直链，非 !=）
                key = -1;                        // 0x8a376c9
                info.clear();                    // 0x8a3de50
                bool ok = false;                 // -0x22d（ORIG 全函数共用槽）
                key = ScanInt(&ok);
                if (ok != true)
                {
                    LogManager::logFormat(0, "../RDARScriptObject.cpp", __FUNCTION__,
                                          0x25ea,
                                          "creature script loading  %s, evolution_item_id_",
                                          path);
                    return false;                // 0x8a37731
                }
                bool nodeActive = true;          // -0x1d
                for (;;)
                {
                    if (ScanType(tag, true) != 1)
                        break;                   // 0x8a37769：截断 → 本 node 不 insert
                redo_lookup:
                    int tagId;
                    if (__inited)
                    {
                        LookupMap::const_iterator it = __lookup.find(tag.c_str());
                        if (it != __lookup.end())
                            tagId = it->second;
                        else
                            tagId = -1;          // 0x8a37925 未命中
                    }
                    else
                    {
                        __init_lock.enter();     // 0x8a3792f
                        if (__inited)            // 竞争双检：释放后重查本行
                        {
                            __init_lock.leave(); // 0x8a3794d
                            goto redo_lookup;    // 0x8a37953 → 0x8a3776f
                        }
                        tagId = 0;               // 0x8a37958 首行注册入口
                    }
                    switch (tagId)
                    {
                    case 0:                      // 与 [/skill node] 共体
                    case CSTAG_NODE_END:
                        if (__inited)
                        {
                            nodeActive = false;  // 0x8a37a73
                            if (key == -1 || typeFlag == -1)
                            {
                                LogManager::logFormat(0, "../RDARScriptObject.cpp",
                                                      __FUNCTION__, 0x25fc,
                                                      "creature script loading  %s, evolution_item_id_",
                                                      path);
                                return false;    // 0x8a37a88
                            }
                            if (typeFlag == 0)
                            {
                                // ORIG 0x8a37ad4：find==end 才 insert（弱实例
                                // 0x8a40b38/b0c/b5e/b72/d2c/d66 序列）。
                                if (script->m_skillList.find(key) == script->m_skillList.end())
                                    script->m_skillList.insert(
                                        std::make_pair<int, CreatureScriptSkillInfo>(key, info));
                            }
                            else
                            {
                                if (script->m_overSkillList.find(key) == script->m_overSkillList.end())
                                    script->m_overSkillList.insert(
                                        std::make_pair<int, CreatureScriptSkillInfo>(key, info));
                            }
                            break;
                        }
                        __lookup.insert(LookupMap::value_type("[/skill node]", static_cast<int>(CSTAG_NODE_END)));
                        /* fall through（0x8a37a6b jmp 0x8a37d16） */
                    case CSTAG_SKILL_TYPE:
                        if (__inited)
                        {
                            ScanStr(&token);     // 0x8a37d73（无括号 "over skill"）
                            typeFlag = (token == "over skill") ? 1 : 0;
                            break;
                        }
                        __lookup.insert(LookupMap::value_type("[skill type]", static_cast<int>(CSTAG_SKILL_TYPE)));
                        /* fall through */
                    case CSTAG_SKILL_NAME:
                        if (__inited)
                        {
                            ScanStr(&info.m_str0);
                            break;
                        }
                        __lookup.insert(LookupMap::value_type("[skill name]", static_cast<int>(CSTAG_SKILL_NAME)));
                        /* fall through */
                    case CSTAG_SKILL_EXPLAIN:
                        if (__inited)
                        {
                            ScanStr(&info.m_str4);
                            break;
                        }
                        __lookup.insert(LookupMap::value_type("[skill explain]", static_cast<int>(CSTAG_SKILL_EXPLAIN)));
                        /* fall through */
                    case CSTAG_SKILL_RECOVERY:
                        if (__inited)
                        {
                            info.m_field8 = ScanInt(&ok);   // 0x8a37eef（&-0x22d）
                            break;
                        }
                        __lookup.insert(LookupMap::value_type("[skill recovery time]", static_cast<int>(CSTAG_SKILL_RECOVERY)));
                        /* fall through */
                    case CSTAG_LEARN_SKILL_LEVEL:
                        if (__inited)
                        {
                            info.m_fieldc = ScanInt(&ok);   // 0x8a37f6e
                            break;
                        }
                        __lookup.insert(LookupMap::value_type("[learn skill level]", static_cast<int>(CSTAG_LEARN_SKILL_LEVEL)));
                        /* fall through */
                    case CSTAG_SKILL_MP:
                        if (__inited)
                        {
                            info.m_field10 = ScanInt(&ok);  // 0x8a37fed（+0x10 int）
                            break;
                        }
                        __lookup.insert(LookupMap::value_type("[skill mp]", static_cast<int>(CSTAG_SKILL_MP)));
                        /* fall through */
                    case CSTAG_SKILL_MOTION:
                        if (__inited)
                        {
                            ScanStr(&info.m_str14);
                            break;
                        }
                        __lookup.insert(LookupMap::value_type("[skill motion]", static_cast<int>(CSTAG_SKILL_MOTION)));
                        /* fall through */
                    case CSTAG_SKILL_UNDER_EFFECT:
                        if (__inited)
                        {
                            ScanStr(&info.m_str18);
                            break;
                        }
                        __lookup.insert(LookupMap::value_type("[skill under effect]", static_cast<int>(CSTAG_SKILL_UNDER_EFFECT)));
                        /* fall through */
                    case CSTAG_SKILL_OVER_EFFECT:
                        if (__inited)
                        {
                            ScanStr(&info.m_str1c);
                            break;
                        }
                        __lookup.insert(LookupMap::value_type("[skill over effect]", static_cast<int>(CSTAG_SKILL_OVER_EFFECT)));
                        /* fall through */
                    case CSTAG_SKILL_INFO:
                        if (__inited)
                        {
                            ScanStr(&info.m_str20);
                            break;
                        }
                        __lookup.insert(LookupMap::value_type("[skill info]", static_cast<int>(CSTAG_SKILL_INFO)));
                        /* fall through */
                    case CSTAG_ATTACK_INFO:
                        if (__inited)
                        {
                            ScanStr(&info.m_str24);         // 0x8a38202（漏项补全）
                            break;
                        }
                        __lookup.insert(LookupMap::value_type("[attack info]", static_cast<int>(CSTAG_ATTACK_INFO)));
                        break;                               // 注册链终点 → 公共尾
                    default:
                        // -1 未命中/未处理 → ORIG 无操作直落公共尾。
                        break;
                    }
                    // 公共尾 0x8a38219。
                    if (!__inited)
                    {
                        __init_lock.leave();     // 0x8a38227
                        __inited = true;         // 0x8a38233
                        goto redo_lookup;        // 0x8a3823a 重查同一行
                    }
                    if (!nodeActive)
                        break;                   // 0x8a38240 → info dtor → 外层
                }
                }                                  // [skill node] 块结束
            }
        }
        else if (tag == "[using random skill]")
        {
            bool ok = false;
            int v = ScanInt(&ok);
            if (ok != true)
            {
                LogManager::logFormat(0, "../RDARScriptObject.cpp", __FUNCTION__,
                                      0x266c,
                                      "creature script loading  %s, evolution_item_id_",
                                      path);
                return false;
            }
            script->m_usingRandomSkill = (v != 0) ? 1 : 0;
        }
        else
        {
            LogManager::logFormat(0, "../RDARScriptObject.cpp", __FUNCTION__, 0x2674,
                                  "creature script loading error : %s : %s",
                                  tag.data(), path);   // 0x8a3832f = string::data
            return false;
        }
    }

    // 末尾 0x8a38376：STScriptFileList::FindIndexByFullScan(this=&列表, path)
    // 写回 m_id 后 return true（g_creatureScriptFileList 建模为 map，布局与
    // STScriptFileList +0x00 一致 → 类型视图调用，同 InitScriptProducers_5 先例）。
    script->m_id = reinterpret_cast<const STScriptFileList*>(&g_creatureScriptFileList)
                       ->FindIndexByFullScan(path);
    return true;
}

// ============================================================================
// ImportAttackInfoScript —— ORIG 0x88d145a（RDARScriptAttackInfo.cpp）
// ============================================================================
// 注意:此函数与 importCreatureScript 的逐行 if-else 直链不同,ORIG 汇编
// (/tmp/ImportAttackInfoScript.md 汇编 0x1577 区 + Ghidra C)为:
//   static map<const char*,int,less_TCHAR> __lookup(函数局部,guard 构造)
//   + SpinLock __init_lock + bool __inited;
//   注册在 switch 内逐 case 完成:!__inited 时每个 case 构造
//   pair(key,tagId) 并 map::insert 后贯穿到下一 case(0x42→0x46→…→0x1d2 链,
//   末 case insert 后跳公共尾 0x24bb),尾处 leave + __inited=1 并重查同一
//   token 做真实解析;之后每行 find→switch。首行 tagId=0 与 case 0x42 共享
//   入口。每行处理前先调 processModuleTagSet:返回 false 表示该行
//   是 module 开/闭边界行(已被其内部 ScanType 消费),跳过本行数据解析;第二
//   输出(subId)!= -1 时写 AttackInfo::m_moduleTag(+0x8c)。
// module 化字段:[elemental property](+0xa4 set)与 [active status] 各自持有
// module 名缓存 string(初值 MODULE_NAME_TAG+0x18 = g_moduleNameTag[6],与
// clear() 的 m_moduleTag=3 对齐);遇 module 变化分别 clear m_elements / 清零
// active status 的 int3 槽(0xec..0xf4)与 3 个 vector(0xf8/0x104/0x110)。
// ORIG 0x88d1f95 的重置循环不清 type/value/int/int2 槽,也不重置 statusIdx。
// [active status] 槽计数 statusIdx(ORIG -0x34)只在函数入口清零:每个文件至多
// 3 个 status;超限 log 后 ORIG 0x88d20b9 跳 0x88d393d 重查同一 token(数据非法
// 时会循环),此处以内层 continue 同构重建。
// [human active status rate] 除以 _DAT_08e0ab18 = 100.0f(原 ELF 0x08e0ab18
// 实测 4 字节 0x42c80000)。
namespace
{
enum AttackInfoTag
{
    AITAG_DAMAGE = 0x42,                     // [damage]
    AITAG_DAMAGE_INCREASE_RATE = 0x46,       // [damage increase rate]
    AITAG_ABSOLUTE_DAMAGE = 0x4a,            // [absolute damage]
    AITAG_DAMAGE_BONUS = 0x4e,               // [damage bonus]
    AITAG_ATTACK_TYPE = 0x52,                // [attack type]
    AITAG_WEAPON_DAMAGE_APPLY = 0x63,        // [weapon damage apply]（99）
    AITAG_ATTACK_MYSELF = 0x67,              // [attack myself]
    AITAG_ATTACK_FRIEND = 0x6b,              // [attack friend]
    AITAG_ATTACK_ENEMY = 0x6f,               // [attack enemy]
    AITAG_ELEMENTAL_PROPERTY = 0x73,         // [elemental property]
    AITAG_ACTIVE_STATUS = 0x86,              // [active status]
    AITAG_DAMAGE_REACTION = 0xe2,            // [damage reaction]
    AITAG_PUSH_ASIDE = 0xf6,                 // [push aside]
    AITAG_LIFT_UP = 0xfa,                    // [lift up]
    AITAG_CRITICAL_HIT = 0xfe,               // [critical hit]
    AITAG_STUCK = 0x102,                     // [stuck]
    AITAG_STUCK_PROB_INCREASE = 0x106,       // [stuck prob increase]
    AITAG_KNUCK_BACK = 0x10a,                // [knuck back]
    AITAG_WEAKNESS = 0x11b,                  // [weakness]
    AITAG_BOUNCE = 0x120,                    // [bounce]
    AITAG_IGNORE_DEFENSE = 0x124,            // [ignore defense]
    AITAG_HUMAN_DAMAGE_RATE = 0x128,         // [human damage rate]
    AITAG_FORT_DAMAGE_RATE = 0x12c,          // [fort damage rate]（300）
    AITAG_MONSTER_DAMAGE_RATE = 0x130,       // [monster damage rate]
    AITAG_HP_DRAIN = 0x134,                  // [HP drain]
    AITAG_MP_DRAIN = 0x13d,                  // [MP drain]
    AITAG_HIT_WAV = 0x146,                   // [hit wav]
    AITAG_ATTACK_DIRECTION = 0x14a,          // [attack direction]
    AITAG_HIT_INFO = 0x15b,                  // [hit info]
    AITAG_IS_USE_WEAPON_HIT_DELAY_RATE = 0x17d,  // [is use weapon hit delay rate]
    AITAG_FORCE_HIT_STUN_TIME = 0x181,       // [force hit stun time]
    AITAG_HIT_DIRECTION = 0x186,             // [hit direction]
    AITAG_IGNORE_SUPER_ARMOR = 0x19b,        // [ignore super armor]
    AITAG_IGNORE_WEIGHT = 0x19f,             // [ignore weight]
    AITAG_HUMAN_ACTIVE_STATUS_RATE = 0x1a3,  // [human active status rate]
    AITAG_IS_HIT_NUMBER_INCREASING = 0x1aa,  // [is hit number increasing]
    AITAG_CATCH_OBJECT = 0x1af,              // [catch object]
    AITAG_MAX_TARGET_HIT_NUMBER = 0x1c6,     // [max target hit number]
    AITAG_IGNORE_DIEHARD = 0x1cd,            // [ignore diehard]
    AITAG_HIT_CHECK_BY_SCREEN_POS = 0x1d2,   // [hit check by screen pos]
};

// ORIG 全局实证同 ImportSkillScript:map<const char*,int> 全部用 less_TCHAR
//(strcmp 内容比较);find(tag.c_str()) 传动态指针,默认 less 会恒 miss。
// 查找表本体为函数局部静态(__lookup/__init_lock/__inited),见下方函数。

}  // namespace

// ORIG 0x88d145a:ImportAttackInfoScript(AttackInfo*, char const*, int)
// 注册链与解析共用同一 switch：ORIG 每个 case 头部 `if (!__inited)` 则
// 构造 pair(key, tagId) 并 map::insert 后**贯穿(fall through)到下一 case**
// 继续注册，形成 0x42→…→0x1d2 的 40 段直链（汇编各 insert 尾 `jmp <next
// case>`）；已注册时执行真实解析并 break 至公共尾(0x24bb)。首行 tagId=0
// 与 case 0x42 共享入口（dispatch 树 `test %eax,%eax; je 0x19ca`）。
bool ImportAttackInfoScript(AttackInfo* info, const char* path, int param3)
{
    if (!loadRDARScriptFile("", path))        // 0x1a:失败 ebx=0 → return false
    {
        return false;
    }
    info->clear();

    std::string tag;                          // -0x3b4 主行标签(ScanType 读入)
    std::string token;                        // -0x3b8 子 token(ScanTypeOrStr 读入)
    int statusIdx = 0;                        // -0x34 文件级 active status 槽计数
    // module 名缓存(元素集 / active status 各一):初值 MODULE_NAME_TAG+0x18 =
    // g_moduleNameTag[3][0],与 clear() 的 m_moduleTag=3 对齐 → 首遇不误重置。
    std::string elemModule(g_moduleNameTag[3][0]);     // -0x3bc
    std::string statusModule(g_moduleNameTag[3][0]);   // -0x3c0

    for (;;)                                  // 行循环:读行 → module 过滤 → 处理
    {
        if (!ScanType(tag, true))             // 0x14e4;失败 → 0x3947 unload + ret 1
            break;
        int moduleA = -1;                     // 0x1505/-0x150f 两输出均初 -1
        int moduleB = -1;
        bool isModuleLine = processModuleTagSet(moduleA, moduleB, &tag, param3, false);
        if (moduleB != -1)                    // 0x10d:仅第二输出写 +0x8c
            info->m_moduleTag = moduleB;
        if (isModuleLine)                     // 0x1572:false=已消费 → 读下一行
        {
            // ORIG 函数局部静态三件套(各带 __cxa_guard;位于 0x11d 重查点)。
            // map<const char*,int> 键比较器用 less_TCHAR(大小写无关 strcmp 语义,
            // 工程内既有类型);find(tag.c_str()) 传动态指针,默认 less 会恒 miss。
            typedef std::map<const char*, int, less_TCHAR> LookupMap;
            static LookupMap __lookup;
            static SpinLock __init_lock;
            static bool __inited = false;

            for (;;)                          // 单行处理(0x1577 区;注册后同 tag 重入)
            {
                int tagId;
                if (__inited)
                {
                    LookupMap::const_iterator it = __lookup.find(tag.c_str());
                    if (it != __lookup.end())
                        tagId = it->second;
                    else
                        tagId = -1;
                }
                else
                {
                    __init_lock.enter();      // 0x2d9
                    if (__inited)             // 竞争双检:已就绪 → 释放并重查本行
                    {
                        __init_lock.leave();
                        continue;             // 0x2fc → 0x24e3 → 0x1577
                    }
                    tagId = 0;                // 0x301 首行 → 注册链入口
                }

                switch (tagId)
                {
                case 0:                       // 注册链入口(与 [damage] 共体)
                case AITAG_DAMAGE:            // +0x00 float
                    if (__inited)
                    {
                        info->m_damage = ScanFloat((bool*)0);
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[damage]", static_cast<int>(AITAG_DAMAGE)));
                    /* fall through（ORIG 0x5c8 jmp 0x5e3） */
                case AITAG_DAMAGE_INCREASE_RATE:  // +0x04 float
                    if (__inited)
                    {
                        info->m_damageIncreaseRate = ScanFloat((bool*)0);
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[damage increase rate]",
                                                          static_cast<int>(AITAG_DAMAGE_INCREASE_RATE)));
                case AITAG_ABSOLUTE_DAMAGE:   // +0x08 int
                    if (__inited)
                    {
                        info->m_absoluteDamage = ScanInt((bool*)0);
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[absolute damage]",
                                                          static_cast<int>(AITAG_ABSOLUTE_DAMAGE)));
                case AITAG_DAMAGE_BONUS:      // +0x0c int
                    if (__inited)
                    {
                        info->m_damageBonus = ScanInt((bool*)0);
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[damage bonus]",
                                                          static_cast<int>(AITAG_DAMAGE_BONUS)));
                case AITAG_ATTACK_TYPE:       // +0x10 枚举
                    if (__inited)
                    {
                        bool isType;          // -0x3cd
                        ScanTypeOrStr(isType, token, true);
                        if (token == "[physic]")
                            info->m_attackType = 0;
                        else if (token == "[magic]")
                            info->m_attackType = 1;
                        else if (token == "[ignore defense]")
                            info->m_attackType = 2;
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[attack type]",
                                                          static_cast<int>(AITAG_ATTACK_TYPE)));
                case AITAG_WEAPON_DAMAGE_APPLY:  // +0x14 bool(int)
                    if (__inited)
                    {
                        info->m_weaponDamageApply = (ScanInt((bool*)0) != 0);
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[weapon damage apply]",
                                                          static_cast<int>(AITAG_WEAPON_DAMAGE_APPLY)));
                case AITAG_ATTACK_MYSELF:     // +0x98 bool(int)
                    if (__inited)
                    {
                        info->m_attackFlags[0] = (ScanInt((bool*)0) != 0);
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[attack myself]",
                                                          static_cast<int>(AITAG_ATTACK_MYSELF)));
                case AITAG_ATTACK_FRIEND:     // +0x9c bool(int)
                    if (__inited)
                    {
                        info->m_attackFlags[1] = (ScanInt((bool*)0) != 0);
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[attack friend]",
                                                          static_cast<int>(AITAG_ATTACK_FRIEND)));
                case AITAG_ATTACK_ENEMY:      // +0xa0 bool(int)
                    if (__inited)
                    {
                        info->m_attackFlags[2] = (ScanInt((bool*)0) != 0);
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[attack enemy]",
                                                          static_cast<int>(AITAG_ATTACK_ENEMY)));
                case AITAG_ELEMENTAL_PROPERTY:  // +0xa4 set<ENUM_ELEMENT>
                    if (__inited)
                    {
                        // module 变化时清空元素集(0x0aa0..0x0ac9)
                        const std::string& curMod = g_moduleNameTag[info->m_moduleTag][0];
                        if (elemModule != curMod)
                            info->m_elements.clear();
                        elemModule = curMod;  // 0x0aec 无条件更新缓存
                        bool isType;          // -0x3ce
                        ScanTypeOrStr(isType, token, true);
                        info->setElement((ENUM_ELEMENT)getElement(token.c_str()));
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[elemental property]",
                                                          static_cast<int>(AITAG_ELEMENTAL_PROPERTY)));
                case AITAG_ACTIVE_STATUS:     // 0xbc..0xf4 + vectors
                    if (__inited)
                    {
                        const std::string& curMod = g_moduleNameTag[info->m_moduleTag][0];
                        if (statusModule != curMod)   // 0x0bb9 module 变化:重置槽
                        {
                            for (int i = 0; i < 3; ++i)
                            {
                                info->m_activeStatusInt3[i] = 0;          // 0xec+4i
                                info->m_activeStatusVectors[i].clear();   // 0xf8+12i
                            }
                        }
                        statusModule = curMod;        // 0x0c28 无条件更新缓存
                        if (statusIdx > 2)            // 0x0c2d 槽满
                        {
                            LogManager::logFormat(0, "../RDARScriptAttackInfo.cpp",
                                                  __FUNCTION__, 0x9c,
                                                  ".atk \xbf\xa1 [active status] \xb0\xb9\xbc\xf6 \xc3\xca\xb0\xfa");
                            continue;                 // 0x0c5f → 0x24e3 重查本行
                        }
                        bool isType;                  // -0x3cf
                        ScanTypeOrStr(isType, token, true);     // 0x0c7f
                        // 逐访问重读 statusIdx(P38:ORIG 每写前 mov -0x34)
                        info->m_activeStatusType[statusIdx] =
                            getActiveStatusType(token);          // 0xbc+4i
                        info->m_activeStatusValue[statusIdx] =
                            ScanFloat((bool*)0);                 // 0xc8+4i
                        info->m_activeStatusInt[statusIdx] =
                            ScanInt((bool*)0);                   // 0xd4+4i
                        info->m_activeStatusInt3[statusIdx] =
                            ScanInt((bool*)0);                   // 0xec+4i
                        // 按状态名追加向量初值(0x0d04..0x1308 比较直链)
                        if (token == "[blind]")
                        {
                            info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                            info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                        }
                        else if (token == "[slow]")
                        {
                            info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                            info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                        }
                        else if (token == "[haste]")
                        {
                            info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                            info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                        }
                        else if (token == "[stone]")
                        {
                            info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                        }
                        else if (token == "[confuse]")
                        {
                            info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                        }
                        else if (token == "[armor break]")
                        {
                            info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                        }
                        else if (token == "[weapon break]")
                        {
                            info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                        }
                        else if (token == "[poison]")
                        {
                            info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                        }
                        else if (token == "[lightning]")
                        {
                            info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                        }
                        else if (token == "[bleeding]")
                        {
                            info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                        }
                        else if (token == "[burn]")       // 0x111f..0x1229:4 次显式
                        {
                            info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                            info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                            info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                            info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                        }
                        else if (token == "[bless]")      // 0x1244:循环 4 次
                        {
                            for (int k = 0; k < 4; ++k)
                            {
                                info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                            }
                        }
                        else if (token == "[curse]")      // 0x12b5:循环 4 次
                        {
                            for (int k = 0; k < 4; ++k)
                            {
                                info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                            }
                        }
                        ++statusIdx;                      // 0x130a
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[active status]",
                                                          static_cast<int>(AITAG_ACTIVE_STATUS)));
                case AITAG_DAMAGE_REACTION:   // +0x1c 枚举
                    if (__inited)
                    {
                        bool isType;          // -0x3d0
                        ScanTypeOrStr(isType, token, true);
                        if (token == "[none]")
                            info->m_damageReaction = 0;
                        else if (token == "[damage]")
                            info->m_damageReaction = 1;
                        else if (token == "[down]")
                            info->m_damageReaction = 2;
                        else if (token == "[damage except human]")
                            info->m_damageReaction = 3;
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[damage reaction]",
                                                          static_cast<int>(AITAG_DAMAGE_REACTION)));
                case AITAG_PUSH_ASIDE:        // +0x20 int
                    if (__inited)
                    {
                        info->m_pushAside = ScanInt((bool*)0);
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[push aside]",
                                                          static_cast<int>(AITAG_PUSH_ASIDE)));
                case AITAG_LIFT_UP:           // +0x24 int
                    if (__inited)
                    {
                        info->m_liftUp = ScanInt((bool*)0);
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[lift up]", static_cast<int>(AITAG_LIFT_UP)));
                case AITAG_CRITICAL_HIT:      // +0x28 float
                    if (__inited)
                    {
                        info->m_criticalHit = ScanFloat((bool*)0);
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[critical hit]",
                                                          static_cast<int>(AITAG_CRITICAL_HIT)));
                case AITAG_STUCK:             // +0x2c float
                    if (__inited)
                    {
                        info->m_stuck = ScanFloat((bool*)0);
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[stuck]", static_cast<int>(AITAG_STUCK)));
                case AITAG_STUCK_PROB_INCREASE:  // +0x30 bool(int)
                    if (__inited)
                    {
                        info->m_stuckProbIncrease = (ScanInt((bool*)0) != 0);
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[stuck prob increase]",
                                                          static_cast<int>(AITAG_STUCK_PROB_INCREASE)));
                case AITAG_KNUCK_BACK:        // +0x34 int;==3 时 +0x38 角度
                    if (__inited)
                    {
                        info->m_knuckBack = ScanInt((bool*)0);
                        if (info->m_knuckBack == 3)   // 0x2b56;fistpl 舍入转 int
                        {
                            bool ok = false;          // -0x3d1
                            info->m_knuckBackAngle = (int)ScanFloat(&ok);
                        }
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[knuck back]",
                                                          static_cast<int>(AITAG_KNUCK_BACK)));
                case AITAG_WEAKNESS:          // +0x3c/+0x40 float
                    if (__inited)
                    {
                        info->m_weakness = ScanFloat((bool*)0);
                        info->m_weakness2 = ScanFloat((bool*)0);
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[weakness]", static_cast<int>(AITAG_WEAKNESS)));
                case AITAG_BOUNCE:            // +0x44 int
                    if (__inited)
                    {
                        info->m_bounce = ScanInt((bool*)0);
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[bounce]", static_cast<int>(AITAG_BOUNCE)));
                case AITAG_IGNORE_DEFENSE:    // +0x48 bool(int)
                    if (__inited)
                    {
                        info->m_ignoreDefense = (ScanInt((bool*)0) != 0);
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[ignore defense]",
                                                          static_cast<int>(AITAG_IGNORE_DEFENSE)));
                case AITAG_HUMAN_DAMAGE_RATE: // +0x11c float
                    if (__inited)
                    {
                        info->m_damageRate[0] = ScanFloat((bool*)0);
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[human damage rate]",
                                                          static_cast<int>(AITAG_HUMAN_DAMAGE_RATE)));
                case AITAG_FORT_DAMAGE_RATE:  // +0x120 float
                    if (__inited)
                    {
                        info->m_damageRate[1] = ScanFloat((bool*)0);
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[fort damage rate]",
                                                          static_cast<int>(AITAG_FORT_DAMAGE_RATE)));
                case AITAG_MONSTER_DAMAGE_RATE:  // +0x124 float
                    if (__inited)
                    {
                        info->m_damageRate[2] = ScanFloat((bool*)0);
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[monster damage rate]",
                                                          static_cast<int>(AITAG_MONSTER_DAMAGE_RATE)));
                case AITAG_HP_DRAIN:          // +0x78 bool(int)
                    if (__inited)
                    {
                        info->m_hpDrain = (ScanInt((bool*)0) != 0);
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[HP drain]", static_cast<int>(AITAG_HP_DRAIN)));
                case AITAG_MP_DRAIN:          // +0x7c bool(int)
                    if (__inited)
                    {
                        info->m_mpDrain = (ScanInt((bool*)0) != 0);
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[MP drain]", static_cast<int>(AITAG_MP_DRAIN)));
                case AITAG_HIT_WAV:           // +0x4c string
                    if (__inited)
                    {
                        ScanStr(&info->m_hitWav);
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[hit wav]", static_cast<int>(AITAG_HIT_WAV)));
                case AITAG_ATTACK_DIRECTION:  // +0x50 枚举(lift up/horizon/down)
                    if (__inited)
                    {
                        bool isType;          // -0x3d2
                        ScanTypeOrStr(isType, token, true);
                        if (token == "[hit lift up]")
                            info->m_attackDirection = 0;
                        else if (token == "[hit horizon]")
                            info->m_attackDirection = 1;
                        else if (token == "[hit down]")
                            info->m_attackDirection = 2;
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[attack direction]",
                                                          static_cast<int>(AITAG_ATTACK_DIRECTION)));
                case AITAG_HIT_INFO:          // +0x54/+0x58/+0x5c/+0x60
                    if (__inited)
                    {
                        bool isType;          // -0x3d3(两段复用同槽)
                        ScanTypeOrStr(isType, token, true);
                        if (token == "[blow]")
                            info->m_hitInfo = 0;
                        else if (token == "[cut]")
                            info->m_hitInfo = 1;
                        else if (token == "[burn]")
                            info->m_hitInfo = 2;
                        else if (token == "[mana burn]")
                            info->m_hitInfo = 4;
                        else if (token == "[etc]")
                            info->m_hitInfo = 3;
                        ScanTypeOrStr(isType, token, true);   // 0x1dd4 [no blood]/[blood]
                        if (token == "[no blood]")
                            info->m_blood = 0;
                        else if (token == "[blood]")
                            info->m_blood = 1;
                        info->m_field5c = ScanInt((bool*)0);   // +0x5c
                        info->m_field60 = ScanFloat((bool*)0); // +0x60
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[hit info]", static_cast<int>(AITAG_HIT_INFO)));
                case AITAG_IS_USE_WEAPON_HIT_DELAY_RATE:  // +0x64 byte bool
                    if (__inited)
                    {
                        info->m_isUseWeaponHitDelayRate = (ScanInt((bool*)0) != 0);
                        break;
                    }
                    __lookup.insert(LookupMap::value_type(
                        "[is use weapon hit delay rate]",
                        static_cast<int>(AITAG_IS_USE_WEAPON_HIT_DELAY_RATE)));
                case AITAG_FORCE_HIT_STUN_TIME:  // +0x74 int
                    if (__inited)
                    {
                        info->m_forceHitStunTime = ScanInt((bool*)0);
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[force hit stun time]",
                                                          static_cast<int>(AITAG_FORCE_HIT_STUN_TIME)));
                case AITAG_HIT_DIRECTION:     // +0x68 枚举(auto/front/back/...)
                    if (__inited)
                    {
                        bool isType;          // -0x3d4
                        ScanTypeOrStr(isType, token, true);
                        if (token == "[auto]")
                            info->m_hitDirection = 0;
                        else if (token == "[front]")
                            info->m_hitDirection = 1;
                        else if (token == "[back]")
                            info->m_hitDirection = 2;
                        else if (token == "[outer]")
                            info->m_hitDirection = 3;
                        else if (token == "[inner]")
                            info->m_hitDirection = 4;
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[hit direction]",
                                                          static_cast<int>(AITAG_HIT_DIRECTION)));
                case AITAG_IGNORE_SUPER_ARMOR:  // +0x6c int
                    if (__inited)
                    {
                        info->m_ignoreSuperArmor = ScanInt((bool*)0);
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[ignore super armor]",
                                                          static_cast<int>(AITAG_IGNORE_SUPER_ARMOR)));
                case AITAG_IGNORE_WEIGHT:     // +0x70 int
                    if (__inited)
                    {
                        info->m_ignoreWeight = ScanInt((bool*)0);
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[ignore weight]",
                                                          static_cast<int>(AITAG_IGNORE_WEIGHT)));
                case AITAG_HUMAN_ACTIVE_STATUS_RATE:  // +0x18 = 值/100.0f
                    if (__inited)
                    {
                        float f = ScanFloat((bool*)0);   // -0x1c 中转
                        info->m_humanActiveStatusRate = f / 100.0f;
                        break;
                    }
                    __lookup.insert(LookupMap::value_type(
                        "[human active status rate]", static_cast<int>(AITAG_HUMAN_ACTIVE_STATUS_RATE)));
                case AITAG_IS_HIT_NUMBER_INCREASING:  // +0x88 byte bool
                    if (__inited)
                    {
                        info->m_isHitNumberIncreasing = (ScanInt((bool*)0) != 0);
                        break;
                    }
                    __lookup.insert(LookupMap::value_type(
                        "[is hit number increasing]", static_cast<int>(AITAG_IS_HIT_NUMBER_INCREASING)));
                case AITAG_CATCH_OBJECT:      // +0x128 vector<CatchObjectInfo>
                    if (__inited)
                    {
                        info->m_catchObjects.clear();    // 0x22d6
                        CatchObjectInfo obj;             // -0x3e0{int,int,float}
                        bool ok;                         // -0x3e1
                        for (;;)                         // 0x22e6 回环
                        {
                            obj.m_field0 = ScanInt(&ok); // 返回值直接入 field0
                            if (!ok)
                                break;
                            obj.m_field4 = ScanInt((bool*)0);
                            obj.m_field8 = ScanFloat((bool*)0);
                            info->m_catchObjects.push_back(obj);
                        }
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[catch object]",
                                                          static_cast<int>(AITAG_CATCH_OBJECT)));
                case AITAG_MAX_TARGET_HIT_NUMBER:  // +0x94 int
                    if (__inited)
                    {
                        info->m_maxTargetHitNumber = ScanInt((bool*)0);
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[max target hit number]",
                                                          static_cast<int>(AITAG_MAX_TARGET_HIT_NUMBER)));
                case AITAG_IGNORE_DIEHARD:    // +0x90 byte bool
                    if (__inited)
                    {
                        info->m_ignoreDiehard = (ScanInt((bool*)0) != 0);
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[ignore diehard]",
                                                          static_cast<int>(AITAG_IGNORE_DIEHARD)));
                case AITAG_HIT_CHECK_BY_SCREEN_POS:  // +0x91;ScanInt 失败 → ret false
                    if (__inited)
                    {
                        int v = 0;                       // -0x3e8
                        if (!ScanInt(&v))                // _Z7ScanIntPi 0x2485
                            return false;                // 0x2491(不经 unload)
                        info->m_hitCheckByScreenPos = (v != 0);
                        break;
                    }
                    __lookup.insert(LookupMap::value_type("[hit check by screen pos]",
                                                          static_cast<int>(AITAG_HIT_CHECK_BY_SCREEN_POS)));
                    break;                    // 注册链终点(0x2470 → 公共尾)
                default:                      // 未知标签:静默忽略(ORIG find=-1)
                    break;
                }

                if (!__inited)                // 0x24bb:注册完成 → 置位并同 tag 重处理
                {
                    __init_lock.leave();
                    __inited = true;
                    continue;
                }
                break;                        // 行处理完成 → 读下一行
            }
        }
    }
    unloadRDARScriptFile();                   // 0x24ee
    return true;
}

// ORIG 0x88d1450：无参，直接返回 true（10 字节：push/mov/xor 后 mov $1）。
bool InitAttackInfoScript()
{
    return true;
}

// 既有转发契约（非 ORIG 符号，保留供外部调用）。
bool InitAttackInfoScript(AttackInfo* info, const char* path, int param3)
{
    return ImportAttackInfoScript(info, path, param3);
}

// ============================================================================
// user_creature::CCreatureScript —— 实现（CreatureScript.h 声明）
// ============================================================================
#include "CCreatureScript.h"

namespace user_creature
{

// ORIG 0x833c77e：vector +8、+0x24/+0x28/+0x2c 清零、set +0x34、vector +0x68、
// clear +8、+0x20 置 0、clear +0x68。
CCreatureScript::CCreatureScript()
    : m_field0(0), m_field4(0), m_vec8(), m_field14(0), m_field18(0),
      m_itemId(0), m_artifactSlotFlags(0), m_field24(0), m_field28(0),
      m_field2c(0), m_set34(), m_field4c(0), m_field50(0), m_field54(0),
      m_field58(0), m_field5c(0), m_field60(0), m_field64(0), m_vec68()
{
    m_vec8.clear();
    m_field24 = 0;
    m_field28 = 0;
    m_field2c = 0;
    m_artifactSlotFlags = 0;
    m_vec68.clear();
}

// ORIG 0x833c848：释放 +0x20 数组（delete[]），析构 +0x68/+0x34/+8。
CCreatureScript::~CCreatureScript()
{
    if (m_artifactSlotFlags != 0)
    {
        delete[] m_artifactSlotFlags;
        m_artifactSlotFlags = 0;
    }
    m_vec68.clear();
    m_set34.clear();
    m_vec8.clear();
}

// ORIG 0x833c8e8：从原生 CreatureScript* 拷贝字段。
// ORIG 0x833c8e8：从原生 CreatureScript* 拷贝字段。
// ORIG 无 src==0 检查（0x833c8e8 直接 _Znaj 后访问 src）；flags 以 0x833c90b
// 循环读（mov 0xc(%ebp),%eax; mov i,%ecx; add $0x14,%ecx; mov (%eax,%ecx,4),%eax
// = src + 4*(i+0x14) = src+0x50+4i，即 m_artifactSlotRed/Blue/Green 组）。
void CCreatureScript::Set(CreatureScript* src)
{
    m_artifactSlotFlags = new bool[3];
    for (int i = 0; i < 3; ++i)
    {
        m_artifactSlotFlags[i] =
            reinterpret_cast<const int*>(src)[0x14 + i] != 0;
    }

    m_field0 = src->m_evolutionQuestId;
    m_field4 = src->m_evolutionCreatureId;

    // ORIG 无 clear：目标 vector 假定空，按 size()+ix 索引循环拷贝。
    int n8 = (int)src->m_evolutionMultyId.size();
    for (int i = 0; i < n8; ++i)
    {
        m_vec8.push_back(src->m_evolutionMultyId[i]);
    }

    m_field14 = src->m_evolutionLevel;
    m_field18 = src->m_maxLevel;
    m_field24 = src->m_field1c;
    m_field28 = src->m_field20;
    m_field2c = (unsigned char)(src->m_learnOverSkillLevel & 0xff);

    int nSet = (int)src->m_itemEvolCreatureIds.size();
    for (int i = 0; i < nSet; ++i)
    {
        m_set34.insert(src->m_itemEvolCreatureIds[i].m_creatureId);
    }

    m_field4c = src->m_selectiveGrow.m_usingExpItem;
    m_field50 = src->m_selectiveGrow.m_growLevel;
    m_field54 = src->m_selectiveGrow.m_growATypeCreature;
    m_field58 = src->m_selectiveGrow.m_growBTypeCreature;
    m_field5c = src->m_selectiveGrow.m_growATypeItem;
    m_field60 = src->m_selectiveGrow.m_growBTypeItem;
    m_field64 = src->m_selectiveGrow.m_growItemCount;

    int n68 = (int)src->m_eventEvlouteCreature.size();
    for (int i = 0; i < n68; ++i)
    {
        m_vec68.push_back(src->m_eventEvlouteCreature[i]);
    }
}

// ORIG 0x833cad8：find(m_set34, creatureId)；命中返回 creatureId，否则 0。
int CCreatureScript::GetItemEvolutionCreatureId(int creatureId)
{
    std::set<int>::const_iterator it = m_set34.find(creatureId);
    if (it != m_set34.end())
    {
        return creatureId;
    }
    return 0;
}

// ORIG 0x833cb38：slot-0x17 在 [0,4) 内返回 m_artifactSlotFlags[slot-0x17]，否则 0。
// 返回 bool + 字段 bool*：callee 尾 movzbl 直落 + caller 0x833771d call 后
// 无物化 = bool→bool 透传（int 返回在 4.4 caller 必插 test+setne）。
bool CCreatureScript::IsAvailableArtifactSlot(int slot)
{
    // ORIG 0x833cb38：仅上界 cmp $3;jle（无下界/判空，负 idx 直读为 ORIG 实况）；
    // slot-0x17 两次现算，无局部槽，pop 收尾。
    if (slot - 0x17 > 3)
        return false;
    return m_artifactSlotFlags[slot - 0x17];
}

void CCreatureScript::SetItemId(int itemId)
{
    m_itemId = itemId;
}

int CCreatureScript::GetItemId()
{
    return m_itemId;
}

bool CCreatureScript::IsExistEvolutionQuest()
{
    return m_field0 != 0;
}

int CCreatureScript::GetEvolutionCreatureId()
{
    return m_field4;
}

// ORIG 0x833f150：遍历 +0x68 向量，命中返回 true。
bool CCreatureScript::validEventEvlouteCreature(int creatureId)
{
    for (std::vector<int>::const_iterator it = m_vec68.begin();
         it != m_vec68.end(); ++it)
    {
        if (*it == creatureId)
        {
            return true;
        }
    }
    return false;
}

// ORIG 0x833f1e0：遍历 +8 向量，命中返回 true。
bool CCreatureScript::isEvoluationCreature(int creatureId)
{
    for (std::vector<int>::const_iterator it = m_vec8.begin();
         it != m_vec8.end(); ++it)
    {
        if (*it == creatureId)
        {
            return true;
        }
    }
    return false;
}

int CCreatureScript::GetEvolutionLevel()
{
    return m_field14;
}

int CCreatureScript::GetMaxLevel()
{
    return m_field18;
}

int CCreatureScript::GetSkillRecoveryTime()
{
    return m_field24;
}

int CCreatureScript::GetOverSkillRecoveryTime()
{
    return m_field28;
}

int CCreatureScript::GetLearnOverSkillLevel()
{
    return (int)(unsigned char)m_field2c;
}

// ORIG 0x833f280：if (m_field4c != 0) return 0;
//   if (m_field5c==itemIdx || m_field60==itemIdx) return 1 else 0。
bool CCreatureScript::validItemExp(int itemIdx)
{
    // ORIG 0x833f280：movzbl 0x4c;xor$1;test;je 校验 → m_field4c==1(有效)才校验
    register unsigned char v asm("eax") = m_field4c;
    v ^= 1;
    if (v)
        return false;
    if (m_field5c == itemIdx || m_field60 == itemIdx)
    {
        return true;
    }
    return false;
}

// ORIG 0x833f2bc：if (m_field4c != 0) return 0; if (m_field5c==itemIdx) return 1 else 0。
bool CCreatureScript::validItem_AType(int itemIdx)
{
    // ORIG 0x833f2bc：movzbl 0x4c;xor$1;test;je 校验 → m_field4c==1 才校验
    register unsigned char v asm("eax") = m_field4c;
    v ^= 1;
    if (v)
        return false;
    if (m_field5c == itemIdx)
    {
        return true;
    }
    return false;
}

// ORIG 0x833f2ee：if (m_field4c != 0) return 0; if (m_field60==itemIdx) return 1 else 0。
bool CCreatureScript::validItem_BType(int itemIdx)
{
    // ORIG 0x833f2ee：同族，m_field4c==1 才校验
    register unsigned char v asm("eax") = m_field4c;
    v ^= 1;
    if (v)
        return false;
    if (m_field60 == itemIdx)
    {
        return true;
    }
    return false;
}

// ORIG 0x833f320：if (m_field5c==-1 && m_field60==-1) return 0 else return m_field4c。
bool CCreatureScript::IsItemGrowCreature()
{
    if (m_field5c == -1 && m_field60 == -1)
    {
        return false;
    }
    return m_field4c;
}

int CCreatureScript::GetTypeSelectEvolutionLevel()
{
    return m_field50;
}

int CCreatureScript::GetGrowItemCountforLevelUp()
{
    return m_field64;
}

int CCreatureScript::GET_ATypeCreature()
{
    return m_field54;
}

int CCreatureScript::GET_BTypeCreature()
{
    return m_field58;
}

}  // namespace user_creature

// ORIG 0x88bfee0：ENUM_ELEMENT → 名称字符串（getElementName::buffer）。
const char* getElementName(ENUM_ELEMENT element)
{
    static char buffer[0x10];
    switch (element)
    {
    case 0:
        strncpy(buffer, "[fire element]", 0x10);
        break;
    case 1:
        strncpy(buffer, "[water element]", 0x10);
        break;
    case 2:
        strncpy(buffer, "[dark element]", 0x10);
        break;
    case 3:
        strncpy(buffer, "[light element]", 0x10);
        break;
    default:
        strncpy(buffer, "[no element]", 0x10);
        break;
    }
    return buffer;
}

// ---- OnlyOrigBatch3：CreatureAdditionalAnimation 3参构造（ORIG W 0x08a3dcc2）----
// ORIG：m_name 默认构造（_ZNSsC1Ev）后赋值（_ZNSsaSEPKc）
CreatureAdditionalAnimation::CreatureAdditionalAnimation(int a, int b, const char* name)
    : m_field0(a), m_field4(b)
{
    m_name = name;
}
