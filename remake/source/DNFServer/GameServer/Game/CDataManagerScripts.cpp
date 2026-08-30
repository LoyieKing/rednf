// CDataManager 脚本 POD 的构造/析构实现，独立于通用桩 TU.
#include "CBoosterGage.h"  // BoosterGageData 权威布局（须先于 CDataManager.h）
#include "CDataManager.h"
#include "GameWorld.h"
#include "CItemGeneratorMgr.h"
#include "CStackableItem.h"
#include "CItem.h"
#include "GlobalData.h"
#include "CWorldMap.h"
#include "NPCScript.h"   // CNPCScript 完整类型（_destroy 中 delete）
#include "STExpertJobScript.h"  // 权威 STExpertJobScript（唯一声明点）
#include "DNFLexWrapper.h"   // ScanType/ScanInt/ScanStr/ScanFloat/loadRDARScriptFile/
                             // unloadRDARScriptFile（RDAR 脚本扫描接口）
#include "Statuepredicate.h"  // pieceQuestRewardStatus 合并谓词（getRewardStatus）
#include "CDungeon.h"        // advancealtar::{StageDifficulty,RewardItemType}
                             // （importAchievementRewardParameter 解析用）
#include <cstring>

// ORIG _Z22loadRDARScriptListFileP16STScriptFileListPKcS2_PSt3mapIiiSt4lessIiESaISt4pairIKiiEEE
// 0x088bbd77 T（定义 STScriptFileList.cpp；DNFLexWrapper.h 未含此声明）。
int loadRDARScriptListFile(STScriptFileList* list, const char* dir,
                           const char* listPath, std::map<int, int>* unused);
// 频道类型 ∈ {4,6,5,7,8,9,10,13,14,15,16} 返回真，否则假。
// 调用侧（GetPvPChannelNumber）按 bool 语义 test %al 使用；定义者按 ORIG
// cmp/je 链 + mov $1/mov $0 分支形态实现即可（int/bool 返回符号相同）。
bool IS_PVP_CHANNEL(int channelType);
#include <algorithm>     // std::find（CWorldMap::hasDungeon）

// ============================================================================
// 脚本 POD 生命周期（ORIG 语义，见 docs/class_func_reports 与 ORIG 反汇编）。
// 各 ctor 构造 STL 容器成员（必要时调 clear()），dtor 逆序析构。
// ============================================================================

#define ZERO_POD(T) T::T() { memset(this, 0, sizeof(*this)); } T::~T() {}

QuestParameterScript::QuestParameterScript()
{
    clear();
}
QuestParameterScript::~QuestParameterScript() {}

void QuestParameterScript::clear()
{
    m_map0.clear();
    m_vec24.clear();
    m_vec30.clear();
    m_map3c.clear();
    m_map54.clear();
    m_field18 = 0;
    m_field1c = 0;
    m_field20 = 0;
}

// ============================================================================
// QuestParameterScript 奖励段查询（ORIG T 0x08a6bdc6 / 0x08a6be66）。
// getRewardLevel：遍历 m_map3c（map<int,int>），取首个 second<=level 的
// first（初始 0）；getRewardStatus：getRewardLevel(level) 后遍历 m_map54 中
// key<=level 的各 vector，按 Statuepredicate（+0x0/+0x4 两 int）合并：
// 命中段累加 m_value（+0x8 float），否则 push_back（sret 返回）。
// ============================================================================
int QuestParameterScript::getRewardLevel(int level)
{
    int result = 0;
    std::map<int, int>::iterator it = m_map3c.begin();
    for (; it != m_map3c.end(); ++it)
    {
        if (it->second <= level)
        {
            result = it->first;
        }
    }
    return result;
}

