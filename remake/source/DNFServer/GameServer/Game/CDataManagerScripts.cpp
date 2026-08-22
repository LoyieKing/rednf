// CDataManager 脚本 POD 的构造/析构实现，独立于通用桩 TU.
#include "CDataManager.h"
#include "GameWorld.h"
#include "CItemGeneratorMgr.h"
#include "CStackableItem.h"
#include "CItem.h"
#include "GlobalData.h"
#include "CWorldMap.h"
#include "NPCScript.h"   // CNPCScript 完整类型（_destroy 中 delete）
#include "STExpertJobScript.h"  // 权威 STExpertJobScript（唯一声明点）
#include <cstring>

// ============================================================================
// 脚本 POD 生命周期（ORIG 语义，见 docs/class_func_reports 与 ORIG 反汇编）。
// 各 ctor 构造 STL 容器成员（必要时调 clear()），dtor 逆序析构。
// ============================================================================

#define ZERO_POD(T) T::T() { memset(this, 0, sizeof(*this)); } T::~T() {}

// ---- QuestParameterScript（ORIG C1 0x836f1c2 / clear 0x836f130 / D1 0x837d3ca）----
QuestParameterScript::QuestParameterScript()
{
    clear();
}
QuestParameterScript::~QuestParameterScript() {}
void QuestParameterScript::clear()
{
    m_map0.clear();
    m_field18 = 0;
    m_field1c = 0;
    m_field20 = 0;
    m_vec24.clear();
    m_vec30.clear();
    m_map3c.clear();
    m_map54.clear();
}

// ---- GuildParameterScript（ORIG C1 0x837058a / clear 0x83704ae / D1 0x837d542）----
GuildParameterScript::GuildParameterScript()
{
    clear();
}
GuildParameterScript::~GuildParameterScript() {}
void GuildParameterScript::clear()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
    m_fieldC = 0;
    m_map10.clear();
    m_field28 = 0;
    m_vec2c.clear();
    m_vec38.clear();
    m_vecE4.clear();
    m_vecF0.clear();
    m_fieldFC = 0.0;
    m_field104 = 0;
    m_field108 = 0;
    m_field10c = 0;
    m_field110 = 0.0;
    memset(m_levelUpCost, 0, sizeof(m_levelUpCost));  // ORIG：+0x5c memset 0x88
}

// ---- AccountCargoScript ----
AccountCargoScript::AccountCargoScript()
    : m_reqLevel(0), m_itemIdx(0), m_upgradeMap()
{
}
AccountCargoScript::~AccountCargoScript() {}

// ---- PvPSkillTreeParameterScript（ORIG C1 0x8371eee / clear 0x8371e5a / D1 0x837da02）----
PvPSkillTreeParameterScript::PvPSkillTreeParameterScript()
{
    clear();
}
PvPSkillTreeParameterScript::~PvPSkillTreeParameterScript() {}
void PvPSkillTreeParameterScript::clear()
{
    m_map0.clear();
    m_map18.clear();
    m_map30.clear();
    m_field48 = 0;
    m_field4c = 0;
    m_field50 = 0;
    m_map54.clear();
}

// ---- IndependentDropParameterScript（ORIG C1 0x8372d46 / clear 0x8372dfa / D1 0x837daac）----
IndependentDropParameterScript::IndependentDropParameterScript()
{
    clear();
}
IndependentDropParameterScript::~IndependentDropParameterScript() {}
void IndependentDropParameterScript::clear()
{
    m_vecC.clear();  // ORIG clear 仅清 +0x0c vector
}

// ---- stMercenarySystemInfo（ORIG C1 0x8373328 / D1 0x837db42）----
stMercenarySystemInfo::stMercenarySystemInfo()
    : m_field0(0xe10), m_field4(0xc8), m_vec8(), m_vec14(), m_vec20(),
      m_str2c(), m_vec30(), m_field3c(0), m_vec40()
{
}
stMercenarySystemInfo::~stMercenarySystemInfo() {}

// ---- channel_script_t ----
channel_script_t::channel_script_t() : m_channelDungeonMap(), m_channelMap() {}
channel_script_t::~channel_script_t() {}

// ---- stBroadCastItemScript / eventReward ----
stBroadCastItemScript::stBroadCastItemScript() : m_vec0(), m_mapC() {}
stBroadCastItemScript::~stBroadCastItemScript() {}

eventReward::eventReward() : m_map0()
{
    // ORIG C1 0x83738a2：构造 map + CSlotBoundChecker 后 clear map。
    // CSlotBoundChecker 重建为静态数组实现（无 per-instance 成员），+0x18 保持 pad。
    m_map0.clear();
}
eventReward::~eventReward()
{
    // ORIG D1 0x837dfb2：析构 map + CSlotBoundChecker（后者静态实现无成员，跳过）
}

// ---- ConditionLevelChkDungeon / FairPvPStatScript / RegenerationROI ----
ZERO_POD(ConditionLevelChkDungeon)
ZERO_POD(FairPvPStatScript)
RegenerationROI::RegenerationROI()
    : m_regeneratorPtr(0), m_regenerator(), m_expertJobList(0)
{
}
RegenerationROI::~RegenerationROI()
{
    // ORIG D1 0x85f8506：+0x00 非空则 delete，置 0，析构 +0x04 regenerator
    if (m_regeneratorPtr != 0)
    {
        ::operator delete(m_regeneratorPtr);
    }
    m_regeneratorPtr = 0;
}
ZERO_POD(stSeriaBlessingScript)

