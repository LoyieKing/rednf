// CDataManager 脚本 POD 的构造/析构实现，独立于通用桩 TU.
#include "CDataManager.h"
#include "GameWorld.h"
#include "CItemGeneratorMgr.h"
#include "CStackableItem.h"
#include "CItem.h"
#include "GlobalData.h"
#include "CWorldMap.h"
#include <cstring>

#define ZERO_POD(T) T::T() { memset(this, 0, sizeof(*this)); } T::~T() {}
ZERO_POD(QuestParameterScript)
ZERO_POD(GuildParameterScript)
AccountCargoScript::AccountCargoScript()
    : m_reqLevel(0), m_itemIdx(0), m_upgradeMap()
{
}
AccountCargoScript::~AccountCargoScript() {}
ZERO_POD(PvPSkillTreeParameterScript)
ZERO_POD(IndependentDropParameterScript)
ZERO_POD(stMercenarySystemInfo)
channel_script_t::channel_script_t() : m_channelDungeonMap(), m_channelMap() {}
channel_script_t::~channel_script_t() {}
ZERO_POD(stBroadCastItemScript)
ZERO_POD(eventReward)
ZERO_POD(ConditionLevelChkDungeon)
ZERO_POD(FairPvPStatScript)
ZERO_POD(RegenerationROI)
ZERO_POD(stSeriaBlessingScript)
ZERO_POD(stLevelUpRewardItemScript)
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

CNPCScriptList::~CNPCScriptList() {}

