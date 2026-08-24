// ============================================================================
// df_game_r 还原 —— native CreatureScript / AttackInfo 类型实现 + 两个解析函数
//   importCreatureScript / ImportAttackInfoScript 及 g_creatureScriptFileList 全局。
//   依据 docs/class_func_reports/_lt_global_gt_/importCreatureScript.md、
//   ImportAttackInfoScript.md 与 AttackInfo/*.md。
// ============================================================================
#include "CreatureScript.h"

#include <string.h>
#include <cstdio>

#include "DNFLexWrapper.h"
#include "GameScriptEnumHelpers.h"
#include "LogManager.h"
#include "NpcScript.h"

// ---- 全局（ORIG 属 RDARScriptObject.cpp/RDARScriptAttackInfo.cpp）----
char g_creatureScriptBaseDirectory[0x104];

// ORIG g_creatureScriptFileList（STScriptFileList，+0x00 即 map<int,string>）。
std::map<int, std::string> g_creatureScriptFileList;

// 模块名表 MODULE_NAME_TAG（processModuleTagSet 用；+8 步长 = 开/闭 string）。
std::string g_moduleNameTag[0x20];

// ---- 原生类型生命周期 ----

selectiveGrowCreatureInfo::selectiveGrowCreatureInfo()
{
    clear();
}

void selectiveGrowCreatureInfo::clear()
{
    m_usingExpItem = 0;
    m_growLevel = 0;
    m_growATypeCreature = 0;
    m_growBTypeCreature = 0;
    m_growATypeItem = 0;
    m_growBTypeItem = 0;
    m_growItemCount = 0;
}

void CreatureScriptSkillInfo::clear()
{
    m_str0.clear();
    m_str4.clear();
    m_field8 = 0;
    m_fieldc = 0;
    m_str10.clear();
    m_str14.clear();
    m_str18.clear();
    m_str1c.clear();
    m_str20.clear();
    m_str24.clear();
}