// stLevelUpRewardItemScript / stStepRewardStackableItemScript（ORIG D1
// 0x8371530 / 0x837151c：成员为 map，默认构造即可）
stLevelUpRewardItemScript::stLevelUpRewardItemScript() : m_map0() {}
stLevelUpRewardItemScript::~stLevelUpRewardItemScript() {}
stStepRewardStackableItemScript::stStepRewardStackableItemScript() : m_map0() {}
stStepRewardStackableItemScript::~stStepRewardStackableItemScript() {}
stGrowthCapsuleScript::stGrowthCapsuleScript() { memset(this, 0, sizeof(*this)); }
CLottery_NeedMoney::CLottery_NeedMoney() : m_needMoneyMap() {}
CLottery_NeedMoney::~CLottery_NeedMoney() {}

bool CLottery_NeedMoney::isNeedMoney(unsigned int itemIdx) const
{
    std::map<unsigned int, unsigned int>::const_iterator it = m_needMoneyMap.find(itemIdx);
    return it == m_needMoneyMap.end() ? false : it->second;
}

void CLottery_NeedMoney::add_lottery_needMoney(unsigned int itemIdx, unsigned int cost)
{
    if (m_needMoneyMap.find(itemIdx) == m_needMoneyMap.end())
        m_needMoneyMap.insert(std::make_pair(itemIdx, cost));
}

channel_info_t* channel_script_t::getChannelInfo(unsigned char channel, unsigned int idx) const
{
    // ORIG 0x088dcd38：外层 map(+0x18) 以 channel 查，内层以 channelNo/idx 查，返回 &channel_info_t。
    std::map<int, std::map<int, channel_info_t> >::const_iterator itOuter =
        m_channelMap.find((int)channel);
    if (itOuter == m_channelMap.end())
        return 0;
    std::map<int, channel_info_t>::const_iterator itInner =
        itOuter->second.find((int)idx);
    if (itInner == itOuter->second.end())
        return 0;
    return const_cast<channel_info_t*>(&itInner->second);
}

float channel_script_t::getBonusRate(unsigned char channel, unsigned int channelNo,
                                     unsigned int dungeonIndex) const
{
    // ORIG 0x088dce3c：getChannelInfo → 遍历 m_pBonusRates（vector<int>），命中则
    // 返回 (float)m_bonusRate / 1000.0f；否则 0.0f。
    channel_info_t* info = getChannelInfo(channel, channelNo);
    if (info == 0 || info->m_pBonusRates == 0)
        return 0.0f;
    std::vector<int>& rates = *info->m_pBonusRates;
    for (size_t i = 0; i < rates.size(); ++i)
    {
        if (rates[i] == (int)dungeonIndex)
            return (float)info->m_bonusRate / 1000.0f;
    }
    return 0.0f;
}

int CNPCScriptList::find(unsigned int idx) const
{
    __gnu_cxx::hash_map<unsigned int, CNPCScript*>::const_iterator it = m_npcMap.find(idx);
    return it == m_npcMap.end() ? 0 : reinterpret_cast<int>(it->second);
}

// ORIG _destroy 0x8581784：遍历 hash_map delete 各 CNPCScript*（调 D1 + operator delete），
// 再 clear()。
void CNPCScriptList::_destroy()
{
    __gnu_cxx::hash_map<unsigned int, CNPCScript*>::iterator it = m_npcMap.begin();
    while (it != m_npcMap.end())
    {
        CNPCScript* script = it->second;
        if (script != 0)
            delete script;
        ++it;
    }
    m_npcMap.clear();
}

CNPCScriptList::~CNPCScriptList()
{
    // ORIG D1 0x83762a8：_destroy() + stNPCCommonData_t D1 + hash_map D1
    _destroy();
}

namespace WongWork
{
CItemGeneratorMgr::~CItemGeneratorMgr() {}
void CItemGeneratorMgr::setDropRatio(float ratio)
{
    // ORIG 0x8534278：遍历 i=0..7，调用 m_generators[i] 虚表 +0x0（float 形参）。
    // CItemGenerator 完整类型未落地（仅前向声明），按虚表手动跳转。
    for (int i = 0; i < 8; ++i)
    {
        CItemGenerator* gen = m_generators[i];
        // gen → 虚表指针 → vtable[0]（第一个虚函数）
        void (*fn)(CItemGenerator*, float) =
            (*reinterpret_cast<void (***)(CItemGenerator*, float)>(gen))[0];
        fn(gen, ratio);
    }
}
// getGenerator 已由 CItemGeneratorMgr.cpp 真实定义，不重复。
}

WongWork::CItemGeneratorMgr* CDataManager::getItemGenerator()
{
    return m_itemGeneratorMgr;
}
// get_event_script_mng 已由 CDataManager.cpp 定义，不重复。
#undef ZERO_POD

