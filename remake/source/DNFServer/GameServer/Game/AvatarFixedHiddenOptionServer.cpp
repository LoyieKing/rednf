// df_game_r Game/ AvatarFixedHiddenOptionServer 实现（独立 TU）。
// 依据 docs/class_func_reports/AvatarFixedHiddenOptionServer/、
// ARAD__SCRIPT__AvatarFixedHiddenOption/（含 parseAbility.md 全量反编译）与 ORIG 反汇编转写。
#include "AvatarFixedHiddenOptionServer.h"

#include <cstdio>
#include <cstring>
#include <map>

#include "GameTypes.h"       // cMyTrace
#include "DNFLexWrapper.h"   // loadRDARScriptFile / ScanType / ScanInt / ScanFloat

// ============================================================================
// STAvatarHiddenAbilityVariation —— ORIG ctor W 0x088b01e8 / 0x088b021a、dtor W 0x088b0aae
// ============================================================================
STAvatarHiddenAbilityVariation::STAvatarHiddenAbilityVariation(
    AVATAR_HIDDEN_ABILITY_TYPE::T type, int value, int rate)
{
    m_type = type;          // +0x00
    m_bFloatValue = false;  // +0x04 = 0
    m_fValue = 0.0f;        // +0x08 清零
    m_nValue = value;       // +0x0c
    m_nRate = rate;         // +0x10
}

STAvatarHiddenAbilityVariation::STAvatarHiddenAbilityVariation(
    AVATAR_HIDDEN_ABILITY_TYPE::T type, float value, int rate)
{
    m_type = type;          // +0x00
    m_bFloatValue = true;   // +0x04 = 1
    m_fValue = value;       // +0x08（浮点值）
    m_nValue = 0;           // +0x0c 清零
    m_nRate = rate;         // +0x10
}

STAvatarHiddenAbilityVariation::~STAvatarHiddenAbilityVariation()
{
}

// ============================================================================
// ARAD::SCRIPT::AvatarFixedHiddenOption
// ============================================================================

// ORIG C1 0x088aecac：6 个 vector 顺序构造后调 clearScript()。
ARAD::SCRIPT::AvatarFixedHiddenOption::AvatarFixedHiddenOption()
{
    clearScript();
}

// ORIG D1 0x088aed5e：clearScript 后逆序析构 6 个 vector（成员析构自动完成）。
ARAD::SCRIPT::AvatarFixedHiddenOption::~AvatarFixedHiddenOption()
{
}

// ORIG 0x088b00e0：clear 每桶并 push_back 一个全 0 默认元素（种子保证 Init 的
// reserve(count-1) 非负，与 ORIG 一致）。
void ARAD::SCRIPT::AvatarFixedHiddenOption::clearScript()
{
    for (int i = 0; i < 6; ++i)
    {
        m_abilities[i].clear();
        m_abilities[i].push_back(
            STAvatarHiddenAbilityVariation(AVATAR_HIDDEN_ABILITY_TYPE::NONE, 0, 0));
    }
}

// ORIG 0x088b0164：return *(int*)(this+0x48)
int ARAD::SCRIPT::AvatarFixedHiddenOption::getMaterialItemIndex()
{
    return m_materialItemIndex;
}

// ORIG 0x088b0170：return *(int*)(this+0x48) == itemIndex
bool ARAD::SCRIPT::AvatarFixedHiddenOption::isMaterialItemIndex(int itemIndex)
{
    return m_materialItemIndex == itemIndex;
}

namespace
{

// less_TCHAR 等价的 C 串比较器（ORIG map 键为 char_const*，函数局部静态映射内部使用，
// 不跨 TU 出现于 ABI）。【推断】比较器名；语义按 std::map<char*,int> strcmp 序。
struct LessCStr
{
    bool operator()(const char* a, const char* b) const
    {
        return std::strcmp(a, b) < 0;
    }
};

typedef std::map<const char*, int, LessCStr> AbilityLookup;

}  // namespace