// ORIG 0x833e5ac：构造全部字符串/向量/映射成员（自动成员构造）后调 clear()。
CreatureScript::CreatureScript()
    : m_id(0),
      m_name(), m_field8(0), m_fieldc(0), m_field10(0), m_layer(0), m_field18(0),
      m_field1c(0), m_field20(0), m_directionCorrect(0),
      m_moveSpeed(), m_gravity(0), m_startLevel(0), m_permissionLevel(0),
      m_maxLevel(0), m_parentMaxLevel(0), m_artifactSlotRed(0),
      m_artifactSlotBlue(0), m_artifactSlotGreen(0), m_learnOverSkillLevel(0),
      m_skillMP(0), m_overSkillMP(0), m_skillType(0), m_overSkillType(0),
      m_basicMotion(), m_walkMotion(), m_runMotion(), m_flyMotion(),
      m_getExpMotion(), m_getExpOverMotion(), m_getExpUnderMotion(),
      m_skillMotion(), m_overSkillMotion(), m_responseMotion(), m_specialMotion(),
      m_equipMotion(), m_equipOverMotion(), m_equipUnderMotion(),
      m_basicOverEffect(), m_walkOverEffect(), m_runOverEffect(),
      m_skillOverEffect(), m_overSkillOverEffect(), m_responseOverEffect(),
      m_specialOverEffect(), m_basicUnderEffect(), m_walkUnderEffect(),
      m_runUnderEffect(), m_skillUnderEffect(), m_overSkillUnderEffect(),
      m_responseUnderEffect(), m_specialUnderEffect(), m_defaultUnderEffect(),
      m_additionalAnimations(), m_defaultParticle(), m_responseSoundTag(),
      m_wingSoundTag(), m_walkSoundTag(), m_runSoundTag(),
      m_revisionX(0), m_revisionY(0), m_etcMotion(), m_attackInfo(),
      m_skillString(), m_skillLevelValues(), m_overSkillLevelValues(),
      m_piercing(), m_intData(), m_stringData(), m_directory(),
      m_evolutionQuestId(0), m_evolutionCreatureId(0), m_evolutionLevel(0),
      m_evolutionMultyId(), m_selectiveGrow(), m_randomMotion(),
      m_defaultRandomMotionRate(0), m_increaseRandomMotionRate(0),
      m_itemEvolution(0), m_itemEvolCreatureIds(), m_skillInfo(),
      m_overSkillInfo(), m_skillList(), m_overSkillList(),
      m_usingRandomSkill(0), m_eventEvlouteCreature()
{
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
    m_layer = 0;
    m_field18 = 0;
    m_field1c = 0;
    m_field20 = 0;
    m_directionCorrect = 0;
    for (int i = 0; i < 5; ++i)
    {
        m_moveSpeed[i] = 0;
    }
    m_gravity = 0;
    m_startLevel = 0;
    m_permissionLevel = 0;
    m_maxLevel = 0;
    m_parentMaxLevel = 0;
    m_artifactSlotRed = 0;
    m_artifactSlotBlue = 0;
    m_artifactSlotGreen = 0;
    m_learnOverSkillLevel = 0;
    m_skillMP = 0;
    m_overSkillMP = 0;
    m_skillType = 0;
    m_overSkillType = 0;
    m_basicMotion.clear();
    m_walkMotion.clear();
    m_runMotion.clear();
    m_flyMotion.clear();
    m_getExpMotion.clear();
    m_getExpOverMotion.clear();
    m_getExpUnderMotion.clear();
    m_skillMotion.clear();
    m_overSkillMotion.clear();
    m_responseMotion.clear();
    m_specialMotion.clear();
    m_equipMotion.clear();
    m_equipOverMotion.clear();
    m_equipUnderMotion.clear();
    m_basicOverEffect.clear();
    m_walkOverEffect.clear();
    m_runOverEffect.clear();
    m_skillOverEffect.clear();
    m_overSkillOverEffect.clear();
    m_responseOverEffect.clear();
    m_specialOverEffect.clear();
    m_basicUnderEffect.clear();
    m_walkUnderEffect.clear();
    m_runUnderEffect.clear();
    m_skillUnderEffect.clear();
    m_overSkillUnderEffect.clear();
    m_responseUnderEffect.clear();
    m_specialUnderEffect.clear();
    m_defaultUnderEffect.clear();
    m_additionalAnimations.clear();
    m_defaultParticle.clear();
    m_responseSoundTag.clear();
    m_wingSoundTag.clear();
    m_walkSoundTag.clear();
    m_runSoundTag.clear();
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
    m_directory.clear();
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
AttackInfo::AttackInfo()
    : m_damage(0.0f), m_damageIncreaseRate(0.0f), m_absoluteDamage(0),
      m_damageBonus(0), m_attackType(0), m_weaponDamageApply(0),
      m_humanActiveStatusRate(0.0f), m_damageReaction(0), m_pushAside(0),
      m_liftUp(0), m_criticalHit(0.0f), m_stuck(0.0f), m_stuckProbIncrease(0),
      m_knuckBack(0), m_knuckBackAngle(0), m_weakness(0.0f), m_weakness2(0.0f),
      m_bounce(0), m_ignoreDefense(0), m_hitWav(), m_attackDirection(0),
      m_hitInfo(0), m_blood(0), m_field5c(0), m_field60(0.0f),
      m_isUseWeaponHitDelayRate(0), m_hitDirection(0), m_ignoreSuperArmor(0),
      m_ignoreWeight(0), m_forceHitStunTime(0), m_hpDrain(0), m_mpDrain(0),
      m_field80(0), m_field84(0.0f), m_isHitNumberIncreasing(0), m_moduleTag(0),
      m_ignoreDiehard(0), m_hitCheckByScreenPos(0), m_maxTargetHitNumber(0),
      m_attackMyself(0), m_attackFriend(0), m_attackEnemy(0), m_elements(),
      m_activeStatusVectors(), m_activeStatusType(), m_activeStatusValue(),
      m_activeStatusInt(), m_activeStatusInt2(), m_activeStatusInt3(),
      m_damageRate(), m_catchObjects()
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
    m_damageIncreaseRate = 0.0f;
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
    m_forceHitStunTime = 0;
    m_hpDrain = 0;
    m_mpDrain = 0;
    m_field80 = 0;
    m_field84 = 100.0f;
    m_isHitNumberIncreasing = 1;
    m_moduleTag = 3;
    m_ignoreDiehard = 0;
    m_hitCheckByScreenPos = 0;
    m_maxTargetHitNumber = -1;
    m_attackMyself = 0;
    m_attackFriend = 0;
    m_attackEnemy = 1;
    m_elements.clear();
    m_activeStatusType[0] = 0x12;
    m_activeStatusType[1] = 0x12;
    m_activeStatusType[2] = 0x12;
    m_activeStatusValue[0] = 0.0f;
    m_activeStatusValue[1] = 0.0f;
    m_activeStatusValue[2] = 0.0f;
    m_activeStatusInt[0] = 0;
    m_activeStatusInt[1] = 0;
    m_activeStatusInt[2] = 0;
    m_activeStatusInt2[0] = 0;
    m_activeStatusInt2[1] = 0;
    m_activeStatusInt2[2] = 0;
    m_activeStatusInt3[0] = 0;
    m_activeStatusInt3[1] = 0;
    m_activeStatusInt3[2] = 0;
    for (int i = 0; i < 3; ++i)
    {
        m_activeStatusVectors[i].clear();
        m_damageRate[i] = 100.0f;
    }
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
        m_elements.erase(element);
        m_elements.insert(element);
    }
}