// ============================================================================
// AccountCargoScript 查询（ORIG 0x88c8056 / 0x88c80ba / 0x88c8124）。
// 全部基于 +0x8 std::map<int, UpgradeInfo>：
//   GetCreateInfo：begin() 的 value（首个升级档），空 map 返回 0；
//   GetCurrUpgradeInfo(idx)：find(idx) 的 value，未找到返回 0；
//   GetNextUpgradeInfo(idx)：find(idx) 的下一项 value，无下一项返回 0。
// 返回 void* 以匹配 CAccountCargo.cpp 的 asm 别名调用。
// ============================================================================
void* AccountCargoScript::GetCreateInfo()
{
    if (m_upgradeMap.empty())
        return 0;
    return &m_upgradeMap.begin()->second;
}

void* AccountCargoScript::GetCurrUpgradeInfo(int idx)
{
    std::map<int, UpgradeInfo>::iterator it = m_upgradeMap.find(idx);
    if (it == m_upgradeMap.end())
        return 0;
    return &it->second;
}

void* AccountCargoScript::GetNextUpgradeInfo(int idx)
{
    std::map<int, UpgradeInfo>::iterator it = m_upgradeMap.find(idx);
    if (it == m_upgradeMap.end())
        return 0;
    ++it;
    if (it == m_upgradeMap.end())
        return 0;
    return &it->second;
}

// ============================================================================
// SecretShopScript::GetNpcIdx / GetNpcByDungeonIdx / GetNpcByDungeonLev
// （ORIG 0x8a82998 / 0x8a828f8 / 0x8a8293c）。
// GetNpcIdx：遍历 equal_range 区间，累加 stDungeonNpc::m_rate（+0x8），
// 首次累计 > target 时输出 m_npcIdx（+0x4）并返回 1，遍历完返回 0。
// ============================================================================
char SecretShopScript::GetNpcIdx(
    int& out, int target,
    std::pair<std::multimap<int, stDungeonNpc>::iterator,
              std::multimap<int, stDungeonNpc>::iterator>& range)
{
    int sum = 0;
    std::multimap<int, stDungeonNpc>::iterator it = range.first;
    while (it != range.second)
    {
        stDungeonNpc& npc = it->second;
        sum += npc.m_rate;
        if (sum > target)
        {
            out = npc.m_npcIdx;
            return 1;
        }
        ++it;
    }
    return 0;
}

char SecretShopScript::GetNpcByDungeonIdx(int& out, int a, int b)
{
    std::pair<std::multimap<int, stDungeonNpc>::iterator,
              std::multimap<int, stDungeonNpc>::iterator> range =
        m_npcMap.equal_range(b);
    return GetNpcIdx(out, a, range);
}

void SecretShopScript::GetNpcByDungeonLev(int& out, int a, int b)
{
    int levelIdx = GetLevelIdx(b);
    std::pair<std::multimap<int, stDungeonNpc>::iterator,
              std::multimap<int, stDungeonNpc>::iterator> range =
        m_levelNpcMap.equal_range(levelIdx);
    GetNpcIdx(out, a, range);
}

// ============================================================================
// DeleteInvalidItemScript 生命周期与查询。
// C1/D1（0x8ac1d40/0x8ac1dce）：构造 string +0x0/string +0x4/map +0x8 后
// clearScript()；IsInvalidItem2Delete（0x8ac2096）：map.find(idx) != end。
// ============================================================================
DeleteInvalidItemScript::DeleteInvalidItemScript()
{
}

DeleteInvalidItemScript::~DeleteInvalidItemScript()
{
}

bool DeleteInvalidItemScript::IsInvalidItem2Delete(int idx)
{
    return m_deleteMap.find(idx) != m_deleteMap.end();
}

// ============================================================================
// createStackableLotteryInfo（ORIG 0x89bacc1）：与 createLotteryInfo 同型，
// 但前两元素写入 m_defaultIdx/m_defaultCount，之后每三元素为一组
// (item, count, weight) 写入 m_items/m_counts。
// ============================================================================
void createStackableLotteryInfo(LotteryInfo* info, std::vector<int>* source)
{
    info->clear();
    std::vector<int>::iterator it = source->begin();
    info->m_defaultIdx = *it;
    ++it;
    info->m_defaultCount = *it;
    ++it;
    while (it != source->end())
    {
        std::pair<int, int> p;
        p.first = *it;
        ++it;
        if (it == source->end())
            break;
        p.second = *it;
        ++it;
        if (it == source->end())
            break;
        info->m_counts.push_back(*it);
        ++it;
        info->m_items.push_back(p);
    }
}

// ============================================================================
// createGlobalEffectInfo（ORIG 0x89bae94）：源为 int 三元组
// (field0, field4, rate*10000)；size 非 3 的倍数返回 0。修正了原桩错误的
// vector<char>& 形参（ORIG mangled 为 vector<stGlobalEffect_t>&）。
// ============================================================================
int createGlobalEffectInfo(std::vector<stGlobalEffect_t>& out,
                           std::vector<int>* source)
{
    out.clear();
    if (source->size() % 3 != 0)
        return 0;
    std::vector<int>::iterator it = source->begin();
    while (it != source->end())
    {
        stGlobalEffect_t effect;
        effect.m_field0 = *it;
        ++it;
        effect.m_field4 = *it;
        ++it;
        effect.m_field8 = static_cast<float>(*it) / 10000.0f;
        ++it;
        out.push_back(effect);
    }
    return 1;
}