namespace WongWork
{
CItemGeneratorMgr::~CItemGeneratorMgr() {}
void CItemGeneratorMgr::setDropRatio(float ratio)
{
    // ORIG：遍历 m_generators[8] 逐生成器下发。CItemGenerator 完整类型未落地
    // （仅前向声明，见 CItemGeneratorMgr.h），暂无法安全虚调用；保留空实现。
    (void)ratio;
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
// CTitleBook 通过 extern asm sub_checkTimeoutItem 调用：检查物品是否过期。
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

// ---- RecipeInfo（CStackableItem.h，含 vector 成员 → 默认构造）----
RecipeInfo::RecipeInfo() {}
RecipeInfo::~RecipeInfo() {}

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

// ---- growthEquipmentScript（含 map 成员 → 默认构造）----
growthEquipmentScript::growthEquipmentScript() {}
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

// ---- stRankSystemInfo / stDeathTower_t / stBloodDungeon_t / stVillageAttacked_t----
stRankSystemInfo::stRankSystemInfo() { memset(m_pad, 0, sizeof(m_pad)); }
stRankSystemInfo::~stRankSystemInfo() {}
stDeathTower_t::stDeathTower_t() { memset(m_pad, 0, sizeof(m_pad)); }
stBloodDungeon_t::stBloodDungeon_t() { memset(m_pad, 0, sizeof(m_pad)); }
stBloodDungeon_t::~stBloodDungeon_t() {}
stVillageAttacked_t::stVillageAttacked_t() { memset(m_pad, 0, sizeof(m_pad)); }
stVillageAttacked_t::~stVillageAttacked_t() {}

// ---- stQuestShop / WorldDropInfo / SlangNameData / InvalidCharData----
stQuestShop::stQuestShop() { memset(m_pad, 0, sizeof(m_pad)); }
stQuestShop::~stQuestShop() {}
WorldDropInfo::WorldDropInfo() { memset(m_pad, 0, sizeof(m_pad)); }
WorldDropInfo::~WorldDropInfo() {}
SlangNameData::SlangNameData() {}  // 含 2×vector 成员 → 默认构造
SlangNameData::~SlangNameData() {}
InvalidCharData::InvalidCharData() { memset(m_pad, 0, sizeof(m_pad)); }
InvalidCharData::~InvalidCharData() {}

// ---- STGrowthPowerData / AvatarColorInfo / BoosterGageData----
STGrowthPowerData::STGrowthPowerData() { memset(m_pad, 0, sizeof(m_pad)); }
STGrowthPowerData::~STGrowthPowerData() {}
AvatarVariation::AvatarColorInfo::AvatarColorInfo()
{
    memset(m_pad, 0, sizeof(m_pad));
}
AvatarVariation::AvatarColorInfo::~AvatarColorInfo() {}
BoosterGageData::BoosterGageData() { memset(m_pad, 0, sizeof(m_pad)); }
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

// ---- PcBangItemRentarData / DimensionActivationData----
PcBangItemRentarData::PcBangItemRentarData()
{
    memset(m_pad, 0, sizeof(m_pad));
}
PcBangItemRentarData::~PcBangItemRentarData() {}
DimensionActivationData::DimensionActivationData()
{
    memset(m_pad, 0, sizeof(m_pad));
}
DimensionActivationData::~DimensionActivationData() {}

// ---- stUnlimitChallengeInfo / stLimitItemUsageInfoEx / CNPCRelationEventManager----
stUnlimitChallengeInfo::stUnlimitChallengeInfo()
{
    memset(m_pad, 0, sizeof(m_pad));
}
stUnlimitChallengeInfo::~stUnlimitChallengeInfo() {}
stLimitItemUsageInfoEx::stLimitItemUsageInfoEx()
{
    memset(m_pad, 0, sizeof(m_pad));
}
stLimitItemUsageInfoEx::~stLimitItemUsageInfoEx() {}
CNPCRelationEventManager::CNPCRelationEventManager()
{
    memset(m_pad, 0, sizeof(m_pad));
}
CNPCRelationEventManager::~CNPCRelationEventManager() {}

// ---- stCharacLinkSystem_t----
stCharacLinkSystem_t::stCharacLinkSystem_t()
{
    memset(m_pad, 0, sizeof(m_pad));
}
stCharacLinkSystem_t::~stCharacLinkSystem_t() {}

// ---- advancealtar 四类（StageTimeLineParameter/AdvanceAltarShopParameter/
//      RewardParameter 清 pad；ConfigParameter 按 ORIG 0x889b662）----
advancealtar::StageTimeLineParameter::StageTimeLineParameter()
{
    memset(m_pad, 0, sizeof(m_pad));
}
advancealtar::StageTimeLineParameter::~StageTimeLineParameter() {}
advancealtar::AdvanceAltarShopParameter::AdvanceAltarShopParameter()
{
    memset(m_pad, 0, sizeof(m_pad));
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

// ---- PowerParameterScript（ORIG 0x08371a26 结构初始化）----
PowerParameterScript::PowerParameterScript()
{
    m_ghostTime = 0;
    m_penaltyThreshold = 0;
    m_pad1c = 0;
    memset(m_pad20, 0, sizeof(m_pad20));
    m_pad34 = 0;
    memset(m_pad38, 0, sizeof(m_pad38));
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
    memset(m_pad70, 0, sizeof(m_pad70));
    m_padA8 = 0;
    memset(m_padAC, 0, sizeof(m_padAC));
    m_powerWarCooldownMinutes = 0;
    memset(m_padBC, 0, sizeof(m_padBC));
    memset(m_padC8, 0, sizeof(m_padC8));
    m_padD4 = 0;
}
PowerParameterScript::~PowerParameterScript() {}

// ---- SecretShopScript（成员默认构造）----
SecretShopScript::SecretShopScript() {}
SecretShopScript::~SecretShopScript() {}

// ---- stTitleBookInfo / stItemMakingSkill----
stTitleBookInfo::stTitleBookInfo() { memset(m_pad, 0, sizeof(m_pad)); }
stTitleBookInfo::~stTitleBookInfo() {}
stItemMakingSkill::stItemMakingSkill() { memset(m_pad, 0, sizeof(m_pad)); }
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

// ---- live_server_info_t / EventEtcScript----
live_server_info_t::live_server_info_t() { memset(m_pad, 0, sizeof(m_pad)); }
live_server_info_t::~live_server_info_t() {}
EventEtcScript::EventEtcScript() { memset(m_pad, 0, sizeof(m_pad)); }
EventEtcScript::~EventEtcScript() {}

// ---- stReturnUserRewardScript / stBingoScript / stNewAccountLevelUpToJobScript----
stReturnUserRewardScript::stReturnUserRewardScript()
{
    memset(m_pad, 0, sizeof(m_pad));
}
stReturnUserRewardScript::~stReturnUserRewardScript() {}
stBingoScript::stBingoScript() { memset(m_pad, 0, sizeof(m_pad)); }
stBingoScript::~stBingoScript() {}
stNewAccountLevelUpToJobScript::stNewAccountLevelUpToJobScript()
{
    memset(m_pad, 0, sizeof(m_pad));
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

// ---- stConditionEventInfo----
stConditionEventInfo::stConditionEventInfo()
{
    memset(m_pad, 0, sizeof(m_pad));
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