// ORIG 0x088aef52 parseAbility：首次调用构建 21 项静态映射（SpinLock 双检的语义等价：
// 函数局部静态守卫），随后查 tag → 内部 id，读数值入桶。未知标签返回 false。
bool ARAD::SCRIPT::AvatarFixedHiddenOption::parseAbility(
    std::vector<STAvatarHiddenAbilityVariation>& variations, std::string& tag)
{
    static AbilityLookup s_lookup;
    static bool s_inited = false;
    if (!s_inited)
    {
        // 插入次序与 ORIG 初始化链一致（0x46..0xac）。
        s_lookup.insert(std::make_pair("[physical attack]", 0x46));
        s_lookup.insert(std::make_pair("[magical attack]", 0x4b));
        s_lookup.insert(std::make_pair("[physical defense]", 0x50));
        s_lookup.insert(std::make_pair("[magical defense]", 0x55));
        s_lookup.insert(std::make_pair("[HP MAX]", 0x5a));
        s_lookup.insert(std::make_pair("[MP MAX]", 0x5f));
        s_lookup.insert(std::make_pair("[HP regen speed]", 100));
        s_lookup.insert(std::make_pair("[MP Regen speed]", 0x69));
        s_lookup.insert(std::make_pair("[attack speed]", 0x6e));
        s_lookup.insert(std::make_pair("[move speed]", 0x73));
        s_lookup.insert(std::make_pair("[cast speed]", 0x78));
        s_lookup.insert(std::make_pair("[inventory limit]", 0x7d));
        s_lookup.insert(std::make_pair("[stuck]", 0x82));
        s_lookup.insert(std::make_pair("[stuck resistance]", 0x87));
        s_lookup.insert(std::make_pair("[all activestatus resistance]", 0x8c));
        s_lookup.insert(std::make_pair("[hit recovery]", 0x91));
        s_lookup.insert(std::make_pair("[equipment magical defence]", 0x96));
        s_lookup.insert(std::make_pair("[equipment physical defence]", 0x9c));
        s_lookup.insert(std::make_pair("[jump power]", 0xa2));
        s_lookup.insert(std::make_pair("[physical critical hit]", 0xa7));
        s_lookup.insert(std::make_pair("[magical critical hit]", 0xac));
        s_inited = true;
    }

    AbilityLookup::const_iterator it = s_lookup.find(tag.c_str());
    if (it == s_lookup.end())
        return false;

    switch (it->second)
    {
    case 0x46:   // [physical attack] → type 1
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::PHYSICAL_ATTACK, ScanInt((bool*)NULL), ScanInt((bool*)NULL)));
        break;
    case 0x4b:   // [magical attack] → type 2
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::MAGICAL_ATTACK, ScanInt((bool*)NULL), ScanInt((bool*)NULL)));
        break;
    case 0x50:   // [physical defense] → type 3
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::PHYSICAL_DEFENSE, ScanInt((bool*)NULL), ScanInt((bool*)NULL)));
        break;
    case 0x55:   // [magical defense] → type 4
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::MAGICAL_DEFENSE, ScanInt((bool*)NULL), ScanInt((bool*)NULL)));
        break;
    case 0x5a:   // [HP MAX] → type 5
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::HP_MAX, ScanInt((bool*)NULL), ScanInt((bool*)NULL)));
        break;
    case 0x5f:   // [MP MAX] → type 6
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::MP_MAX, ScanInt((bool*)NULL), ScanInt((bool*)NULL)));
        break;
    case 100:    // [HP regen speed] → type 7
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::HP_REGEN_SPEED, ScanInt((bool*)NULL), ScanInt((bool*)NULL)));
        break;
    case 0x69:   // [MP Regen speed] → type 8
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::MP_REGEN_SPEED, ScanInt((bool*)NULL), ScanInt((bool*)NULL)));
        break;
    case 0x6e:   // [attack speed] → type 9
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::ATTACK_SPEED, ScanInt((bool*)NULL), ScanInt((bool*)NULL)));
        break;
    case 0x73:   // [move speed] → type 10
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::MOVE_SPEED, ScanInt((bool*)NULL), ScanInt((bool*)NULL)));
        break;
    case 0x78:   // [cast speed] → type 11
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::CAST_SPEED, ScanInt((bool*)NULL), ScanInt((bool*)NULL)));
        break;
    case 0x7d:   // [inventory limit] → type 12
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::INVENTORY_LIMIT, ScanInt((bool*)NULL), ScanInt((bool*)NULL)));
        break;
    case 0x82:   // [stuck] → type 13（第一个数值为浮点）
    {
        float value = ScanFloat((bool*)NULL);
        int rate = ScanInt((bool*)NULL);
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::STUCK, value, rate));
        break;
    }
    case 0x87:   // [stuck resistance] → type 14
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::STUCK_RESISTANCE, ScanInt((bool*)NULL), ScanInt((bool*)NULL)));
        break;
    case 0x8c:   // [all activestatus resistance] → type 15
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::ALL_ACTIVESTATUS_RESISTANCE,
            ScanInt((bool*)NULL), ScanInt((bool*)NULL)));
        break;
    case 0x91:   // [hit recovery] → type 16
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::HIT_RECOVERY, ScanInt((bool*)NULL), ScanInt((bool*)NULL)));
        break;
    case 0x96:   // [equipment magical defence] → type 17（首个 ScanInt 弃读）
    {
        ScanInt((bool*)NULL);
        int value = ScanInt((bool*)NULL);
        int rate = ScanInt((bool*)NULL);
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::EQUIPMENT_MAGICAL_DEFENCE, value, rate));
        break;
    }
    case 0x9c:   // [equipment physical defence] → type 18（首个 ScanInt 弃读）
    {
        ScanInt((bool*)NULL);
        int value = ScanInt((bool*)NULL);
        int rate = ScanInt((bool*)NULL);
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::EQUIPMENT_PHYSICAL_DEFENCE, value, rate));
        break;
    }
    case 0xa2:   // [jump power] → type 19
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::JUMP_POWER, ScanInt((bool*)NULL), ScanInt((bool*)NULL)));
        break;
    case 0xa7:   // [physical critical hit] → type 20（浮点）
    {
        float value = ScanFloat((bool*)NULL);
        int rate = ScanInt((bool*)NULL);
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::PHYSICAL_CRITICAL_HIT, value, rate));
        break;
    }
    case 0xac:   // [magical critical hit] → type 21（浮点）
    {
        float value = ScanFloat((bool*)NULL);
        int rate = ScanInt((bool*)NULL);
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::MAGICAL_CRITICAL_HIT, value, rate));
        break;
    }
    default:
        return false;
    }
    return true;
}