// ============================================================================
// SetLightServerFlag / GetLightServerFlag（ORIG 0x8a8cba5 / 0x822ad3a）。
// ORIG 全局标志位于 0x940b558（.bss）；SetLightServerFlag 经
// GetLightServerFlag() 取指针后写入 bool。
// ============================================================================
namespace
{
bool g_lightServerFlag = false;   // ORIG 0x940b558
}

bool* GetLightServerFlag()
{
    return &g_lightServerFlag;
}

void SetLightServerFlag(bool flag)
{
    *GetLightServerFlag() = flag;
}

// ============================================================================
// _checkTimeoutItem（ORIG 0x867cf8d T，G3 任务域辅助）
// CTitleBook 通过 _checkTimeoutItem 直接调用：检查物品是否过期。
// ORIG 逻辑（逐步核对 0x867cf8d 反汇编）：
//   find_item(item.m_addInfo) 失败 → 0；usablePeriod/expirationDate 任一非 0 → 0；
//   虚表 +0x10（IsCreatureItem）非 0 → 0；
//   虚表 +0x48（IsRandomOption）非 0 且 item.m_field0 != 0 → 1；
//   虚表 +0xc（GetItemType）== 0xa 时：expirationDate <= now → 1；
//   否则（其他类型）：item.m_fieldb * 86400 + 0x44a54a80 <= now → 1；
//   其余 → 0。
// ============================================================================
int _checkTimeoutItem(const Inven_Item& item)
{
    CDataManager* dm = G_CDataManager();
    CItem* ci = dm->find_item(item.m_addInfo);
    if (ci == 0)
    {
        return 0;
    }
    if (ci->getUsablePeriod() != 0 || ci->getExpirationDate() != 0)
    {
        return 0;
    }
    if (ci->IsCreatureItem())
    {
        return 0;
    }
    if (ci->IsRandomOption() && item.m_field0 != 0)
    {
        return 1;
    }
    int now = GlobalData::s_systemTime_.getCurSec();
    if (ci->GetItemType() == 0xa)
    {
        if (ci->getExpirationDate() <= now)
        {
            return 1;
        }
    }
    else
    {
        int expire = item.m_fieldb * 0x15180 + 0x44a54a80;
        if (expire <= now)
        {
            return 1;
        }
    }
    return 0;
}

// ============================================================================
// _getIntegratedPvPItemAttr（ORIG T 0x84fc541，G3 PvP 物品属性）
// ORIG 反汇编（84fc541）：
//   it = G_CDataManager()->find_item(item.m_addInfo)；it==0 → 0；
//   虚表 +0xc（GetItemType）== 0xa → item.GetReSealCount() << 5；
//   0xb < GetItemType() <= 0x15 → item.GetReSealCount() << 5；
//   其余 → item.GetItemAttr()。
// ============================================================================
int _getIntegratedPvPItemAttr(const Inven_Item& item)
{
    CItem* it = G_CDataManager()->find_item(item.m_addInfo);
    if (it == 0)
    {
        return 0;
    }
    if (it->GetItemType() == 0xa)
    {
        return (int)item.GetReSealCount() << 5;
    }
    if (it->GetItemType() > 0xb && it->GetItemType() <= 0x15)
    {
        return (int)item.GetReSealCount() << 5;
    }
    return item.GetItemAttr();
}

// ============================================================================
// GetIntegratedPvPItemAttr（ORIG T 0x84fc5ff）
// ORIG 反汇编（84fc5ff）：
//   G_GameWorld()->IsIntegratedPvPBaseChannel() 且
//   G_CDataManager()->GetUpgradeRevisionPvPChannel() == 0 → _getIntegratedPvPItemAttr(item)
//   否则 → item.GetItemAttr()
// ============================================================================
int GetIntegratedPvPItemAttr(const Inven_Item& item)
{
    if (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
        G_CDataManager()->GetUpgradeRevisionPvPChannel() == 0)
    {
        return _getIntegratedPvPItemAttr(item);
    }
    return item.GetItemAttr();
}

// ============================================================================
// GameStubs.cpp 迁移：纯数据脚本类 ctor/dtor（符号匹配 ORIG，T）
// 权威声明见 CDataManager.h（seria/growth/create/use/rank/death/blood/village/
// questShop/WorldDrop/Slang/Invalid/GrowthPower/Booster/AvatarColor 等）、
// CStackableItem.h（RecipeInfo/MultiBoxLotteryInfo）、CEquipItem.h（RandomItemTable）、
// CItemList.h（BaseItemKey/BaseItemValue）。
// ============================================================================

// ---- RecipeInfo（CStackableItem.h；ORIG C1 0x8513c6a / clear 0x8513bf2 / D1 0x85144f4）----
RecipeInfo::RecipeInfo()
{
    clear();
}
RecipeInfo::~RecipeInfo() {}
void RecipeInfo::clear()
{
    m_vec0.clear();
    m_vecC.clear();
    m_vec18.clear();
    m_field24 = 0;
    m_field25 = 0;
    m_vec28.clear();
    m_field34 = 0;
    m_field38 = 0;
    m_field3c = 0;
    m_vec40.clear();
}