std::vector<pieceQuestRewardStatus> QuestParameterScript::getRewardStatus(
    int level)
{
    std::vector<pieceQuestRewardStatus> result;
    int rewardLevel = getRewardLevel(level);
    if (rewardLevel == -1)
        return result;

    for (std::map<int, std::vector<pieceQuestRewardStatus> >::iterator it =
             m_map54.begin(); it != m_map54.end(); ++it)
    {
        if (rewardLevel < it->first)
            break;

        const std::vector<pieceQuestRewardStatus>& section = it->second;
        for (std::vector<pieceQuestRewardStatus>::const_iterator sit =
                 section.begin(); sit != section.end(); ++sit)
        {
            Statuepredicate pred(*sit);
            std::vector<pieceQuestRewardStatus>::iterator found =
                std::find_if(result.begin(), result.end(), pred);
            if (found != result.end())
            {
                found->m_value += sit->m_value;
            }
            else
            {
                result.push_back(*sit);
            }
        }
    }
    return result;
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

// ORIG C1 0x8372480：仅构造 +0x08 map 后调 clear()（+0x1c call；+0/+4 由 clear 内清零）。
AccountCargoScript::AccountCargoScript()
    : m_upgradeMap()
{
    clear();
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
// ============================================================================
// getPvPSkillPointNext（ORIG T 0x08a5df0e，const）：
// m_map18[grade] 与 m_map18[grade-1] 中 (job*5+firstGrow) 的投入点数差，
// 再减 m_map30[(job,firstGrow)] 的 m_pointCost。secondGrow 不参与
// （ORIG 写入 0 后仅用于 key 第三字节，恒 0）。
// ============================================================================
int PvPSkillTreeParameterScript::getPvPSkillPointNext(
    int job, int firstGrow, int secondGrow, int grade) const
{
    (void)secondGrow;
    int points = 0;
    std::map<int, std::map<int, int> >::const_iterator gradeIt =
        m_map18.find(grade);
    if (gradeIt != m_map18.end())
    {
        std::map<int, int>::const_iterator slotIt =
            gradeIt->second.find(job * 5 + firstGrow);
        if (slotIt != gradeIt->second.end())
            points = slotIt->second;
    }

    int prevPoints = 0;
    std::map<int, std::map<int, int> >::const_iterator prevIt =
        m_map18.find(grade - 1);
    if (prevIt != m_map18.end())
    {
        std::map<int, int>::const_iterator slotIt =
            prevIt->second.find(job * 5 + firstGrow);
        if (slotIt != prevIt->second.end())
            prevPoints = slotIt->second;
    }

    PvPSkillTreeKey key;
    key.m_a = static_cast<unsigned char>(job);
    key.m_b = static_cast<unsigned char>(firstGrow);
    int cost = 0;
    std::map<PvPSkillTreeKey,
             FAIR_PVP_SKILL_TREE_SKILL_PER_POINT>::const_iterator costIt =
        m_map30.find(key);
    if (costIt != m_map30.end())
        cost = costIt->second.m_pointCost;

    return (points - prevPoints) - cost;
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

// ORIG C1 0x8371694：构造 vector +0x0/map +0xc 后各自 clear()（+0x27/+0x35 call）。
stBroadCastItemScript::stBroadCastItemScript() : m_vec0(), m_mapC()
{
    m_vec0.clear();
    m_mapC.clear();
}
stBroadCastItemScript::~stBroadCastItemScript() {}

// ORIG isUnCommonItem 0x8918d9c：m_mapC.find(itemIdx) != end()
bool stBroadCastItemScript::isUnCommonItem(int itemIdx) const
{
    return m_mapC.find(itemIdx) != m_mapC.end();
}

// ORIG checkCommonItem 0x8918cdc：遍历 m_vec0 找 itemIdx
bool stBroadCastItemScript::checkCommonItem(int itemIdx) const
{
    for (std::vector<int>::const_iterator it = m_vec0.begin(); it != m_vec0.end(); ++it)
    {
        if (*it == itemIdx)
        {
            return true;
        }
    }
    return false;
}

// ORIG checkUnCommonNestedItem 0x8918dfe：m_mapC.find(itemIdx) 命中后遍历其子 vector 找 subItemIdx
bool stBroadCastItemScript::checkUnCommonNestedItem(int itemIdx, int subItemIdx) const
{
    std::map<int, std::vector<int> >::const_iterator it = m_mapC.find(itemIdx);
    if (it == m_mapC.end())
    {
        return false;
    }
    const std::vector<int>& v = it->second;
    for (std::vector<int>::const_iterator vit = v.begin(); vit != v.end(); ++vit)
    {
        if (*vit == subItemIdx)
        {
            return true;
        }
    }
    return false;
}

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
// ORIG C1 0x837c9ac / D1 0x837c9c0（本 TU 唯一 owner）：仅构造/析构 +0x00 的
// vector<int>；objdump 直转，标量成员无动作。
ConditionLevelChkDungeon::ConditionLevelChkDungeon()
    : m_unableDungeons()
{
}
ConditionLevelChkDungeon::~ConditionLevelChkDungeon() {}
// ORIG C1 0x83724f0：map 构造后 clear()；D1 仅析构 map；clear 0x83724c8 清空 map。
FairPvPStatScript::FairPvPStatScript()
{
    m_map0.clear();
}
FairPvPStatScript::~FairPvPStatScript() {}
void FairPvPStatScript::clear() { m_map0.clear(); }
// ORIG _ZN28RandomOptionItem_RegeneratorC1Ev 0x85f971a T：四容器构造后 +0x48=0。
RandomOptionItem_Regenerator::RandomOptionItem_Regenerator()
    : m_map0(), m_map18(), m_vec30(), m_vec3c()
{
    m_field48 = 0;
}

RegenerationROI::RegenerationROI()
    : m_regenerator()
{
    // ORIG C1 0x85f8486：构造 +0x04 regenerator 后 new CMTRand(0x9c8) 存 +0x00；
    // 不初始化 +0x50。
    m_regeneratorPtr = new CMTRand();
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
// ORIG C1/D1：map<int,Value> 默认构造/析构（原 pad 占位时的 ZERO_POD 已随真实布局移除）。
stSeriaBlessingScript::stSeriaBlessingScript() {}
stSeriaBlessingScript::~stSeriaBlessingScript() {}

// ORIG W 0x822af22 is_possible_lv：FindOneByLv(0x822aebe 存 lv) 谓词
// （0x822aecc）：min==max → lv==min；否则 min<=lv<max。
bool stSeriaBlessingScript::is_possible_lv(int lv)
{
    FindOneByLv pred(lv);                     // ORIG：谓词在入口即构造
    std::map<int, Value>::iterator it = m_map0.begin();
    std::map<int, Value>::iterator itEnd = m_map0.end();
    std::map<int, Value>::iterator found = std::find_if(it, itEnd, pred);  // ORIG：返回值独立变量
    return found != m_map0.end();
}

// ORIG C1 0x83724f0 / FindCharacStat 0x8693c66：map.find(CharacterKind(job,grow))，
// 命中返回 &it->second，未命中返回 0（迭代器 != end 判定）。
CharacterStatus* FairPvPStatScript::FindCharacStat(int job, int growType)
{
    std::map<CharacterKind, CharacterStatus>::iterator it =
        m_map0.find(CharacterKind((unsigned char)job, (unsigned char)growType));
    if (it != m_map0.end())
        return &it->second;
    return 0;
}

// ORIG W 0x822b07e get_break_away_section_data(level)：sret；
// 返回最后一个 elem.m_field1 <= level 的段拷贝（memmove 3 字节；
// 一旦 m_field1>level 即退出，未命中返回清零段）。
STBreakAwaySection STGrowthPowerData::get_break_away_section_data(int level)
{
    STBreakAwaySection out;
    for (std::vector<STBreakAwaySection>::const_iterator it = m_vec0.begin();
         it != m_vec0.end(); ++it)
    {
        if (it->m_field1 > level)
            break;
        memmove(&out, &*it, sizeof(STBreakAwaySection));
    }
    return out;
}

// ORIG W 0x822afd6 get_break_away_section_low_level(lv)：返回最后一个
// m_field1<=lv 段的 m_field1；首个 m_field1>lv 即返回 -1（循环内直返）。
int STGrowthPowerData::get_break_away_section_low_level(int lv)
{
    int result = -1;
    for (std::vector<STBreakAwaySection>::const_iterator it = m_vec0.begin();
         it != m_vec0.end(); ++it)
    {
        if (it->m_field1 > lv)
            return result;
        result = it->m_field1;
    }
    return result;
}

// ORIG C1 0x8371194 / 0x83714c6：map 构造后 clear()。
stLevelUpRewardItemScript::stLevelUpRewardItemScript()
{
    m_map0.clear();
}
stLevelUpRewardItemScript::~stLevelUpRewardItemScript() {}
stStepRewardStackableItemScript::stStepRewardStackableItemScript()
{
    m_map0.clear();
}
stStepRewardStackableItemScript::~stStepRewardStackableItemScript() {}
stGrowthCapsuleScript::stGrowthCapsuleScript()
{
    // ORIG C1 0x0837114a：五个 int 成员逐个清零（非 memset）。
    m_maxGageExp = 0;
    m_gagePercent = 0;
    m_capsuleId = 0;
    m_capsuleExp = 0;
    m_capsuleLimitLv = 0;
}
CLottery_NeedMoney::CLottery_NeedMoney() : m_needMoneyMap() {}
CLottery_NeedMoney::~CLottery_NeedMoney() {}

bool CLottery_NeedMoney::isNeedMoney(unsigned int itemIdx) const
{
    static std::map<unsigned int, unsigned int>::const_iterator _itr;  // ORIG _ZZNK…isNeedMoneyEjE4_itr
    _itr = m_needMoneyMap.find(itemIdx);
    return _itr == m_needMoneyMap.end() ? false : _itr->second;
}

void CLottery_NeedMoney::add_lottery_needMoney(unsigned int itemIdx, unsigned int cost)
{
    static std::map<unsigned int, unsigned int>::iterator _itr;  // ORIG _ZZN…add_lottery_needMoneyEjjE4_itr
    _itr = m_needMoneyMap.find(itemIdx);
    if (_itr == m_needMoneyMap.end())
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

int channel_script_t::GetPvPChannelNumber(unsigned char channel, unsigned int channelNo) const
{
    // ORIG 0x088dcc28：外层 map(+0x18) 以 channel(serverNo) 查，未命中返回 -1；
    // 内层按 channelNo 升序遍历，每遇 PVP 频道（IS_PVP_CHANNEL(m_channelType)）
    // 计数 +1（-1 起步），命中 channelNo 即返回当前计数；遍历完未命中返回 -1。
    std::map<int, std::map<int, channel_info_t> >::const_iterator itOuter =
        m_channelMap.find((int)channel);
    if (itOuter == m_channelMap.end())
        return -1;
    const std::map<int, channel_info_t>* pInner = &itOuter->second;
    int pvpIndex = -1;
    for (std::map<int, channel_info_t>::const_iterator it = pInner->begin();
         it != pInner->end(); ++it)
    {
        if (IS_PVP_CHANNEL(it->second.m_channelType))
            ++pvpIndex;
        if (it->first == (int)channelNo)
            return pvpIndex;
    }
    return -1;
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
        if (rates.at(i) == (int)dungeonIndex)
            return (float)info->m_bonusRate / 1000.0f;
    }
    return 0.0f;
}

// ORIG T 0x08581680：hash_map 命中返回 CNPCScript*，未命中返回 0。
CNPCScript* CNPCScriptList::find(unsigned int idx) const
{
    __gnu_cxx::hash_map<unsigned int, CNPCScript*>::const_iterator it =
        m_npcMap.find(idx);
    if (it == m_npcMap.end())
        return 0;
    return it->second;
}

// ORIG _ZNK14CNPCScriptList16getMaxFavorValueEv W 0x8582364：
// movzwl 0x14(%eax)（m_npcCommon.m_maxFavorExp）+ movzwl %ax 零扩展，int 返回。
int CNPCScriptList::getMaxFavorValue() const
{
    return m_npcCommon.m_maxFavorExp;
}

// ORIG _ZNK14CNPCScriptList13getFavorLevelEt 0x85816e4：favorExp 低于上限时按
// m_favorExp[1..3] 阈值表（ushort 无符号比较）返回 1..3，否则 4。
int CNPCScriptList::getFavorLevel(unsigned short favorExp) const
{
    // 帧对齐：ORIG sub $0x34（Ghidra 未显示的死区局部，P31 dummy 对齐）。
    char pad[24];
    if (favorExp < getMaxFavorValue())
    {
        for (int i = 1; i <= 3; ++i)
        {
            if (m_npcCommon.m_favorExp[i] >= favorExp)
                return i;
        }
    }
    return 4;
}

// ORIG _ZNK14CNPCScriptList18getFavorLevelValueEj 0x858174e：level 0..3 返
// m_favorExp[level]（level 0 返 0），越界（>3）返 getMaxFavorValue()。
unsigned int CNPCScriptList::getFavorLevelValue(unsigned int level) const
{
    // 帧对齐：ORIG sub $0x18（P31 dummy 对齐）。
    char pad[20];
    if (level > 3)
        return getMaxFavorValue();
    if (level == 0)
        return 0;
    return m_npcCommon.m_favorExp[level];
}

// ORIG _destroy 0x8581784：遍历 hash_map delete 各 CNPCScript*（调 D1 + operator delete），
// 再 clear()。
void CNPCScriptList::_destroy()
{
    __gnu_cxx::hash_map<unsigned int, CNPCScript*>::const_iterator
        it(m_npcMap.begin());
    while (it != __gnu_cxx::hash_map<unsigned int, CNPCScript*>::const_iterator(
                       m_npcMap.end()))
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
    // ORIG 0x8534278：遍历 i=0..7，虚槽 +0x00（setDropRatio，权威头已落地）。
    for (int i = 0; i < 8; ++i)
    {
        // ORIG 不判空（initialize 后 8 槽全为有效生成器）。
        CItemGenerator* gen = m_generators[i];
        gen->setDropRatio(ratio);
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

char SecretShopScript::GetNpcByDungeonLev(int& out, int a, int b)
{
    // ORIG 0x8a8293c：GetLevelIdx(b) → +0x18（m_levelNpcMap）equal_range →
    // GetNpcIdx（返回其 char 结果）。
    int levelIdx = GetLevelIdx(b);
    std::pair<std::multimap<int, stDungeonNpc>::iterator,
              std::multimap<int, stDungeonNpc>::iterator> range =
        m_levelNpcMap.equal_range(levelIdx);
    return GetNpcIdx(out, a, range);
}

// ============================================================================
// DeleteInvalidItemScript 生命周期与查询。
// C1/D1（0x8ac1d40/0x8ac1dce）：构造 string +0x0/string +0x4/map +0x8 后
// clearScript()；IsInvalidItem2Delete（0x8ac2096）：map.find(idx) != end。
// ============================================================================
DeleteInvalidItemScript::DeleteInvalidItemScript()
{
    // ORIG C1 0x8ac1d40：构造 string/map 后调 clearScript()（+0x35 call）
    clearScript();
}
// ORIG D1 0x8ac1dce W：clearScript() 后 string +0x0/+0x4 与 map +0x8 由编译器按成员序析构
// （P12 真实析构，非空桩——成员析构仍生成）。
DeleteInvalidItemScript::~DeleteInvalidItemScript()
{
    // ORIG D1 0x8ac1dce：先调 clearScript()（+0x0e call）再析构成员
    clearScript();
}

// ORIG 0x08ac2066 T：清 +0x00/+0x04 两 string 与 +0x8 map（loadScript 前置）。
void DeleteInvalidItemScript::clearScript()
{
    m_field0.clear();
    m_field4.clear();
    m_deleteMap.clear();
}

bool DeleteInvalidItemScript::IsInvalidItem2Delete(int idx)
{
    return m_deleteMap.find(idx) != m_deleteMap.end();
}

// ORIG 0x8ac20f8：map.find(idx)；命中 → rewardIdx=data.m_field0、rewardCnt=data.m_field4。
bool DeleteInvalidItemScript::GetRewardItemInfo(int idx, int& rewardIdx,
                                                int& rewardCnt)
{
    std::map<int, DeleteInvalidItemData>::const_iterator it =
        m_deleteMap.find(idx);
    if (it == m_deleteMap.end())
        return false;
    // ORIG 0x8ac2152：ptEv 读 second+0x4 / second+0x8 —— rewardIdx=m_field4、rewardCnt=m_field8
    //（m_field0 为脚本 idx，与 map key 重复；loadScript 填充 data[0]=idx,data[4]=rewardIdx,data[8]=rewardCnt）
    rewardIdx = it->second.m_field4;
    rewardCnt = it->second.m_field8;
    return true;
}

// ORIG 0x8ac2180：返回脚本 +0x00/+0x04 两字符串（发件人名/正文模板）。
void DeleteInvalidItemScript::GetMailInfo(std::string& title,
                                          std::string& body) const
{
    title = m_field0;
    body = m_field4;
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

bool* GetLightServerFlag()
{
    static bool light_server_flag = false;  // ORIG _ZZ18GetLightServerFlagvE17light_server_flag V @0x940b558
	return &light_server_flag;
}

void SetLightServerFlag(bool flag)
{
    // ORIG：经 GetLightServerFlag() 取标志指针（存入局部变量）后写入。
    bool* lightFlag = GetLightServerFlag();
    *lightFlag = flag;
}
// stLimitItemUsageInfoEx 权威定义在 CDataManager.h（含 ctor/dtor 声明），
// 此处不再重复定义。
// ============================================================================
// _checkTimeoutItem（ORIG 0x867cf8d T，G3 任务域辅助）
// CTitleBook 通过 _checkTimeoutItem 直接调用：检查物品是否过期。
// ORIG 逻辑（逐步核对 0x867cf8d 反汇编）：
//   虚表 +0x10（IsAvatarItem，@4）非 0 → 0；
//   虚表 +0x48（PeriodAfterUnsealing，@18）非 0 且 item.m_field0 != 0 → 1；
//   虚表 +0xc（GetItemType，@3）== 0xa 时：expirationDate <= now → 1；
//   否则（其他类型）：item.m_fieldb * 86400 + 0x44a54a80 <= now → 1；
//   其余 → 0。
//   注：ORIG vtable（0x8c96da8）实证 @4=IsAvatarItem、@18=PeriodAfterUnsealing、
//   @19=IsRandomOption（NEW 原实现误用 IsCreatureItem/IsRandomOption，已纠正）。
int _checkTimeoutItem(const Inven_Item& item)
{
    CDataManager* dm = G_CDataManager();
    CItem* ci = dm->find_item(item.m_addInfo);
    if (ci == 0)
    {
        return 0;
    }

    // stLimitItemUsageInfoEx 权威定义见上方（SetLightServerFlag 之后）。
    if (ci->getUsablePeriod() != 0 || ci->getExpirationDate() != 0)
    {
        return 0;
    }
    if (ci->IsAvatarItem())
    {
        return 0;
    }
    if (ci->PeriodAfterUnsealing() && item.m_field0 != 0)
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

void RecipeInfo::changeOutput(std::vector<std::pair<int, int> >& out)
{   // ORIG W 0x08693a78：out.clear(); out = m_vec40（this+0x40 向量赋值）
    out.clear();
    out = m_vec40;
}

// ---- RandomItemTable（CEquipItem.h；ORIG W 0x8515468 / 0x8514b46）----
RandomItemTable::RandomItemTable() {}
RandomItemTable::~RandomItemTable() {}

// ---- BaseItemKey / BaseItemValue（CItemList.h）----
BaseItemKey::BaseItemKey(unsigned char rarity, unsigned char usableLevel,
                         unsigned short groupName)
    : m_rarity(rarity), m_level(usableLevel), m_partType(groupName)
{
}
BaseItemValue::BaseItemValue() : m_field0(0) {}
BaseItemValue::BaseItemValue(int value) : m_field0(value) {}

// ---- seriaRoomDecoEventScript（ORIG C1 0x8372ef4：构造 vector@0、+0xc=0、
//      map@+0x10、vector@+0x28、+0x34=0、+0x38=0 后三容器各自 clear）----
seriaRoomDecoEventScript::seriaRoomDecoEventScript()
    : m_eventItemIdx(), m_maxEventItemCount(0), m_matchEventMap(),
      m_soundList(), m_fatigueCondition(0), m_rewardMailExpireDate(0)
{
    m_matchEventMap.clear();   // ORIG：先 clear +0x10 map
    m_eventItemIdx.clear();    // 再 clear +0x00 vector
    m_soundList.clear();       // 最后 clear +0x28 vector
}
seriaRoomDecoEventScript::~seriaRoomDecoEventScript() {}

// ---- growthEquipmentScript（ORIG C1 0x8372fbc）----
growthEquipmentScript::growthEquipmentScript()
    : m_rewardMap(), m_giftBox(-1), m_materialItem(-1),
      m_materialMax(0), m_eventBubbleMax(0), m_eventSandMax(0)
{
    m_rewardMap.clear();  // ORIG C1 构造 map 后 clear
}
growthEquipmentScript::~growthEquipmentScript() {}

// ---- createChracScript（ORIG C1 0x8373042：map 构造后 clear）----
createChracScript::createChracScript() { m_map0.clear(); }
// ORIG D1 0x837c3d4 (W)：析构成员 map<int,rewardInfoList>。
createChracScript::~createChracScript() {}
// ORIG rewardInfoList C1 0x84e91fa：vector<rewardInfo> ctor + clear()。
createChracScript::rewardInfoList::rewardInfoList()
{
    m_list.clear();
}
// ORIG C1 0x8373098：map 构造后 clear()。
useCharacterJobFatigue::useCharacterJobFatigue()
{
    m_giftMap.clear();
}
useCharacterJobFatigue::~useCharacterJobFatigue() {}

// ---- CharacterCreateCreatureGiftScript（ORIG C1 0x83730ee：构造 map@0/
//      +0x18=0/map@+0x1c/map@+0x34 后三容器各自 clear）----
CharacterCreateCreatureGiftScript::CharacterCreateCreatureGiftScript()
    : m_eventJobMap(), m_changeAbleCreatureLev(0), m_clientChangeMap(),
      m_infinityChangeMap()
{
    m_eventJobMap.clear();
    m_clientChangeMap.clear();
    m_infinityChangeMap.clear();
}
CharacterCreateCreatureGiftScript::~CharacterCreateCreatureGiftScript() {}

// ---- seriaRoomDecoAniEventScript（ORIG C1 0x83731ca：五容器构造后 clear 0x83732a4）----
seriaRoomDecoAniEventScript::seriaRoomDecoAniEventScript() { clear(); }
seriaRoomDecoAniEventScript::~seriaRoomDecoAniEventScript() {}

stRankSystemInfo::stDungeonRankSystem_t::stDungeonRankSystem_t() : m_map0(), m_vec18()
{
    clear();
}
stRankSystemInfo::stDungeonRankSystem_t::~stDungeonRankSystem_t() {}
void stRankSystemInfo::stDungeonRankSystem_t::clear()
{
    // ORIG 0x8370e34：清 map/vector + memset +0x24 0x12 字节
    m_map0.clear();
    m_vec18.clear();
    // ORIG 0x8370e34：+0x24 起 memset 0x12 字节 = m_rankLevel[9] 全体
    memset(m_rankLevel, 0, sizeof(m_rankLevel));
}

// ORIG C1 0x8370eba：构造子对象 + 4 容器后调 clear()（clear 内写 +0x74）。
stRankSystemInfo::stRankSystemInfo() : m_dungeonRank(), m_vec38(), m_map44(),
                                       m_vec5c(), m_vec68()
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

// ---- stRankSystemInfo 嵌套子类（ORIG W 符号）----
// stDungeonClearTimeData：C1 0x0891b6f2（调 clear）/ clear 0x0891b706
// （memset(this,0,0x1c)）/ D1 0x0893fbc6（空体）。
stRankSystemInfo::stDungeonClearTimeData::stDungeonClearTimeData()
{
    clear();
}
void stRankSystemInfo::stDungeonClearTimeData::clear()
{
    memset(this, 0, sizeof(*this));   // ORIG memset 0x1c
}
stRankSystemInfo::stDungeonClearTimeData::~stDungeonClearTimeData() {}

// stComboIntervalBoundary：C1 0x0891b72a / clear 0x0891b73e
// （memset(this,0,0x10)）/ D1 0x08940176（空体）。
stRankSystemInfo::stComboIntervalBoundary::stComboIntervalBoundary()
{
    clear();
}
void stRankSystemInfo::stComboIntervalBoundary::clear()
{
    memset(this, 0, sizeof(*this));   // ORIG memset 0x10
}
stRankSystemInfo::stComboIntervalBoundary::~stComboIntervalBoundary() {}

// ---- stBloodDungeon_t（ORIG C1 0x8370a3a / clear 0x8370b02 / D1 0x8370ab0）----
stBloodDungeon_t::stBloodDungeon_t() : m_vec334(), m_map358()
{
    clear();
}
stBloodDungeon_t::~stBloodDungeon_t() {}
void stBloodDungeon_t::clear()
{
    // ORIG 0x8370b02：循环(unsigned≤199)置 1.0f → +0x334 clear → +0x320=1 →
    // +0x330=1.0f → memset(+0x324,0,0xc) → memset(+0x340,0,0x18) → +0x358 clear。
    for (unsigned int i = 0; i <= 199; ++i)
        m_rate[i] = 1.0f;
    m_vec334.clear();
    m_field320 = 1;
    m_field330 = 1.0f;
    memset(m_field324, 0, sizeof(m_field324));
    memset(m_pad340, 0, 0x18);  // 连续覆盖 +0x340..+0x357（含 344/348 两 int）
    m_map358.clear();
}
// stVillageAttacked_t 现为纯 vector 包装（CDataManager.h），ctor/dtor 隐式生成
// （ORIG W 0x837cf1e/0x837cf32 即隐式符号），无显式定义。

// ---- stDeathTower_t（ORIG C1 0x08370970）----

// ORIG C1 0x8370970：四个 200 元素表初始化 + 尾部字段，非全量 memset。
stDeathTower_t::stDeathTower_t()
{
    m_apcExpWeight = 0.0f;        // ORIG +0x00 位清零（float 语义：import [apc exp weight] fstps 实证）
    m_monsterExpWeight = 0.0f;    // float 语义（[monster exp weight] fstps）
    m_rewardGoldWeight = 1.0f;
    for (unsigned int i = 0; i < 200; ++i)
    {
        m_rewardExpWeight[i] = 1.0f;          // this+0x04+i*4
        m_rewardCardNum[i] = 0;               // this+0x324+i*4
        m_illusionRewardExpWeight[i] = 1.0f;  // this+0x644+i*4
        m_illusionRewardCardNum[i] = 0;       // this+0x964+i*4
    }
    memset(m_rewardItemProb, 0, sizeof(m_rewardItemProb));  // ORIG memset(+0xc88,0,12)
}

// live_server_info_t ctor ORIG 0x0837dd64：仅 map 成员构造（自动）。
live_server_info_t::live_server_info_t() : m_pvpChannelMap() {}
live_server_info_t::~live_server_info_t() {}

stQuestShop::stQuestShop()
{
    // ORIG C1 0x83710d4：[map C1][initCost C1][map clear] 后 -1、memset(+0x28,0,0x4d)。
    m_map0.clear();
    m_field18 = -1;
    memset(m_pad28, 0, sizeof(m_pad28));
}
stQuestShop::~stQuestShop() {}
InvalidCharData::InvalidCharData()
    : m_charList(), m_charRange(), m_shortList(), m_shortRange()
{
}
InvalidCharData::~InvalidCharData() {}

// ---- SlangNameData（ORIG C1 0x837d08e / D1 0x837d0fa，本 TU 唯一 owner）----
// C1 依次构造 string/unsigned-char/unsigned-short 三个 vector；D1 反向析构。
SlangNameData::SlangNameData()
    : m_slangList(), m_charList(), m_shortList()
{
}
SlangNameData::~SlangNameData() {}

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
{
    // ORIG C1 0x83850cc：[vector C1] 后 clear + 三字节清零。
    m_BoosterPoint.clear();
    m_cMaxBoosterGage = 0;
    m_cFieldD = 0;
    m_cMultiboxCount = 0;
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

// ORIG get_reseal_cost @0x08900b02（T）。见 CDataManager.h 声明注释。
int reseal_table_t::get_reseal_cost(unsigned char param_1,
                                    unsigned char param_2,
                                    unsigned char param_3)
{
    // param_1 强化等级索引 0..5；param_2 等级 1..99；param_3 选项 0..6
    if (param_2 == 0)              // -0x20(%ebp)==0
        return -1;
    if (param_1 > 5)              // -0x1c(%ebp)>5 → jbe 未命中时
        return -2;
    if (param_2 > 99)             // -0x20(%ebp)>99
        return -3;
    if (param_3 > 6)              // -0x24(%ebp)>6
        return -4;

    // (param_2-1)/10（0x66666667 乘法魔数 → 无符号除 10 的 32 位无符号截断）
    unsigned int idx = (unsigned int)(param_2 - 1) / 10u;
    if (m_vec.size() <= idx)      // setbe → vector::size() <= idx
        idx = m_vec.size() - 1;   // 取末元素
    // return m_vec[idx]*m_arr1[param_1] + param_3*m_arr2[param_1]
    // （ORIG imul 为 32 位模 2^32，此处用无符号累加再一次性转 int 保持位模式）
    unsigned int result =
        m_vec[idx] * (unsigned int)m_arr1[param_1]
        + (unsigned int)param_3 * (unsigned int)m_arr2[param_1];
    return (int)result;
}

// ORIG get_oneshot_reseal_cost @0x08900be0（T）。见头声明注释。
int reseal_table_t::get_oneshot_reseal_cost(unsigned char param_1,
                                            unsigned char param_2,
                                            unsigned char param_3)
{
    // 汇编仅校验 param_1<=5 与 param_2 范围，忽略 param_3（无 -0xc(%ebp) 读取）
    (void)param_3;
    if (param_2 == 0)
        return -1;
    if (param_1 > 5)
        return -2;
    if (param_2 > 99)
        return -3;
    return 1;
}

// ---- InGameAdvertisementScript（本 TU 唯一 owner；InitScriptProducers_7 重复已删）----
// ORIG C1 0x83732f0：仅调 clear()。
InGameAdvertisementScript::InGameAdvertisementScript()
{
    clear();
}

// ORIG clear 0x8373304：memset(this, 1, 0x97)——前 0x97 字节置 1，尾字节保持原值。
void InGameAdvertisementScript::clear()
{
    memset(this, 1, 0x97);
}

// ---- PcBangItemRentarData（ORIG D1 0x837c2c0）----
PcBangItemRentarData::PcBangItemRentarData() : m_vec0(), m_mapC()
{
}
PcBangItemRentarData::~PcBangItemRentarData() {}

// ---- DimensionActivationData（ORIG C1 0x8371422 / D1 0x837c306）----
DimensionActivationData::DimensionActivationData()
    : m_field0(0), m_coinMap()
{
	// ORIG C1：+0x00 = 0，构造 map/rewardData 后清 coinMap（m_rewardData 默认初始化，无值初始化 memset）
	m_coinMap.clear();
}
DimensionActivationData::~DimensionActivationData() {}

// ORIG C1 0x837c53c：仅构造 +0x4/+0x10/+0x1c/+0x28/+0x34 五容器，+0x00 不初始化。
stUnlimitChallengeInfo::stUnlimitChallengeInfo()
    : m_vec4(), m_vec10(), m_vec1c(), m_vec28(), m_multiMap34()
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
    : m_npcEventMap()
{
}
// ORIG 0x890ce34：遍历 delete 节点后直接析构 map（无显式 clear()）。
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
}

// ---- CNPCRelationEventManager::AddNPCInfo（ORIG 0x890ceea：
//      make_pair(idx, info) 后 map insert）----
void CNPCRelationEventManager::AddNPCInfo(unsigned short idx,
                                          stNPCRelationEventInfo* info)
{
    m_npcEventMap.insert(std::make_pair(idx, info));
}

// ---- CNPCRelationEventManager::AddFavorItemInfo（ORIG 0x890cf44：
//      find(idx)，非 end 时将 {itemId,count} push_back 到 node+kind*0x14
//      即 m_favor[kind].m_items；步长 0x14 为 stNPCFavorAndRewardInfo）----
void CNPCRelationEventManager::AddFavorItemInfo(
    unsigned short idx, ENUM_NPC_RELATION_EVENT_FAVOR_KIND kind,
    unsigned int itemId, unsigned int count)
{
    std::map<unsigned short, stNPCRelationEventInfo*>::iterator it =
        m_npcEventMap.find(idx);
    if (it == m_npcEventMap.end())
        return;

    stNPCEventItemInfo item;
    item.m_itemId = static_cast<int>(itemId);
    item.m_count = static_cast<int>(count);
    it->second->m_favor[kind].m_items.push_back(item);
}

// ---- CNPCRelationEventManager::AddRewardItemInfo（ORIG 0x890cfda：
//      find(idx)，非 end 时写 node+kind*0x14+0xc / +0x10 两奖励值）----
void CNPCRelationEventManager::AddRewardItemInfo(
    unsigned short idx, ENUM_NPC_RELATION_EVENT_FAVOR_KIND kind,
    unsigned int rewardA, unsigned int rewardB)
{
    std::map<unsigned short, stNPCRelationEventInfo*>::iterator it =
        m_npcEventMap.find(idx);
    if (it == m_npcEventMap.end())
        return;

    it->second->m_favor[kind].m_rewardA = static_cast<int>(rewardA);
    it->second->m_favor[kind].m_rewardB = static_cast<int>(rewardB);
}

// ---- CNPCRelationEventManager::AddDislikeItemInfo（ORIG 0x890d07a：
//      find(idx)，非 end 时向 node+0x3c 的 vector<int> push_back）----
void CNPCRelationEventManager::AddDislikeItemInfo(unsigned short idx,
                                                  unsigned int itemId)
{
    std::map<unsigned short, stNPCRelationEventInfo*>::iterator it =
        m_npcEventMap.find(idx);
    if (it == m_npcEventMap.end())
        return;

    it->second->m_dislikeItems.push_back(static_cast<int>(itemId));
}

// ---- CNPCRelationEventManager::GetFavorIndex（ORIG 0x890d3d0：
//      strncmp 前 10 字节分派 [good]/[soso]/[bad]→0/1/2，其余 3）----
ENUM_NPC_RELATION_EVENT_FAVOR_KIND CNPCRelationEventManager::GetFavorIndex(
    const char* name)
{
    if (std::strncmp("[good]", name, 10) == 0)
        return ENUM_NPC_RELATION_FAVOR_GOOD;
    if (std::strncmp("[soso]", name, 10) == 0)
        return ENUM_NPC_RELATION_FAVOR_SOSO;
    if (std::strncmp("[bad]", name, 10) == 0)
        return static_cast<ENUM_NPC_RELATION_EVENT_FAVOR_KIND>(2);
    return static_cast<ENUM_NPC_RELATION_EVENT_FAVOR_KIND>(3);
}

// ============================================================================
// IsDislikeItem（ORIG T 0x0890d100）：m_npcEventMap.find(idx) 未命中 → 0；
// node->m_dislikeItems（+0x3c vector<int>）任一元素 == itemId → 1。
// ============================================================================
bool CNPCRelationEventManager::IsDislikeItem(unsigned short idx,
                                             unsigned int itemId)
{
    std::map<unsigned short, stNPCRelationEventInfo*>::iterator it =
        m_npcEventMap.find(idx);
    if (it == m_npcEventMap.end())
        return false;

    stNPCRelationEventInfo* node = it->second;
    for (std::vector<int>::iterator vit = node->m_dislikeItems.begin();
         vit != node->m_dislikeItems.end(); ++vit)
    {
        if (*vit == (int)itemId)
            return true;
    }
    return false;
}

// ============================================================================
// GetItemKind（ORIG T 0x0890d1c0，sret stTempRewardInfo*）：
// m_npcEventMap.find(idx) 未命中 → 默认值；遍历 node->m_favor[kind].m_items
// （kind=0..2，步长 0x14）：item.m_itemId==itemId 且 item.m_count<=count：
//   count==0 → 默认值（不写）；
//   否则 → out.m_kind=kind、out.m_count=ROUND(count/item.m_count)。
// ============================================================================
stTempRewardInfo CNPCRelationEventManager::GetItemKind(
    unsigned short idx, unsigned int itemId, unsigned int count)
{
    stTempRewardInfo result;
    std::map<unsigned short, stNPCRelationEventInfo*>::iterator it =
        m_npcEventMap.find(idx);
    if (it == m_npcEventMap.end())
        return result;

    stNPCRelationEventInfo* node = it->second;
    for (int kind = 0; kind < 3; ++kind)
    {
        const std::vector<stNPCEventItemInfo>& items =
            node->m_favor[kind].m_items;
        for (std::vector<stNPCEventItemInfo>::const_iterator vit =
                 items.begin(); vit != items.end(); ++vit)
        {
            if (vit->m_itemId != (int)itemId)
                continue;
            if (vit->m_count > (int)count)
                continue;
            if (vit->m_count == 0)
                return result;
            result.m_kind = kind;
            result.m_count = static_cast<int>(
                (double)count / (double)vit->m_count + 0.5);
            return result;
        }
    }
    return result;
}

// ORIG C1 0x837cf46：仅构造 +0x4/+0x10/+0x1c/+0x28 四 vector，+0x00 不初始化。
stCharacLinkSystem_t::stCharacLinkSystem_t()
    : m_vec4(), m_vec10(), m_vec1c(), m_vec28()
{
}
stCharacLinkSystem_t::~stCharacLinkSystem_t() {}
// ---- CNPCRelationEventManager::GetRewardItem（ORIG T 0x0890d326，sret
//      stNPCRewardItemInfo）：find(idx) 未命中 → {0,0}；命中 →
//      node->m_favor[kind].m_rewardA / m_rewardB（+0xc/+0x10）。----
stNPCRewardItemInfo CNPCRelationEventManager::GetRewardItem(
    unsigned short idx, ENUM_NPC_RELATION_EVENT_FAVOR_KIND kind)
{
    stNPCRewardItemInfo result;
    result.m_rewardA = 0;
    result.m_rewardB = 0;
    std::map<unsigned short, stNPCRelationEventInfo*>::iterator it =
        m_npcEventMap.find(idx);
    if (it == m_npcEventMap.end())
        return result;

    stNPCRelationEventInfo* node = it->second;
    result.m_rewardA = node->m_favor[kind].m_rewardA;
    result.m_rewardB = node->m_favor[kind].m_rewardB;
    return result;
}

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
// ORIG _ZN12advancealtar14BuyUpgradeDataC1Ev 0x088a3570 T：构造 string +0xc/
// vector +0x10/map +0x1c 后：+0x00=0、+0x04=0、+0x08=1（ORIG movb 低字节，
// bool 语义）、vector.clear()、map.clear()。
advancealtar::BuyUpgradeData::BuyUpgradeData()
    : m_strDesc(), m_fieldTypes(), m_listMap()
{
    m_nItemId = 0;
    m_field4 = 0;
    m_field8 = 1;
    m_fieldTypes.clear();
    m_listMap.clear();
}
advancealtar::RewardParameter::RewardParameter()
{
    // ORIG C1 0x889af74 W：构造 this+0 的 map<int,AchievementRewardData>；
    // m_rewardMap 默认构造即空 map，与 ORIG 一致。
}
advancealtar::RewardParameter::~RewardParameter() {}
// ORIG _ZN12advancealtar11BuyShopDataC1Ev 0x88a3630：三 vector 构造后 +0=0、clear×3。
advancealtar::BuyShopData::BuyShopData()
{
    m_nField0 = 0;
    m_vecT0.clear();
    m_vecT1.clear();
    m_vecT2.clear();
}
advancealtar::ConfigParameter::ConfigParameter()
{
    // ORIG 0x889b662：+0/+4 清零、char*+0x18=&(+8)、两段 0x10 memset。
    m_0 = 0;
    m_4 = 0.0f;
    m_strPtr = reinterpret_cast<char*>(this) + 8;
    std::memset(m_gageUpRate, 0, sizeof(m_gageUpRate));
    std::memset(m_gageUpAux, 0, sizeof(m_gageUpAux));
}
advancealtar::ConfigParameter::~ConfigParameter() {}

// ORIG reset 0x0889af9c T：m_rewardMap.clear()。
void advancealtar::RewardParameter::reset()
{
    m_rewardMap.clear();
}

// ============================================================================
// eventReward::importRewardTable（ORIG 0x080ef29c T）
// 列表装载失败返 0x77；逐文件 ImportRewardScript 失败返 0x71；成功返 0。
// ORIG STScriptFileList 即 +0 的 map<int,string>（NpcScript.h 占位版同尺寸），
// 故直接以本地 map 重解释传入，避免与本 TU 已含头中的占位定义冲突。
// ============================================================================
int eventReward::importRewardTable(const char* dir, const char* file)
{
    std::map<int, std::string> fileList;
    if (!loadRDARScriptListFile(reinterpret_cast<STScriptFileList*>(&fileList),
                                dir, file, 0))
        return 0x77;

    for (std::map<int, std::string>::const_iterator it = fileList.begin();
         it != fileList.end(); ++it)
    {
        if (ImportRewardScript(dir, it->second.c_str()) == 0)
            return 0x71;
    }
    return 0;
}

// ============================================================================
// DeleteInvalidItemScript::loadScript（ORIG 0x08ac1e60 T）
// tw_DeleteInvalidItem.etc：[delete item] 三元组 (idx, rewardIdx, count)
// 以 idx 为 key、{idx,rewardIdx,count} 为值 insert；[send mail title]/
// [send mail content] 分别 ScanStr 进 +0/+4 字符串，ScanStr 失败直接结束；
// ScanType 耗尽正常结束返回 1。
// ============================================================================
DeleteInvalidItemScript::DeleteInvalidItemData::DeleteInvalidItemData()
{
    // ORIG C1 0x08ac220c W：转调 init()（三 int 清零）。
    init();
}
void DeleteInvalidItemScript::DeleteInvalidItemData::init()
{
    // ORIG 0x08ac2220 W：三 int 清零。
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
}

bool DeleteInvalidItemScript::loadScript()
{
    if (!loadRDARScriptFile("Etc/", "tw_DeleteInvalidItem.etc"))
        return false;

    std::string token;
    bool ok = false;
    DeleteInvalidItemData data;
    while (ScanType(token, true))
    {
        if (token == "[delete item]")
        {
            for (;;)
            {
                int idx = ScanInt(&ok);
                if (!ok)
                    break;
                int rewardIdx = ScanInt(&ok);
                if (!ok)
                    break;
                int count = ScanInt(&ok);
                if (!ok)
                    break;
                data.init();
                data.m_field0 = idx;        // ORIG：data.f0=首个 int（与 key 同值）
                data.m_field4 = rewardIdx;
                data.m_field8 = count;
                m_deleteMap.insert(std::make_pair(idx, data));
            }
        }
        else if (token == "[send mail title]")
        {
            if (!ScanStr(&m_field0))
                break;
        }
        else if (token == "[send mail content]")
        {
            if (!ScanStr(&m_field4))
                break;
        }
    }
    unloadRDARScriptFile();
    return true;
}

// ============================================================================
// advancealtar::ConfigParameter::importConfigParameter（ORIG 0x0889b6cc T）
// [gage up rate] 行 (idx, a, b)：0<=idx<=3 时 rate[idx]=a、aux[idx]=b；
// [gage upgrade cost percent] ScanFloat 存 +4；[daily refill ticket] 存 +0。
// 错误码：a 失败 0x3e9、b 失败 0x3ec、float 失败 0x3f8、ticket 失败 0x3ff；
// 成功/文件打开失败均返 0（init.md 口径非零=Fail）。
// ============================================================================
int advancealtar::ConfigParameter::importConfigParameter(const char* path)
{
    std::string token;
    bool ok = false;
    if (!loadRDARScriptFile("", path))
        return 0;

    while (ScanType(token, true))
    {
        if (token == "[gage up rate]")
        {
            for (;;)
            {
                int idx = ScanInt(&ok);
                if (!ok)
                    break;                      // ORIG：行首失败仅结束本段
                int rate = ScanInt(&ok);
                if (!ok)
                    return 0x3e9;
                int aux = ScanInt(&ok);
                if (!ok)
                    return 0x3ec;
                if (idx >= 0 && idx <= 3)
                {
                    m_gageUpRate[idx] = rate;
                    m_gageUpAux[idx] = aux;
                }
            }
        }
        else if (token == "[gage upgrade cost percent]")
        {
            m_4 = ScanFloat(&ok);
            if (!ok)
                return 0x3f8;
        }
        else if (token == "[daily refill ticket]")
        {
            m_0 = ScanInt(&ok);
            if (!ok)
                return 0x3ff;
        }
        // [/gage up rate] / [/gage upgrade cost percent] / [/daily refill ticket]
        // 与未知 token 一致跳过（ORIG 仅做串比较后回循环头）。
    }
    return 0;
}

// ============================================================================
// advancealtar::AdvanceAltarShopParameter::importAdvanceAltarShopParameter
// （ORIG 0x088a10cc T，恒返 0）
// [star reset fee gold]：(key, fee) 插入 m_starRestMap；[ridable id] 段以
// isValidRidableId 校验后建 BuyShopData，[unit]/[skill]/[tower] 各自收集
// [id] 子块入对应 vector；[/ridable id] 或文件尾把 shop 按 m_nField0 入
// m_buyShopMap。
// ============================================================================
int advancealtar::AdvanceAltarShopParameter::importAdvanceAltarShopParameter(
    const char* path)
{
    std::string token;
    bool ok = false;
    if (!loadRDARScriptFile("", path))
        return 0;

    for (;;)
    {
        // 跳过无关 token，处理 [star reset fee gold]，直到 [ridable id] 或 EOF。
        bool isRidable = false;
        while (ScanType(token, true))
        {
            if (token == "[star reset fee gold]")
            {
                for (;;)
                {
                    _StarRestFeeByGold fee;
                    int key = ScanInt(&ok);
                    if (!ok)
                        break;
                    fee.m_field0 = ScanInt(&ok);
                    if (!ok)
                        break;
                    m_starRestMap.insert(std::make_pair(key, fee));
                }
                continue;
            }
            if (token == "[ridable id]")
            {
                isRidable = true;
                break;
            }
        }
        if (!isRidable)
            break;                                   // 文件耗尽 → 返 0

        BuyShopData shop;
        int ridableId = ScanInt(&ok);
        if (ok && isValidRidableId(static_cast<advancealtar::RidableId::T>(ridableId)))
        {
            shop.m_nField0 = ridableId;
            const char* endTag = 0;
            std::vector<BuyUpgradeData>* outVec = 0;
            bool inSection = false;
            bool closed = false;
            while (!closed && ScanType(token, true))
            {
                if (token == "[unit]" || token == "[skill]" || token == "[tower]")
                {
                    endTag = (token == "[unit]") ? "[/unit]"
                           : (token == "[skill]") ? "[/skill]" : "[/tower]";
                    outVec = (token == "[unit]") ? &shop.m_vecT0
                               : (token == "[skill]") ? &shop.m_vecT1 : &shop.m_vecT2;
                    const bool isSkill = (token == "[skill]");
                    inSection = true;
                    while (ScanType(token, true))
                    {
                        if (token == endTag)
                        {
                            inSection = false;
                            break;
                        }
                        if (token != "[id]")
                            continue;
                        BuyUpgradeData item;
                        int idVal = ScanInt(&ok);
                        if (!ok)
                            break;                   // ORIG：失败弃项回段循环
                        item.m_nItemId = idVal;
                        bool idDone = false;
                        while (ScanType(token, true))
                        {
                            if (token == "[unlock stage index]")
                            {
                                item.m_field4 = ScanInt(&ok);
                                if (!ok)
                                    break;
                                continue;
                            }
                            if (isSkill && token == "[is settable slot]")
                            {
                                int v = ScanInt(&ok);
                                if (!ok)
                                    break;
                                item.m_field8 = (v != 0);
                                continue;
                            }
                            if (token == "[name]")
                            {
                                ScanStr(&item.m_strDesc);
                                continue;
                            }
                            if (token == "[field name]")
                            {
                                std::string fieldName;
                                while (ScanStr(&fieldName))
                                {
                                    int ft = getFieldType(fieldName);
                                    if (ft != -1)
                                        item.m_fieldTypes.push_back(
                                            static_cast<FieldType::T>(ft));
                                }
                                continue;
                            }
                            if (token == "[list]")
                            {
                                FieldDataMap rowMap;
                                bool rowOk = true;
                                while (rowOk)
                                {
                                    rowMap.clear();
                                    for (size_t fi = 0; fi < item.m_fieldTypes.size(); ++fi)
                                    {
                                        FieldType::T ft = item.m_fieldTypes[fi];
                                        FieldData fd;
                                        fd.m_field0 = ft;
                                        if (getdataType(ft) == 0)
                                        {
                                            std::string s;
                                            if (!ScanStr(&s))
                                            {
                                                rowOk = false;
                                                break;
                                            }
                                            fd.m_str = s;
                                        }
                                        else
                                        {
                                            fd.m_value = ScanInt(&ok);
                                            if (!ok)
                                            {
                                                rowOk = false;
                                                break;
                                            }
                                        }
                                        rowMap.insert(std::make_pair(ft, fd));
                                    }
                                    if (rowOk)
                                    {
                                        // ORIG key=FieldType-0 行 FieldData+4（m_value）。
                                        int key =
                                            rowMap[static_cast<FieldType::T>(0)].m_value;
                                        item.m_listMap.insert(
                                            std::make_pair(key, rowMap));
                                    }
                                }
                                continue;
                            }
                            if (token == "[/id]")
                            {
                                idDone = true;
                                break;
                            }
                        }
                        if (idDone || token == endTag)
                            break;                   // 弃项或段闭包均回段级扫描
                    }
                    continue;
                }
                if (token == "[/ridable id]")
                    closed = true;
                // inSection 状态下段级 ScanType 已在子循环处理；此处仅段外 token。
            }
        }
        // ORIG LAB_088a267d：无论段内成败，[/ridable id] 或 EOF 都插入当前 shop。
        m_buyShopMap.insert(std::make_pair(shop.m_nField0, shop));
    }
}

// ---- advancealtar::BuyUpgradeData::getFieldDataPoint（ORIG T 0x088a2e4c）----
// m_listMap.find(level) 未命中→0；it->second.find(ft) 未命中→0；命中返回
// &fit->second（FieldData*）。报告 advancealtar__BuyUpgradeData/getFieldDataPoint.md。
advancealtar::FieldData* advancealtar::BuyUpgradeData::getFieldDataPoint(
    int level, FieldType::T ft)
{
    std::map<int, FieldDataMap>::iterator it = m_listMap.find(level);
    if (it == m_listMap.end())
        return 0;
    std::map<FieldType::T, FieldData>::iterator fit = it->second.find(ft);
    if (fit == it->second.end())
        return 0;
    return &fit->second;
}

// ---- advancealtar::BuyShopData::getBuyUpgradeDataVec（ORIG T 0x088a105c）----
// type 0/1/2 → &m_vecT0/T1/T2；其它 → 0。报告 advancealtar__BuyShopData/。
std::vector<advancealtar::BuyUpgradeData>*
advancealtar::BuyShopData::getBuyUpgradeDataVec(AdvanceAltarShopType::T type)
{
    switch (type)
    {
    case AdvanceAltarShopType::T_0: return &m_vecT0;
    case AdvanceAltarShopType::T_1: return &m_vecT1;
    case AdvanceAltarShopType::T_2: return &m_vecT2;
    default: return 0;
    }
}

// ---- advancealtar::AdvanceAltarShopParameter::getBuyUpgradeDataVec
// （ORIG T 0x088a0fe8）：m_buyShopMap.find(key) 未命中→0；命中转调
// BuyShopData::getBuyUpgradeDataVec(type)。报告同名 md。
std::vector<advancealtar::BuyUpgradeData>*
advancealtar::AdvanceAltarShopParameter::getBuyUpgradeDataVec(
    int key, AdvanceAltarShopType::T type)
{
    std::map<int, BuyShopData>::iterator it = m_buyShopMap.find(key);
    if (it == m_buyShopMap.end())
        return 0;
    return it->second.getBuyUpgradeDataVec(type);
}

// ============================================================================
// advancealtar::RewardParameter::importAchievementRewardParameter
// （ORIG 0x0889afb0 T）[reward data]…[/reward data] 块解析进
// AchievementRewardData 后按 m_type 插入 m_rewardMap。错误码：
// type 扫描 0x362 / type<=0 0x364 / 难度串 0x36a / 难度枚举 0x36d /
// 清空次数 0x373 / 图标 0x37a / 奖励计数 0x391 / 标题 0x399 /
// 邮政标题 0x39e / 正文 0x3a3。[reward item] 三元组仅 {type,_,itemId}
// 进入向量（第三值写入越界临时不入拷贝，见报告注记）。
// ============================================================================
int advancealtar::RewardParameter::importAchievementRewardParameter(
    const char* path)
{
    std::string token;
    bool ok = false;
    reset();
    if (!loadRDARScriptFile("", path))
        return 0;

    while (ScanType(token, true))
    {
        if (token != "[reward data]")
            continue;

        AchievementRewardData data;
        while (ScanType(token, true))
        {
            if (token == "[/reward data]")
            {
                m_rewardMap.insert(std::make_pair(data.m_type, data));
                break;
            }
            if (token == "[type]")
            {
                data.m_type = ScanInt(&ok);
                if (!ok)
                    return 0x362;
                if (!(data.m_type > 0))
                    return 0x364;
                continue;
            }
            if (token == "[stage difficulty]")
            {
                std::string diffName;
                if (!ScanStr(&diffName))
                    return 0x36a;
                StageDifficulty::T diff;
                if (!StageDifficulty::getDifficulty(diffName, diff))
                    return 0x36d;
                data.m_difficulty = static_cast<int>(diff);
                continue;
            }
            if (token == "[stage clear count]")
            {
                data.m_stageClearCount = ScanInt(&ok);
                if (!ok)
                    return 0x373;
                continue;
            }
            if (token == "[icon image]")
            {
                data.m_iconImage = ScanInt(&ok);
                if (!ok)
                    return 0x37a;
                continue;
            }
            if (token == "[reward item]")
            {
                for (;;)
                {
                    _RewardItem item;
                    std::string typeName;
                    RewardItemType::T itemType;
                    if (!ScanStr(&typeName))
                        break;                       // 静默退出本段
                    if (!RewardItemType::getRewardItemType(typeName, itemType))
                        break;
                    item.m_type = itemType;
                    item.m_value2 = ScanInt(&ok);    // ORIG 写 scratch+8
                    if (!ok)
                        break;
                    // ORIG 第二次 ScanInt 写入 12 字节临时对象之外，
                    // push_back 仅拷贝前 12 字节，计数值不入向量；
                    // 此处保留解析推进语义但不存储。
                    ScanInt(&ok);
                    if (!ok)
                        return 0x391;
                    data.m_rewardItems.push_back(item);
                }
                continue;
            }
            if (token == "[reward title]")
            {
                if (!ScanStr(&data.m_rewardTitle))
                    return 0x399;
                continue;
            }
            if (token == "[postal title]")
            {
                if (!ScanStr(&data.m_postalTitle))
                    return 0x39e;
                continue;
            }
            if (token == "[postal letter]")
            {
                if (!ScanStr(&data.m_postalLetter))
                    return 0x3a3;
                continue;
            }
        }
    }
    return 0;
}

// ORIG C1 0x08371a26：构造 7 容器成员后调 clear()（+0x84 call clear）。
PowerParameterScript::PowerParameterScript()
{
    clear();
}
PowerParameterScript::~PowerParameterScript()
{
    // ORIG D1：逆序析构 7 个容器成员（+0xc8/+0xbc/+0xac/+0x9c/+0x70/+0x38/+0x20）
}

// ---- PowerParameterScript 查询（ORIG T 0x08a59c6e / 0x08a59d58）----
// GetPenalty：遍历 m_vecBC（+0xbc），取首个 m_field4 >= victory 的条目按值返回；
// 无匹配时返回最后一条（ORIG memmove(&vec[size-1], 0x10)）；空表未定义。
stVictoryPenalty PowerParameterScript::GetPenalty(int victory)
{
    stVictoryPenalty result;
    for (std::vector<stVictoryPenalty>::iterator it = m_vecBC.begin();
         it != m_vecBC.end(); ++it)
    {
        const stVictoryPenalty& entry = *it;
        if (entry.m_field4 >= victory)
        {
            result.m_field0 = entry.m_field0;
            result.m_field4 = entry.m_field4;
            result.m_field8 = entry.m_field8;
            result.m_fieldC = entry.m_fieldC;
            return result;
        }
    }
    // ORIG：无条件整体赋值（memmove 16B，无 empty 保护；size==0 时为
    // 与 ORIG 相同的越界读，勿加保护）。
    result = m_vecBC[m_vecBC.size() - 1];
    return result;
}

// GetBreakBonus（ORIG T 0x08a59d58）：m_vecC8（+0xc8 连破奖励表）；
// winCount < size → v[winCount]，否则 v[size-1]（越界取末元素）。
int PowerParameterScript::GetBreakBonus(int winCount)
{
    const std::vector<int>& v = m_vecC8;
    const int size = static_cast<int>(v.size());
    if (winCount < size)
    {
        return v[winCount];
    }
    return v[size - 1];
}

// ============================================================================
// checkPowerWarServer（ORIG T 0x08a59db4）：遍历 m_powerWarServerList
// （+0x9c，vector<pair<int,int>>），pair.first==a && pair.second==b → 1，
// 否则 0。
// ============================================================================
bool PowerParameterScript::checkPowerWarServer(int a, int b)
{
    for (std::vector<std::pair<int, int> >::iterator it =
             m_powerWarServerList.begin(); it != m_powerWarServerList.end(); ++it)
    {
        if (it->first == a && it->second == b)
            return true;
    }
    return false;
}

// ---- SecretShopScript（成员默认构造）----

// ---- SecretShopScript（成员默认构造）----
SecretShopScript::SecretShopScript()
{
    clear();   // ORIG C1 0x83720f4：构造 5 容器成员后调 clear()（写 +0x80/+0x84）
}
SecretShopScript::~SecretShopScript() {}

// ---- stTitleBookInfo（ORIG C1 0x837d828 / D1 0x837d906）----
stTitleBookInfo::stTitleBookInfo()
    : m_vec0(), m_vecC(), m_map18(), m_vec30(), m_vec3c(), m_vec48()
{
}
stTitleBookInfo::~stTitleBookInfo() {}

// ---- stItemMakingSkill（ORIG C1 0x8370ff2 / clear 0x837105a / D1 0x837dce2）----
stItemMakingSkill::stItemMakingSkill() : m_map0(), m_vec18()
{
    clear();   // ORIG C1：构造 map/vector 后调 clear()（清三 int）
}
stItemMakingSkill::~stItemMakingSkill() {}
// ORIG clear 0x837105a：map.clear()、vector.clear()、+0x24/+0x28/+0x2c 三 int 清零。
void stItemMakingSkill::clear()
{
    m_map0.clear();
    m_vec18.clear();
    m_exp = 0;
    m_bonusExp = 0;
    m_levelLimit = 0;
}

// ---- ChoiceItemInfo / ImageCommunicationData（无析构声明）----
ChoiceItemInfo::ChoiceItemInfo()
{
    clear();
}

void ChoiceItemInfo::clear()
{
    m_checked = 0;
    m_choice = static_cast<unsigned int>(-1);
}
ImageCommunicationData::ImageCommunicationData()
{
    m_charge = 0;
}

// ---- ARAD::SCRIPT::AradJumping_Script----
// W7_BuildHarmonize 去重：生命周期唯一 owner 为 Arad_Script.cpp
// （同文件含 getRDARScriptData/importScript 等同类方法）。

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
// ORIG C1 0x8371544：两个 int 置 0 后构造 + clear 内嵌 stLevelUpRewardItemScript。
stReturnUserRewardScript::stReturnUserRewardScript()
    : m_field0(0), m_field4(0)
{
    m_levelUp.m_map0.clear();
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
CWorldMapList::~CWorldMapList()
{
    destroy();  // ORIG D1 0x837481e：destroy() 后析构 map
}

// ORIG 0x8374752：遍历 delete 各 CWorldMap*（D1 + operator delete），最后清空 map。
void CWorldMapList::destroy()
{
    // ORIG 0x8374752：局部变量暂存 second、delete、清零局部（不写回 map 值）。
    for (std::map<int, CWorldMap*>::iterator it = m_map.begin(); it != m_map.end(); ++it)
    {
        CWorldMap* worldMap = it->second;
        if (worldMap != 0)
        {
            delete worldMap;
        }
        worldMap = 0;
    }
    m_map.clear();
}

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

// ORIG W 0x0822b514 _ZNK9CWorldMap18getDungeonMinLevelEv（const）：返回
// m_dungeonIndex（this+0x8）；StartGame::check_error 消费。
int CWorldMap::getDungeonMinLevel() const
{
    return m_dungeonIndex;
}

// ORIG W 0x822b52c _ZNK9CWorldMap10hasDungeonEj（0x85 字节）：
// std::find(m_dungeonList.begin(), m_dungeonList.end(), dungeonIdx) != end()。
// 实参为 unsigned int（find 模板 _ZSt4findI...jE...）；调用方
// DisPatcher_SelectDungeon::process 传 (int)(short)MSG_CHARAC_SELECT+0xd。
bool CWorldMap::hasDungeon(unsigned int dungeonIdx) const
{
    return std::find(m_dungeonList.begin(), m_dungeonList.end(), dungeonIdx) !=
           m_dungeonList.end();
}

StageMapList::StageMapList() : m_firstStageId(0) {}
StageMapList::~StageMapList() {}
// ORIG C1 0x083708c6：构造 +0x120 的 map 后调用 clear()。
WarAreaCounter::WarAreaCounter()
{
    clear();
}

// ORIG W 0x083707a4：memset +0x000/0x60/0xc0 三段数组、清 +0xe8..+0xf4 四个
// int、memset +0x0f8 段、map(+0x120).clear()、清 +0x140；其余（wpPenalty/
// monsterExpConst/warRoomCount/pad_2d4）不动。
void WarAreaCounter::clear()
{
    memset(m_counterTable, 0, sizeof(m_counterTable));
    memset(m_timeHourList, 0, sizeof(m_timeHourList));
    memset(m_doorFee, 0, sizeof(m_doorFee));
    m_curHourIdx = 0;
    m_banInterval = 0;
    m_kickPoint = 0;
    m_readyCount = 0;
    memset(m_warAreaDungeon, 0, sizeof(m_warAreaDungeon));
    m_wpPerMonsterHp.clear();
    m_num = 0;
    // ORIG 尾段：+0x140=num 后 memset(+0x144,0,0x3c0)、清 +0x138、+0x13c。
    memset(m_warRoomCount, 0, sizeof(m_warRoomCount));
    m_wpPenalty = 0;
    m_monsterExpConst = 0;
}
WarAreaCounter::~WarAreaCounter() {}

// ---- stEquipmentGradeSample（ORIG C1 0x837c810 / D1 0x837c884：
//      5 个 vector<unsigned int> 依 +0x00→+0x30 构造、逆序析构）----
stEquipmentGradeSample::stEquipmentGradeSample()
    : m_grade0(), m_grade1(), m_grade2(), m_grade3(), m_grade4()
{
}
stEquipmentGradeSample::~stEquipmentGradeSample() {}

// ---- stOneEmotionData / stEmotionTypeTagsInfo（自 GameStubs.cpp 迁移）----
// ORIG 0x891b560：构造 strings + vector<string>，再 clear
stOneEmotionData::stOneEmotionData() : m_8(), m_c(), m_14(), m_18()
{
    clear();
}
// ORIG 0x891b640（stOneEmotionData::operator=）：逐成员复制
// 0x00/0x04 int、0x08/0x0c/0x14 string、0x10/0x11/0x12 char、0x18 vector。
// 注意：+0x13 为 padding（m_pad13），ORIG 不复制；镜像布局须显式定义以
// 覆盖编译器隐式逐成员赋值（隐式版会复制 m_pad13）。
stOneEmotionData& stOneEmotionData::operator=(const stOneEmotionData& rhs)
{
    m_0 = rhs.m_0;
    m_4 = rhs.m_4;
    m_8 = rhs.m_8;
    m_c = rhs.m_c;
    m_10 = rhs.m_10;
    m_11 = rhs.m_11;
    m_12 = rhs.m_12;
    m_14 = rhs.m_14;
    m_18 = rhs.m_18;
    return *this;
}
// ORIG W 0x8370bb0（0x9b 字节）：成员自动析构，标量无操作。
stOneEmotionData::~stOneEmotionData() {}
// ORIG W 0x891b61a：clear —— 清零标量 + 清空容器。
void stOneEmotionData::clear()
{
    m_0 = 0;
    m_4 = 0;
    m_8.clear();
    m_c.clear();
    m_10 = 0;
    m_11 = 0;
    m_12 = 0;
    m_pad13 = 0;
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

// ORIG C1 0x837c8c6：仅构造 +0x8/+0x20/+0x38 三容器，+0x0/+0x4 不初始化。
stConditionEventInfo::stConditionEventInfo()
    : m_map8(), m_multiMap20(), m_multiMap38()
{
}
stConditionEventInfo::~stConditionEventInfo() {}

// ---- pvp_channel_info_t（ORIG C1 _ZN18pvp_channel_info_tC1Ev 0x8370912：
//      将 +0x00..+0x20 共 9 个 dword 清零；+0x24 m_pvpMissionSystem 不初始化）----
pvp_channel_info_t::pvp_channel_info_t()
{
    // ORIG 0x8370912：+0x00..+0x20 共 9 个 dword 清零；+0x24 不初始化。
    m_field0 = 0;
    m_minGrade = 0;
    m_maxGrade = 0;
    m_channelType = 0;        // +0x10（先于 +0x0c 清零，与 ORIG 序一致）
    m_upgradeRevision = 0;    // +0x0c
    m_minLevel = 0;
    m_maxLevel = 0;
    m_field1c = 0;
    m_field20 = 0;
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

// STExpertJobScript::GetCompoundRate（ORIG 0x0849f366 W）。
// 遍历 m_compoundRateVec（+0x1f4）：返回首个 stCompoundRate.m_level >= level 的元素；
// 全表都 < level 时返回末元素；空表返回 0。调用方（GetSuccessRate）读 +4/+8/+0xc。
stCompoundRate* STExpertJobScript::GetCompoundRate(int level)
{
    if (m_compoundRateVec.empty())
        return 0;
    std::vector<stCompoundRate>::iterator it = m_compoundRateVec.begin();
    for (; it != m_compoundRateVec.end(); ++it)
    {
        if (it->m_level >= level)
            return &(*it);
    }
    --it;  // 全部 < level：取末元素
    return &(*it);
}

// STExpertJobScript::GetRecipeInfoIndex（ORIG 0x0832b726 W）。
// 在 m_recipeIndexMap（+0x218，map<unsigned long,int>）按 itemIndex 查；
// 找到返回 mapped int（recipe 信息索引），否则 -1。
int STExpertJobScript::GetRecipeInfoIndex(unsigned long itemIndex)
{
    std::map<unsigned long, int>::iterator it = m_recipeIndexMap.find(itemIndex);
    if (it == m_recipeIndexMap.end())
        return -1;
    return it->second;
}

// STExpertJobScript::GetRecipeInfo（ORIG 0x0832b6b6 W）。
// 在 m_recipeInfoMap（+0x200，map<int,stRecipeInfo>）按 index 查；
// 找到返回 &it->second，否则 0。
stRecipeInfo* STExpertJobScript::GetRecipeInfo(int index)
{
    std::map<int, stRecipeInfo>::iterator it = m_recipeInfoMap.find(index);
    if (it == m_recipeInfoMap.end())
        return 0;
    return &it->second;
}
// ORIG D1 0x837c3d4 (W)：析构成员 map<int,rewardInfoList>。
// ---- GuildParameterScript::getGuildExpWeight（ORIG T 0x089795de）----
// m_map44.find(level) 命中返回 second，否则 0（CUser::FatigueUp 公会经验权重，
// 与 0x08979864 getExpWeight 的 m_vecE4 扫表路径不同；ORIG 返回 int）。
int GuildParameterScript::getGuildExpWeight(int level)
{
    std::map<int, int>::iterator it = m_map44.find(level);
    if (it != m_map44.end()) return it->second;
    return 0;
}

// ---- GuildParameterScript::getGuildAgitUpgradeInfo（ORIG T 0x089796dc）----
// m_vec2c（STAgitUpgradeInfo[agit upgrade info]）取下标 level-1；
// level-1 >= size 返回 NULL（ORIG cmp/setl 分支一致）。
STAgitUpgradeInfo* GuildParameterScript::getGuildAgitUpgradeInfo(int level)
{
    if (level - 1 < (int)m_vec2c.size())
        return &m_vec2c[level - 1];
    return 0;
}

// ---- GuildParameterScript::getExpWeight（ORIG T 0x08979864）----

// ---- GuildParameterScript::getExpWeight（ORIG T 0x08979864）----
// 自高向低扫 m_vecE4（vector<pair<int,double>>，等级阈值→经验权重），
// 首个 first<=guildLevel 的 second；全不命中取 [0].second（ORIG 无判空，照抄）。
double GuildParameterScript::getExpWeight(int guildLevel)
{
    for (int i = (int)m_vecE4.size() - 1; i >= 0; --i)
    {
        if (m_vecE4[i].first <= guildLevel)
            return m_vecE4[i].second;
    }
    return m_vecE4[0].second;
}

// ---- GuildParameterScript::getGuildLevelUpParam（ORIG T 0x08979648）----
// level∈[1,16] 返回 m_levelUpCost+(level-1)*8 的地址（按 int 口径），否则 0。
int GuildParameterScript::getGuildLevelUpParam(int level)
{
    if (level <= 0 || 0x10 < level)
        return 0;
    return reinterpret_cast<int>(
        reinterpret_cast<const char*>(&m_levelUpCost[0]) + (level - 1) * 8);
}

// ---- GuildParameterScript::getLevelupPrice（ORIG T 0x089797a4）----
// 自高向低扫 m_vecF0，首个 first<=memberCnt（members 阈值）的
// second[level]；全不命中取 [0].second[level]；结果 ×10000。
int GuildParameterScript::getLevelupPrice(int level, int memberCnt)
{
    const std::vector<std::pair<int, std::vector<int> > >& tbl = m_vecF0;
    int priceVecIdx = -1;
    for (unsigned int i = tbl.size(); i-- > 0;)
    {
        if (tbl[i].first <= memberCnt)
        {
            priceVecIdx = static_cast<int>(i);
            break;
        }
    }
    if (priceVecIdx >= 0)
        return tbl[priceVecIdx].second[level] * 10000;
    return tbl[0].second[level] * 10000;
}

// ---- GuildParameterScript::getGuildExpBook（ORIG T 0x08979672）----
// m_map10.find(key) 命中返回 second，否则 0（W7-BigThree increase_status case 0x10 依赖）。
int GuildParameterScript::getGuildExpBook(int key)
{
    std::map<int, int>::iterator it = m_map10.find(key);
    if (it != m_map10.end()) return it->second;
    return 0;
}
// ORIG _Z14IS_PVP_CHANNELi 0x088dd255（W）：线性 cmpl/je 链，命中集合
// {4,6,5,7,8,9,10,13,14,15,16} 返回 1，否则 0（P45 短路线性判定同构）。
// 定义落本 TU：声明同文件（CDataManagerScripts.cpp 顶部），调用点在
// GetPvPChannelNumber 0x088dcc28（按 bool 语义消费）。
bool IS_PVP_CHANNEL(int channelType)
{
    return (channelType == 4) || (channelType == 6) ||
           (channelType == 5) || (channelType == 7) ||
           (channelType == 8) || (channelType == 9) ||
           (channelType == 10) || (channelType == 13) ||
           (channelType == 14) || (channelType == 15) ||
           (channelType == 16);
}