// ORIG 0x88907fe：对集合中每个元素（除 4）置 is-valid 标记，调用 isValidAttackInfoElement。
bool AttackInfo::isValidElement(ENUM_ELEMENT element)
{
    char flags[4] = {0, 0, 0, 0};
    for (std::set<ENUM_ELEMENT>::const_iterator it = m_elements.begin();
         it != m_elements.end(); ++it)
    {
        if (*it != ENUM_ELEMENT_NONE && *it >= 0 && *it < 4)
        {
            flags[*it] = 1;
        }
    }
    return isValidAttackInfoElement(flags, element);
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
    return 0;
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
            for (int i = 0; i < 5; ++i)
            {
                script->m_moveSpeed[i] = ScanInt((bool*)0);
            }
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
            CreatureAdditionalAnimation anim;
            ScanStr(&anim.m_name);
            anim.m_field4 = ScanInt((bool*)0);
            ScanStr(&token);
            anim.m_field8 = 0;
            script->m_additionalAnimations.push_back(anim);
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
            bool ok = false;
            int v = ScanInt(&ok);
            while (ok)
            {
                script->m_skillLevelValues.push_back(v);
                v = ScanInt(&ok);
            }
        }
        else if (tag == "[over skill level values]")
        {
            script->m_overSkillLevelValues.clear();
            bool ok = false;
            int v = ScanInt(&ok);
            while (ok)
            {
                script->m_overSkillLevelValues.push_back(v);
                v = ScanInt(&ok);
            }
        }
        else if (tag == "[piercing]")
        {
            script->m_piercing.clear();
            bool ok = false;
            int v = ScanInt(&ok);
            while (ok)
            {
                script->m_piercing.push_back(v);
                v = ScanInt(&ok);
            }
        }
        else if (tag == "[int data]")
        {
            script->m_intData.clear();
            bool ok = false;
            int v = ScanInt(&ok);
            while (ok)
            {
                script->m_intData.push_back(v);
                v = ScanInt(&ok);
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
                script->m_evolutionMultyId.push_back(v);
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
            while (ScanType(tag, true) && tag != "[/selective grow creature]")
            {
                if (tag == "[item count for levelup]")
                    script->m_selectiveGrow.m_growItemCount = ScanInt((bool*)0);
                else if (tag == "[using exp item]")
                    script->m_selectiveGrow.m_usingExpItem = (ScanInt((bool*)0) != 0) ? 1 : 0;
                else if (tag == "[grow Atype item index]")
                    script->m_selectiveGrow.m_growATypeItem = ScanInt((bool*)0);
                else if (tag == "[grow Btype item index]")
                    script->m_selectiveGrow.m_growBTypeItem = ScanInt((bool*)0);
                else if (tag == "[grow Atype creature id]")
                    script->m_selectiveGrow.m_growATypeCreature = ScanInt((bool*)0);
                else if (tag == "[grow Btype creature id]")
                    script->m_selectiveGrow.m_growBTypeCreature = ScanInt((bool*)0);
                else if (tag == "[grow level]")
                    script->m_selectiveGrow.m_growLevel = ScanInt((bool*)0);
            }
        }
        else if (tag == "[event evolute creature]")
        {
            script->m_eventEvlouteCreature.clear();
            bool ok = false;
            int v = ScanInt(&ok);
            while (ok)
            {
                script->m_eventEvlouteCreature.push_back(v);
                v = ScanInt(&ok);
            }
        }
        else if (tag == "[item evolution]")
        {
            bool ok = false;
            int itemId = ScanInt(&ok);
            if (!ok)
            {
                LogManager::logFormat(0, "../RDARScriptObject.cpp", "importCreatureScript",
                                      0x259e,
                                      "creature script loading  %s, evolution_item_id_",
                                      path);
                return false;
            }
            script->m_itemEvolution = itemId;
            script->m_itemEvolCreatureIds.clear();
            while (ScanInt(&ok))
            {
                st_crature_id_item_id pair;
                pair.m_creatureId = itemId;
                pair.m_itemId = ScanInt(&ok);
                if (!ok)
                    return false;
                script->m_itemEvolCreatureIds.push_back(pair);
                itemId = pair.m_creatureId;
            }
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
            while (ScanType(tag, true) && tag != "[/skill list]")
            {
                if (tag == "[skill node]")
                {
                    bool ok = false;
                    int key = ScanInt(&ok);
                    if (!ok)
                        break;
                    CreatureScriptSkillInfo info;
                    info.clear();
                    int type = 0;
                    std::string sub;
                    while (ScanType(sub, true))
                    {
                        if (sub == "[/skill node]")
                            break;
                        if (sub == "[skill type]")
                        {
                            ScanStr(&token);
                            type = (token == "over skill") ? 1 : 0;
                        }
                        else if (sub == "[skill name]")
                        {
                            ScanStr(&info.m_str0);
                        }
                        else if (sub == "[skill explain]")
                        {
                            ScanStr(&info.m_str4);
                        }
                        else if (sub == "[skill recovery time]")
                        {
                            info.m_field8 = ScanInt((bool*)0);
                        }
                        else if (sub == "[learn skill level]")
                        {
                            info.m_fieldc = ScanInt((bool*)0);
                        }
                        else if (sub == "[skill mp]")
                        {
                            info.m_str10 = "";
                            info.m_field8 = ScanInt((bool*)0);
                        }
                        else if (sub == "[skill motion]")
                        {
                            ScanStr(&info.m_str14);
                        }
                        else if (sub == "[skill under effect]")
                        {
                            ScanStr(&info.m_str18);
                        }
                        else if (sub == "[skill over effect]")
                        {
                            ScanStr(&info.m_str1c);
                        }
                        else if (sub == "[skill info]")
                        {
                            ScanStr(&info.m_str20);
                        }
                    }
                    if (type == 0)
                        script->m_skillList[key] = info;
                    else
                        script->m_overSkillList[key] = info;
                }
            }
        }
        else if (tag == "[using random skill]")
        {
            bool ok = false;
            int v = ScanInt(&ok);
            if (!ok)
            {
                LogManager::logFormat(0, "../RDARScriptObject.cpp", "importCreatureScript",
                                      0x266c,
                                      "creature script loading  %s, evolution_item_id_",
                                      path);
                return false;
            }
            script->m_usingRandomSkill = (v != 0) ? 1 : 0;
        }
        else
        {
            LogManager::logFormat(0, "../RDARScriptObject.cpp", "importCreatureScript", 0x2674,
                                  "creature script loading error : %s : %s",
                                  tag.c_str(), path);
            return false;
        }
    }

    // 末尾：以文件列表索引写回 m_id。
    for (std::map<int, std::string>::const_iterator it = g_creatureScriptFileList.begin();
         it != g_creatureScriptFileList.end(); ++it)
    {
        if (it->second == path)
        {
            script->m_id = it->first;
            break;
        }
    }
    return true;
}