// ---- RandomItemTable（CEquipItem.h；ORIG W 0x8515468 / 0x8514b46）----
RandomItemTable::RandomItemTable() {}
RandomItemTable::~RandomItemTable() {}

// ---- MultiBoxLotteryInfo::LotteryUnit（namespace，含 2×vector 成员）----
MultiBoxLotteryInfo::LotteryUnit::LotteryUnit() {}
MultiBoxLotteryInfo::LotteryUnit::LotteryUnit(const LotteryUnit& other) {}
MultiBoxLotteryInfo::LotteryUnit::~LotteryUnit() {}

// ---- MultiBoxLotteryInfo::LotteryDefaultArad（3×uint 成员）----
MultiBoxLotteryInfo::LotteryDefaultArad::LotteryDefaultArad() {}
MultiBoxLotteryInfo::LotteryDefaultArad::LotteryDefaultArad(unsigned int, unsigned int,
                                                            unsigned int)
{
}
MultiBoxLotteryInfo::LotteryDefaultArad::~LotteryDefaultArad() {}

// ---- BaseItemKey / BaseItemValue（CItemList.h）----
BaseItemKey::BaseItemKey() {}
BaseItemKey::BaseItemKey(unsigned char, unsigned char, unsigned short)
{
}
BaseItemValue::BaseItemValue() : m_field0(0) {}
BaseItemValue::BaseItemValue(int value) : m_field0(value) {}

// ---- seriaRoomDecoEventScript（含 vector/map 成员 → 默认构造）----
seriaRoomDecoEventScript::seriaRoomDecoEventScript() {}
seriaRoomDecoEventScript::~seriaRoomDecoEventScript() {}

// ---- growthEquipmentScript（ORIG C1 0x8372fbc）----
growthEquipmentScript::growthEquipmentScript()
    : m_rewardMap(), m_giftBox(-1), m_materialItem(-1),
      m_materialMax(0), m_eventBubbleMax(0), m_eventSandMax(0)
{
    m_rewardMap.clear();  // ORIG C1 构造 map 后 clear
}
growthEquipmentScript::~growthEquipmentScript() {}

// ---- createChracScript（纯 pad）----
createChracScript::createChracScript() { memset(m_pad, 0, sizeof(m_pad)); }
createChracScript::~createChracScript() {}

// ---- useCharacterJobFatigue（含 map 成员 → 默认构造）----
useCharacterJobFatigue::useCharacterJobFatigue() {}
useCharacterJobFatigue::~useCharacterJobFatigue() {}

// ---- CharacterCreateCreatureGiftScript（含 map 成员 → 默认构造）----
CharacterCreateCreatureGiftScript::CharacterCreateCreatureGiftScript() {}
CharacterCreateCreatureGiftScript::~CharacterCreateCreatureGiftScript() {}

// ---- seriaRoomDecoAniEventScript（含 vector/map 成员 → 默认构造）----
seriaRoomDecoAniEventScript::seriaRoomDecoAniEventScript() {}
seriaRoomDecoAniEventScript::~seriaRoomDecoAniEventScript() {}

// ---- stRankSystemInfo（ORIG C1 0x8370eba / clear 0x8370f94 / D1 0x837ce4c）----
stDungeonRankSystem_t::stDungeonRankSystem_t() : m_map0(), m_vec18()
{
    clear();
}
stDungeonRankSystem_t::~stDungeonRankSystem_t() {}
void stDungeonRankSystem_t::clear()
{
    // ORIG 0x8370e34：清 map/vector + memset +0x24 0x12 字节
    m_map0.clear();
    m_vec18.clear();
    memset(m_rankLevel, 0, sizeof(m_rankLevel));
    memset(m_pad34, 0, sizeof(m_pad34));
}

stRankSystemInfo::stRankSystemInfo() : m_dungeonRank(), m_vec38(), m_map44(),
                                       m_vec5c(), m_vec68(), m_field74(0)
{
    clear();
}
stRankSystemInfo::~stRankSystemInfo() {}
void stRankSystemInfo::clear()
{
    m_vec38.clear();
    m_map44.clear();
    m_vec5c.clear();
    m_vec68.clear();
    m_field74 = 0;
}

// ---- stBloodDungeon_t（ORIG C1 0x8370a3a / clear 0x8370b02 / D1 0x8370ab0）----
stBloodDungeon_t::stBloodDungeon_t() : m_vec334(), m_map358()
{
    clear();
}
stBloodDungeon_t::~stBloodDungeon_t() {}
void stBloodDungeon_t::clear()
{
    // ORIG：+0x00..+0x31f 置 1.0f（200 float）；+0x320 = 1；
    // +0x324..+0x32f 清 0；+0x330 = 1.0f；+0x334 vector.clear()；
    // +0x340..+0x357 清 0；+0x358 map.clear()
    for (int i = 0; i < 200; ++i)
        m_rate[i] = 1.0f;
    m_field320 = 1;
    memset(m_field324, 0, sizeof(m_field324));
    m_field330 = 1.0f;
    m_vec334.clear();
    memset(m_pad340, 0, sizeof(m_pad340));
    m_map358.clear();
}
stVillageAttacked_t::stVillageAttacked_t() { memset(m_pad, 0, sizeof(m_pad)); }
stVillageAttacked_t::~stVillageAttacked_t() {}