// ORIG 0x088aedce loadScript。控制流忠实转写：外层 ScanType 失败即返回 0；
// "[upper]" / "[rare]" 分支内先 ScanType 再 parseAbility，parse 未命中回落外层；
// "[mystic circle]" 读一个整数到材料索引。打开脚本失败返回 -1。
int ARAD::SCRIPT::AvatarFixedHiddenOption::loadScript()
{
    if (!loadRDARScriptFile("Etc/Avatar_Roulette/", "AvatarFixedHiddenOptionList.etc"))
        return -1;

    std::string token;
    for (;;)
    {
        if (!ScanType(token, true))
            return 0;
        if (token == "[upper]")
        {
            for (;;)
            {
                if (!ScanType(token, true))
                    break;
                if (!parseAbility(m_abilities[2], token))   // ORIG this+0x18
                    break;
            }
        }
        else if (token == "[rare]")
        {
            for (;;)
            {
                if (!ScanType(token, true))
                    break;
                if (!parseAbility(m_abilities[3], token))   // ORIG this+0x24
                    break;
            }
        }
        else if (token == "[mystic circle]")
        {
            m_materialItemIndex = ScanInt((bool*)NULL);           // ORIG *(int*)(this+0x48)
        }
    }
}

// ============================================================================
// AvatarFixedHiddenOptionServer —— ORIG C1 0x0817e02c / D1 0x0817e0e6 /
// Load 0x0817e154 / Destroy 0x0817e1e2 / Init 0x0817e3e8
// ============================================================================

AvatarFixedHiddenOptionServer::AvatarFixedHiddenOptionServer()
{
}

AvatarFixedHiddenOptionServer::~AvatarFixedHiddenOptionServer()
{
}

// ORIG 0x0817e154：cMyTrace(__PRETTY_FUNCTION__,0x1a,9,true,false) +
// "\t- Loading Avatar Hidden Option - "；loadScript==0 且 Init()!=0 才算成功。
bool AvatarFixedHiddenOptionServer::Load()
{
    cMyTrace trace("virtual bool AvatarFixedHiddenOptionServer::Load()", 0x1a, 9, true, false);
    trace("\t- Loading Avatar Hidden Option - ");
    return m_script.loadScript() == 0 && Init() != 0;
}

// ORIG 0x0817e1e2：恒返回 1（接口 void 化同 CerashopAddRestrict::Manager 先例）。
void AvatarFixedHiddenOptionServer::Destroy()
{
}

// ORIG 0x0817e3e8：6 个桶各自把下标 1..count-1 的 m_nRate（元素 +0x10）累加成前缀和
// push 入 m_cumulativeRate[i]，之前 reserve(count-1)。恒返回 1。
int AvatarFixedHiddenOptionServer::Init()
{
    for (int i = 0; i < 6; ++i)
    {
        int cumulative = 0;
        int count = (int)m_script.m_abilities[i].size();
        m_cumulativeRate[i].reserve((std::vector<int>::size_type)(count - 1));
        for (int k = 1; k < count; ++k)
        {
            cumulative += m_script.m_abilities[i][k].m_nRate;
            m_cumulativeRate[i].push_back(cumulative);
        }
    }
    return 1;
}