// ============================================================================
// ImportAttackInfoScript —— ORIG 0x88d145a（RDARScriptAttackInfo.cpp）
// ============================================================================
bool ImportAttackInfoScript(AttackInfo* info, const char* path, int param3)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }
    info->clear();

    std::string tag;
    std::string token;
    bool isType = false;
    int module = -1;
    while (ScanType(tag, true))
    {
        if (tag == "[damage]")
        {
            info->m_damage = ScanFloat((bool*)0);
        }
        else if (tag == "[damage increase rate]")
        {
            info->m_damageIncreaseRate = ScanFloat((bool*)0);
        }
        else if (tag == "[absolute damage]")
        {
            info->m_absoluteDamage = ScanInt((bool*)0);
        }
        else if (tag == "[damage bonus]")
        {
            info->m_damageBonus = ScanInt((bool*)0);
        }
        else if (tag == "[attack type]")
        {
            ScanTypeOrStr(isType, token, true);
            if (token == "[physic]")
                info->m_attackType = 0;
            else if (token == "[magic]")
                info->m_attackType = 1;
            else if (token == "[ignore defense]")
                info->m_attackType = 2;
        }
        else if (tag == "[weapon damage apply]")
        {
            info->m_weaponDamageApply = (ScanInt((bool*)0) != 0) ? 1 : 0;
        }
        else if (tag == "[attack myself]")
        {
            info->m_attackMyself = (ScanInt((bool*)0) != 0) ? 1 : 0;
        }
        else if (tag == "[attack friend]")
        {
            info->m_attackFriend = (ScanInt((bool*)0) != 0) ? 1 : 0;
        }
        else if (tag == "[attack enemy]")
        {
            info->m_attackEnemy = (ScanInt((bool*)0) != 0) ? 1 : 0;
        }
        else if (tag == "[elemental property]")
        {
            ScanTypeOrStr(isType, token, true);
            info->setElement((ENUM_ELEMENT)getElement(token.c_str()));
        }
        else if (tag == "[active status]")
        {
            int idx = 0;
            while (ScanTypeOrStr(isType, token, true))
            {
                if (idx >= 3)
                    break;
                info->m_activeStatusType[idx] = getActiveStatusType(token);
                info->m_activeStatusValue[idx] = ScanFloat((bool*)0);
                info->m_activeStatusInt[idx] = ScanInt((bool*)0);
                info->m_activeStatusInt2[idx] = ScanInt((bool*)0);
                info->m_activeStatusVectors[idx].clear();
                int v = ScanInt((bool*)0);
                info->m_activeStatusVectors[idx].push_back(v);
                idx++;
            }
        }
        else if (tag == "[damage reaction]")
        {
            ScanTypeOrStr(isType, token, true);
            if (token == "[none]")
                info->m_damageReaction = 0;
            else if (token == "[damage]")
                info->m_damageReaction = 1;
            else if (token == "[down]")
                info->m_damageReaction = 2;
            else if (token == "[damage except human]")
                info->m_damageReaction = 3;
        }
        else if (tag == "[push aside]")
        {
            info->m_pushAside = ScanInt((bool*)0);
        }
        else if (tag == "[lift up]")
        {
            info->m_liftUp = ScanInt((bool*)0);
        }
        else if (tag == "[critical hit]")
        {
            info->m_criticalHit = ScanFloat((bool*)0);
        }
        else if (tag == "[stuck]")
        {
            info->m_stuck = ScanFloat((bool*)0);
        }
        else if (tag == "[stuck prob increase]")
        {
            info->m_stuckProbIncrease = (ScanInt((bool*)0) != 0) ? 1 : 0;
        }
        else if (tag == "[knuck back]")
        {
            info->m_knuckBack = ScanInt((bool*)0);
            if (info->m_knuckBack == 3)
            {
                bool ok = false;
                info->m_knuckBackAngle = (int)ScanFloat(&ok);
            }
        }
        else if (tag == "[weakness]")
        {
            info->m_weakness = ScanFloat((bool*)0);
            info->m_weakness2 = ScanFloat((bool*)0);
        }
        else if (tag == "[bounce]")
        {
            info->m_bounce = ScanInt((bool*)0);
        }
        else if (tag == "[ignore defense]")
        {
            info->m_ignoreDefense = (ScanInt((bool*)0) != 0) ? 1 : 0;
        }
        else if (tag == "[human damage rate]")
        {
            info->m_damageRate[0] = ScanFloat((bool*)0);
        }
        else if (tag == "[fort damage rate]")
        {
            info->m_damageRate[1] = ScanFloat((bool*)0);
        }
        else if (tag == "[monster damage rate]")
        {
            info->m_damageRate[2] = ScanFloat((bool*)0);
        }
        else if (tag == "[hit wav]")
        {
            ScanStr(&info->m_hitWav);
        }
        else if (tag == "[attack direction]")
        {
            ScanTypeOrStr(isType, token, true);
            if (token == "[hit lift up]")
                info->m_attackDirection = 0;
            else if (token == "[hit horizon]")
                info->m_attackDirection = 1;
            else if (token == "[hit down]")
                info->m_attackDirection = 2;
        }
        else if (tag == "[hit info]")
        {
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
            ScanTypeOrStr(isType, token, true);
            info->m_blood = (token == "[blood]") ? 1 : 0;
            info->m_field5c = ScanInt((bool*)0);
            info->m_field60 = ScanFloat((bool*)0);
        }
        else if (tag == "[is use weapon hit delay rate]")
        {
            info->m_isUseWeaponHitDelayRate = (ScanInt((bool*)0) != 0) ? 1 : 0;
        }
        else if (tag == "[force hit stun time]")
        {
            info->m_forceHitStunTime = ScanInt((bool*)0);
        }
        else if (tag == "[hit direction]")
        {
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
        }
        else if (tag == "[ignore super armor]")
        {
            info->m_ignoreSuperArmor = ScanInt((bool*)0);
        }
        else if (tag == "[ignore weight]")
        {
            info->m_ignoreWeight = ScanInt((bool*)0);
        }
        else if (tag == "[HP drain]")
        {
            info->m_hpDrain = (ScanInt((bool*)0) != 0) ? 1 : 0;
        }
        else if (tag == "[MP drain]")
        {
            info->m_mpDrain = (ScanInt((bool*)0) != 0) ? 1 : 0;
        }
        else if (tag == "[human active status rate]")
        {
            info->m_humanActiveStatusRate = ScanFloat((bool*)0);
        }
        else if (tag == "[is hit number increasing]")
        {
            info->m_isHitNumberIncreasing = (ScanInt((bool*)0) != 0) ? 1 : 0;
        }
        else if (tag == "[catch object]")
        {
            info->m_catchObjects.clear();
            bool ok = false;
            while (ScanInt(&ok))
            {
                CatchObjectInfo obj;
                obj.m_field0 = info->m_field5c;
                obj.m_field4 = ScanInt((bool*)0);
                obj.m_field8 = ScanFloat((bool*)0);
                info->m_catchObjects.push_back(obj);
            }
        }
        else if (tag == "[max target hit number]")
        {
            info->m_maxTargetHitNumber = ScanInt((bool*)0);
        }
        else if (tag == "[ignore diehard]")
        {
            info->m_ignoreDiehard = (ScanInt((bool*)0) != 0) ? 1 : 0;
        }
        else if (tag == "[hit check by screen pos]")
        {
            int v = 0;
            bool ok = ScanInt(&v);
            if (!ok)
                return false;
            info->m_hitCheckByScreenPos = (v != 0) ? 1 : 0;
        }
        else
        {
            LogManager::logFormat(0, "../RDARScriptAttackInfo.cpp", "ImportAttackInfoScript",
                                  0x9c, "attack info script loading error : %s : %s",
                                  tag.c_str(), path);
            return false;
        }
    }
    return true;
}

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
void CCreatureScript::Set(CreatureScript* src)
{
    if (src == 0)
    {
        return;
    }
    m_artifactSlotFlags = new char[3];
    m_artifactSlotFlags[0] = (src->m_artifactSlotRed != 0) ? 1 : 0;
    m_artifactSlotFlags[1] = (src->m_artifactSlotBlue != 0) ? 1 : 0;
    m_artifactSlotFlags[2] = (src->m_artifactSlotGreen != 0) ? 1 : 0;

    m_field0 = src->m_evolutionQuestId;
    m_field4 = src->m_evolutionCreatureId;

    m_vec8.clear();
    for (std::vector<int>::const_iterator it = src->m_evolutionMultyId.begin();
         it != src->m_evolutionMultyId.end(); ++it)
    {
        m_vec8.push_back(*it);
    }

    m_field14 = src->m_evolutionLevel;
    m_field18 = src->m_maxLevel;
    m_field24 = src->m_field1c;
    m_field28 = src->m_field20;
    m_field2c = (unsigned char)(src->m_learnOverSkillLevel & 0xff);

    m_set34.clear();
    for (std::vector<st_crature_id_item_id>::const_iterator it =
             src->m_itemEvolCreatureIds.begin();
         it != src->m_itemEvolCreatureIds.end(); ++it)
    {
        m_set34.insert(it->m_creatureId);
    }

    m_field4c = src->m_selectiveGrow.m_usingExpItem;
    m_field50 = src->m_selectiveGrow.m_growLevel;
    m_field54 = src->m_selectiveGrow.m_growATypeCreature;
    m_field58 = src->m_selectiveGrow.m_growBTypeCreature;
    m_field5c = src->m_selectiveGrow.m_growATypeItem;
    m_field60 = src->m_selectiveGrow.m_growBTypeItem;
    m_field64 = src->m_selectiveGrow.m_growItemCount;

    m_vec68.clear();
    for (std::vector<int>::const_iterator it = src->m_eventEvlouteCreature.begin();
         it != src->m_eventEvlouteCreature.end(); ++it)
    {
        m_vec68.push_back(*it);
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
unsigned char CCreatureScript::IsAvailableArtifactSlot(int slot)
{
    int idx = slot - 0x17;
    if (idx < 0 || idx >= 4 || m_artifactSlotFlags == 0)
    {
        return 0;
    }
    return (unsigned char)m_artifactSlotFlags[idx];
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
    if (m_field4c != 0)
    {
        return false;
    }
    if (m_field5c == itemIdx || m_field60 == itemIdx)
    {
        return true;
    }
    return false;
}

// ORIG 0x833f2bc：if (m_field4c != 0) return 0; if (m_field5c==itemIdx) return 1 else 0。
bool CCreatureScript::validItem_AType(int itemIdx)
{
    if (m_field4c != 0)
    {
        return false;
    }
    if (m_field5c == itemIdx)
    {
        return true;
    }
    return false;
}

// ORIG 0x833f2ee：if (m_field4c != 0) return 0; if (m_field60==itemIdx) return 1 else 0。
bool CCreatureScript::validItem_BType(int itemIdx)
{
    if (m_field4c != 0)
    {
        return false;
    }
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
    return m_field4c != 0;
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
const char* getElementName(int element)
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