// ---- stDeathTower_t（ORIG C1 0x08370970，0xc98 字节清零）----
stDeathTower_t::stDeathTower_t() { memset(m_pad, 0, sizeof(m_pad)); }

// ---- live_server_info_t（ORIG C1 0x0837dd64 / D1 0x0837dd78）----
live_server_info_t::live_server_info_t() { memset(m_pad, 0, sizeof(m_pad)); m_eventScriptMng = 0; }
live_server_info_t::~live_server_info_t() {}

// ---- stQuestShop / WorldDropInfo / SlangNameData / InvalidCharData----
stQuestShop::stQuestShop() { memset(m_pad, 0, sizeof(m_pad)); }
stQuestShop::~stQuestShop() {}
WorldDropInfo::WorldDropInfo() { memset(m_pad, 0, sizeof(m_pad)); }
WorldDropInfo::~WorldDropInfo() {}
SlangNameData::SlangNameData() {}  // 含 2×vector 成员 → 默认构造
SlangNameData::~SlangNameData() {}
InvalidCharData::InvalidCharData()
    : m_charList(), m_charRange(), m_shortList(), m_shortRange(), m_slangFilter(0)
{
}
InvalidCharData::~InvalidCharData() {}

// ---- STGrowthPowerData（ORIG C1 0x83711fe / D1 0x837d290）----
STGrowthPowerData::STGrowthPowerData() : m_vec0(), m_vecC(), m_map18()
{
    // ORIG C1 构造后 clear 各容器
    m_vec0.clear();
    m_vecC.clear();
    m_map18.clear();
}
STGrowthPowerData::~STGrowthPowerData() {}

// ---- AvatarColorInfo（ORIG D1 0x837d358）----
AvatarVariation::AvatarColorInfo::AvatarColorInfo() : m_colorMap(), m_nameMap()
{
}
AvatarVariation::AvatarColorInfo::~AvatarColorInfo() {}
BoosterGageData::BoosterGageData()
    : m_BoosterPoint(), m_cMaxBoosterGage(0), m_cFieldD(0), m_cMultiboxCount(0)
{
}
BoosterGageData::~BoosterGageData() {}

// ============================================================================
// GameStubs.cpp 迁移（第二批）：其余纯数据脚本类 ctor/dtor（符号匹配 ORIG，T）
// ============================================================================

// ---- reseal_table_t（含 vector + 2×int[6]；ORIG 0x8370742）----
reseal_table_t::reseal_table_t()
{
    for (int i = 0; i <= 5; ++i)
        m_arr1[i] = 0;
    for (int i = 0; i <= 5; ++i)
        m_arr2[i] = 0;
}
reseal_table_t::~reseal_table_t() {}

// ---- InGameAdvertisementScript（无析构声明）----
InGameAdvertisementScript::InGameAdvertisementScript()
{
    memset(m_pad, 0, sizeof(m_pad));
}

// ---- PcBangItemRentarData（ORIG D1 0x837c2c0）----
PcBangItemRentarData::PcBangItemRentarData() : m_vec0(), m_mapC()
{
}
PcBangItemRentarData::~PcBangItemRentarData() {}

// ---- DimensionActivationData（ORIG C1 0x8371422 / D1 0x837c306）----
DimensionActivationData::DimensionActivationData()
    : m_field0(0), m_coinMap(), m_rewardData()
{
    // ORIG C1：+0x00 = 0，构造 map/rewardData 后清 coinMap
    m_coinMap.clear();
}
DimensionActivationData::~DimensionActivationData() {}

// ---- stUnlimitChallengeInfo（ORIG C1 0x837c53c / D1 0x837c5fa）----
stUnlimitChallengeInfo::stUnlimitChallengeInfo()
    : m_field0(0), m_vec4(), m_vec10(), m_vec1c(), m_vec28(), m_multiMap34()
{
}
stUnlimitChallengeInfo::~stUnlimitChallengeInfo() {}

// ---- stLimitItemUsageInfoEx（ORIG D1 0x837c7a2）----
stLimitItemUsageInfoEx::stLimitItemUsageInfoEx()
    : m_info0(), m_resetC(), m_refill18()
{
}
stLimitItemUsageInfoEx::~stLimitItemUsageInfoEx() {}

// ---- CNPCRelationEventManager（ORIG D1 0x890ce34：遍历 map 删除节点后析构 map）----
CNPCRelationEventManager::CNPCRelationEventManager()
    : m_npcEventMap(), m_itemLimitEditionMgr(0)
{
}
CNPCRelationEventManager::~CNPCRelationEventManager()
{
    std::map<unsigned short, stNPCRelationEventInfo*>::iterator it =
        m_npcEventMap.begin();
    while (it != m_npcEventMap.end())
    {
        stNPCRelationEventInfo* node = it->second;
        if (node != 0)
            delete node;
        ++it;
    }
    m_npcEventMap.clear();
}

// ---- stCharacLinkSystem_t（ORIG C1 0x837cf46 / D1 0x837cfde）----
stCharacLinkSystem_t::stCharacLinkSystem_t()
    : m_field0(0), m_vec4(), m_vec10(), m_vec1c(), m_vec28()
{
}
stCharacLinkSystem_t::~stCharacLinkSystem_t() {}

// ---- advancealtar（StageTimeLineParameter D1 0x8899e88 /
//      AdvanceAltarShopParameter D1 0x88a0fa2：析构 map 成员；
//      RewardParameter 清 pad；ConfigParameter 按 ORIG 0x889b662）----
advancealtar::StageTimeLineParameter::StageTimeLineParameter()
    : m_summonMap(), m_actionMap(), m_timeLine()
{
}
advancealtar::StageTimeLineParameter::~StageTimeLineParameter() {}
advancealtar::AdvanceAltarShopParameter::AdvanceAltarShopParameter()
    : m_buyShopMap(), m_starRestMap()
{
}
advancealtar::AdvanceAltarShopParameter::~AdvanceAltarShopParameter() {}
advancealtar::RewardParameter::RewardParameter()
{
    memset(m_pad, 0, sizeof(m_pad));
}
advancealtar::RewardParameter::~RewardParameter() {}
advancealtar::ConfigParameter::ConfigParameter()
{
    // ORIG 0x889b662
    m_0 = 0;
    m_4 = 0;
    m_strPtr = m_str;
    memset(m_str, 0, 0x10);
    memset(m_str2, 0, 0x10);
}
advancealtar::ConfigParameter::~ConfigParameter() {}

// ---- PowerParameterScript（ORIG C1 0x08371a26 结构初始化 / D1 0x0837d634）----
PowerParameterScript::PowerParameterScript()
{
    m_ghostTime = 0;
    m_penaltyThreshold = 0;
    m_pad1c = 0;
    m_pad34 = 0;
    m_pad44 = 0;
    m_pad48 = 0;
    m_pad4c = 0;
    m_pad50 = 0;
    m_pad54 = 0;
    m_pad58 = 0;
    m_pad5c = 5;  // ORIG clear() 默认值
    m_pad60 = 5;  // ORIG clear() 默认值
    m_minEnterLevel = 0;
    m_minEnterAge = 0;
    m_pad6c = 0;
    m_padA8 = 0;
    m_powerWarCooldownMinutes = 0;
    m_padD4 = 0;
}
PowerParameterScript::~PowerParameterScript()
{
    // ORIG D1：逆序析构 7 个容器成员（+0xc8/+0xbc/+0xac/+0x9c/+0x70/+0x38/+0x20）
}

// ---- SecretShopScript（成员默认构造）----
SecretShopScript::SecretShopScript() {}
SecretShopScript::~SecretShopScript() {}

// ---- stTitleBookInfo（ORIG C1 0x837d828 / D1 0x837d906）----
stTitleBookInfo::stTitleBookInfo()
    : m_vec0(), m_vecC(), m_map18(), m_vec30(), m_vec3c(), m_vec48()
{
}
stTitleBookInfo::~stTitleBookInfo() {}

// ---- stItemMakingSkill（ORIG D1 0x837dce2）----
stItemMakingSkill::stItemMakingSkill() : m_map0(), m_vec18()
{
}
stItemMakingSkill::~stItemMakingSkill() {}

// ---- ChoiceItemInfo / ImageCommunicationData（无析构声明）----
ChoiceItemInfo::ChoiceItemInfo()
{
    memset(this, 0, sizeof(*this));
}
ImageCommunicationData::ImageCommunicationData()
{
    memset(m_pad, 0, sizeof(m_pad));
}

// ---- ARAD::SCRIPT::AradJumping_Script----
ARAD::SCRIPT::AradJumping_Script::AradJumping_Script()
{
    memset(m_pad, 0, sizeof(m_pad));
}
ARAD::SCRIPT::AradJumping_Script::~AradJumping_Script() {}

// ---- EventEtcScript（ORIG C1 0x8372e66 / clear 0x8372ece / D1 0x837dda0）----
EventEtcScript::EventEtcScript() : m_vec0(), m_mapC()
{
    clear();
}
EventEtcScript::~EventEtcScript() {}
void EventEtcScript::clear()
{
    m_vec0.clear();
    m_vec0.reserve(7);
}

// ---- stReturnUserRewardScript / stBingoScript / stNewAccountLevelUpToJobScript----
stReturnUserRewardScript::stReturnUserRewardScript()
{
    memset(m_pad, 0, sizeof(m_pad));
}
stReturnUserRewardScript::~stReturnUserRewardScript() {}
stBingoScript::stBingoScript() : m_map0(), m_map18(), m_map30(), m_vec48()
{
    // ORIG C1：构造 4 容器后各自 clear()
    m_map0.clear();
    m_map18.clear();
    m_map30.clear();
    m_vec48.clear();
}
stBingoScript::~stBingoScript() {}
stNewAccountLevelUpToJobScript::stNewAccountLevelUpToJobScript()
    : m_stepReward(), m_levelUp18(), m_levelUp30()
{
}
stNewAccountLevelUpToJobScript::~stNewAccountLevelUpToJobScript() {}
// ---- CWorldMapList / StageMapList / WarAreaCounter----
// ORIG CWorldMapList（0x837480a ctor / 0x837481e dtor / 0x8374752 destroy）即一个
// map<int,CWorldMap*>，成员 m_map 位于偏移 0。
CWorldMapList::CWorldMapList() : m_map() {}
CWorldMapList::~CWorldMapList() {}

// ORIG 0x83647a2：map<int,CWorldMap*>::find(idx)，命中返回 second，否则 0。
CWorldMap* CWorldMapList::find_world_map(int idx) const
{
    std::map<int, CWorldMap*>::const_iterator it = m_map.find(idx);
    return (it == m_map.end()) ? 0 : it->second;
}

// ---- CWorldMap 访问器（ORIG 无独立报告；按 objdump 直读字节字段实现） ----
bool CWorldMap::IsInHellDungeon()   // ORIG W 0x830e6ba：movzbl 0x4(%eax)（非 const）
{
    return m_isHellDungeon != 0;
}

bool CWorldMap::hasDeathTower() const     // ORIG W 0x822b520：movzbl 0xc(%eax)
{
    return m_hasDeathTower != 0;
}

StageMapList::StageMapList() { memset(m_pad, 0, sizeof(m_pad)); }
StageMapList::~StageMapList() {}
WarAreaCounter::WarAreaCounter()
{
    // 布局 = 具名成员（CDataManager.h），全部零初始化
    memset(m_timeHourList, 0, sizeof(m_timeHourList));
    m_curHourIdx = 0;
    memset(m_pad_ec, 0, sizeof(m_pad_ec));
    m_num = 0;
    memset(m_warRoomCount, 0, sizeof(m_warRoomCount));
    memset(m_pad_2d4, 0, sizeof(m_pad_2d4));
}
WarAreaCounter::~WarAreaCounter() {}

// ---- stEquipmentGradeSample（4×vector<uint> 默认构造；ORIG 0x837c810）----
stEquipmentGradeSample::stEquipmentGradeSample()
{
}
stEquipmentGradeSample::~stEquipmentGradeSample() {}

// ---- stOneEmotionData / stEmotionTypeTagsInfo（自 GameStubs.cpp 迁移）----
// ORIG 0x891b560：构造 strings + vector<string>，再 clear
stOneEmotionData::stOneEmotionData() : m_8(), m_c(), m_14(), m_18()
{
    clear();
}
stOneEmotionData::~stOneEmotionData() {}
void stOneEmotionData::clear()
{
    // ORIG 0x891b61a
    m_0 = 0;
    m_4 = 0;
    m_8.clear();
    m_c.clear();
    m_10 = 0;
    m_11 = 0;
    m_12 = 0;
    m_14.clear();
    m_18.clear();
}

// ORIG 0x8370c4c：构造两个 vector 后 clear
stEmotionTypeTagsInfo::stEmotionTypeTagsInfo() : m_tags(), m_data()
{
    clear();
}
stEmotionTypeTagsInfo::~stEmotionTypeTagsInfo() {}
void stEmotionTypeTagsInfo::clear()
{
    // ORIG 0x8370cfa
    m_tags.clear();
    m_data.clear();
}

// ---- stConditionEventInfo（ORIG D1 0x837c938）----
stConditionEventInfo::stConditionEventInfo()
    : m_field0(0), m_field4(0), m_map8(), m_multiMap20(), m_multiMap38()
{
}
stConditionEventInfo::~stConditionEventInfo() {}

// ---- pvp_channel_info_t（m_pad8 + m_maxGrade + m_upgradeRevision + m_pad10 + m_pvpMissionSystem）----
pvp_channel_info_t::pvp_channel_info_t()
{
    memset(m_pad8, 0, sizeof(m_pad8));
    m_maxGrade = 0;
    m_upgradeRevision = 0;
    memset(m_pad10, 0, sizeof(m_pad10));
    memset(&m_pvpMissionSystem, 0, sizeof(m_pvpMissionSystem));
}

// ============================================================================
// STExpertJobScript::isBoundaryExpValue（TSV 修复，ORIG 0x0849f420 T）。
// ORIG：遍历 vector<ExpertJobExp>（元素尺寸 12，见 _ZNSt6vectorI12ExpertJobExpSaIS0_EEixEj
// 乘法 i*12），任一元素首整数字段 == exp 返回 true，否则 false。
// 权威声明见 STExpertJobScript.h（CUser::GetCurExpertJobLevel 真实调用）。
// ============================================================================

bool STExpertJobScript::isBoundaryExpValue(int exp)
{
    for (size_t i = 0; i < m_expVec.size(); ++i)
    {
        if (m_expVec[i].m_exp == exp)
            return true;
    }
    return false;
}

// STExpertJobScript::GetLevel（TSV 修复发明符号：_ZN17STExpertJobScript8GetLevelEj，ORIG 0x08693ba0）。
// ORIG：遍历 vector<ExpertJobExp>，返回首个 v[i].m_exp > exp 的 i+1；无则返回 size()。
int STExpertJobScript::GetLevel(unsigned int exp)
{
    for (size_t i = 0; i < m_expVec.size(); ++i)
    {
        if ((unsigned int)m_expVec[i].m_exp > exp)
            return (int)i + 1;
    }
    return (int)m_expVec.size();
}
