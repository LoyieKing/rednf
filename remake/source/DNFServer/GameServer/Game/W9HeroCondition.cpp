// ============================================================================
// df_game_r 还原 —— HeroMissionCondition / BaseHeroMissionCondition 生命周期与
// 方法实现。符号对照 ORIG nm：
//   _ZN20HeroMissionConditionC1Ev / C2Ev      0x081681c8 (W)
//   _ZN20HeroMissionConditionC1ERKS_          0x08964f5c (W)
//   _ZN20HeroMissionConditionD1Ev / D2Ev      0x08168260 (W)
//   _ZN20HeroMissionConditionaSERKS_          0x08961234 (T)
//   _ZN20HeroMissionCondition5clearEv         0x08961376 (T)
//   _ZN20HeroMissionCondition10RewardDataD1Ev 0x08966634 (W)
//   _ZN24BaseHeroMissionConditionC1EN20HeroMissionCondition9MissionNo1TE
//                                             0x08164f12 (T)
//   _ZN24BaseHeroMissionConditionD0Ev         0x08164f6e / D1Ev/D2Ev 0x08164f3a
//   _ZN24BaseHeroMissionCondition16setConditionDataER20HeroMissionCondition
//                                             0x08164f8c (T)
// 其余 Base getter / rewardProcess 地址见各函数注释。缺报告符号均已按
// objdump -d ORIG 忠实转写。
// ============================================================================

#include "HeroMissionCondition.h"

#include <string>
#include <vector>

#include "CMailBoxHelper.h"               // WongWork::ReqDBSendNewSystemMail
#include "CDataManager.h"                 // G_CDataManager/find_item
#include "CInventory.h"                   // insertItemIntoInventory/eItemAddReason
#include "GameTypes.h"                    // cMyTrace
#include "Inven_Item.h"                   // Inven_Item/set_add_info
#include "RDARScriptStringManager.h"      // g_scriptStringManager_/findString
#include "CUser.h"                        // CUser 完整定义
#include "CUserCharacInfo.h"              // getCurCharacInvenW/ENUM_ITEMSPACE

// ---------------------------------------------------------------------------
// HeroMissionCondition::RewardData
// ORIG _ZN20HeroMissionCondition10RewardDataD1Ev 0x08966634：
// push ebp / pop ebp / ret —— 空体但为用户定义析构（保持类型 non-trivial，
// vector reallocation 走 move/uninitialized_copy 链）。
HeroMissionCondition::RewardData::~RewardData()
{
}

// ---------------------------------------------------------------------------
// HeroMissionCondition C1/C2 —— ORIG 0x081681c8：仅依次构造 +0x0c/+0x10
// std::string 与 +0x14/+0x20 vector；标量区不初始化。
HeroMissionCondition::HeroMissionCondition()
    : m_mailTitle(), m_mailContent(), m_rewards(), m_extra()
{
}

// ORIG copy ctor 0x08964f5c：拷贝 +0..+0x9 标量后逐成员拷贝构造两 string、
// 两 vector（vector<RewardData> copy ctor = ORIG C1ERKS3_）。
HeroMissionCondition::HeroMissionCondition(const HeroMissionCondition& other)
    : m_missionType(other.m_missionType),
      m_field04(other.m_field04),
      m_isRepeatMission(other.m_isRepeatMission),
      m_isResetState(other.m_isResetState),
      m_mailTitle(other.m_mailTitle),
      m_mailContent(other.m_mailContent),
      m_rewards(other.m_rewards),
      m_extra(other.m_extra)
{
}

// ORIG dtor 0x08168260：显式逆序析构两个 vector 与两个 string；空体即编译器
// 生成等价逐成员析构。
HeroMissionCondition::~HeroMissionCondition()
{
}

// ORIG operator= 0x08961234：先 dword/字节级拷贝标量区，再两个 string
// operator=，最后对 +0x14/+0x20 两个 vector 用 std::copy + back_inserter 追加
// other 内容（ORIG 为 append 语义，不清空目标已有元素，非容器赋值替换）。
HeroMissionCondition& HeroMissionCondition::operator=(
    const HeroMissionCondition& other)
{
    m_missionType = other.m_missionType;
    m_field04 = other.m_field04;
    m_isRepeatMission = other.m_isRepeatMission;
    m_isResetState = other.m_isResetState;
    m_mailTitle = other.m_mailTitle;
    m_mailContent = other.m_mailContent;

    m_rewards.insert(m_rewards.end(), other.m_rewards.begin(),
                     other.m_rewards.end());
    m_extra.insert(m_extra.end(), other.m_extra.begin(), other.m_extra.end());
    return *this;
}

// ORIG clear 0x08961376：置 +0/+4 为 0、+8/+9 为 0，再 clear 两个 string 与
// 两个 vector。
void HeroMissionCondition::clear()
{
    m_missionType = 0;
    m_field04 = 0;
    m_isRepeatMission = 0;
    m_isResetState = 0;
    m_mailTitle.clear();
    m_mailContent.clear();
    m_rewards.clear();
    m_extra.clear();
}

// ---------------------------------------------------------------------------
// BaseHeroMissionCondition
// ORIG ctor 0x08164f12：写 vptr；this+0x4 = missionNo；构造 this+0x8 处的
// HeroMissionCondition（默认构造）。
BaseHeroMissionCondition::BaseHeroMissionCondition(
    HeroMissionCondition::MissionNo::T no)
    : m_missionCode(static_cast<unsigned int>(no)), m_condition()
{
}

// ORIG D1 0x08164f3a：写回基类 vptr 后析构内嵌 condition；D0 0x08164f6e 为
// deleting 形态。虚析构定义于本 TU 保证唯一 vtable/typeinfo 输出。
BaseHeroMissionCondition::~BaseHeroMissionCondition()
{
}

// ORIG setConditionData 0x08164f8c：HeroMissionCondition::operator= 拷到 this+8。
void BaseHeroMissionCondition::setConditionData(HeroMissionCondition& data)
{
    m_condition = data;
}

// ORIG getMissionCode 0x08164faa：返回 *(unsigned int*)(this+0x4)。
unsigned int BaseHeroMissionCondition::getMissionCode()
{
    return m_missionCode;
}

// ORIG getMissionType 0x08164fb6：返回 *(int*)(this+0xc) = condition +0x00。
unsigned int BaseHeroMissionCondition::getMissionType()
{
    return static_cast<unsigned int>(m_condition.m_missionType);
}

// ORIG isRepeatMission 0x08164fc2：movzbl *(this+0x10) = condition 字节 +0x08。
char BaseHeroMissionCondition::isRepeatMission()
{
    return m_condition.m_isRepeatMission;
}

// ORIG isResetState 0x08164fce：movzbl *(this+0x11) = condition 字节 +0x09。
char BaseHeroMissionCondition::isResetState()
{
    return m_condition.m_isResetState;
}

// ORIG getMailTitle 0x08164fda：sret，std::string 拷贝构造自 this+0x14 =
// condition +0x0c。
std::string BaseHeroMissionCondition::getMailTitle()
{
    return m_condition.m_mailTitle;
}

// ORIG getMailContent 0x08165004：sret，拷贝构造自 this+0x18 = condition +0x10。
std::string BaseHeroMissionCondition::getMailContent()
{
    return m_condition.m_mailContent;
}

// ORIG rewardProcess 0x0816502e：遍历 rewards；逐项 find_item → make_item →
// set_add_info → insertItemIntoInventory(item, eItemAddReason_0xd, true, true)；
// 成功 SendUpdateItem(1, 0, slot)；失败经 RDARScriptStringManager::findString
// (table 4) 解析邮件标题/正文并发系统邮件。找不到物品则 trace 并整体 return false；
// 循环正常结束返回 true。
bool BaseHeroMissionCondition::rewardProcess(CUser* user)
{
    for (std::vector<HeroMissionCondition::RewardData>::iterator it =
             m_condition.m_rewards.begin();
         it != m_condition.m_rewards.end(); ++it)
    {
        const int rewardItem = it->m_itemIdx;

        // ORIG 0x0816507e：G_CDataManager() + find_item(reward->m_itemIdx)。
        CDataManager* dataMgr = G_CDataManager();
        CItem* found = dataMgr == 0 ? 0 : dataMgr->find_item(rewardItem);
        if (found == 0)
        {
            // ORIG 0x081650ad..0x08165109：cMyTrace(PRETTY, 0x6e, 5) +
            // operator()("[HeroMission] Invalid item. (charac:%u, mission:%d,
            // item:%u)", get_charac_no(-1), m_missionCode, rewardItem)。
            char traceName[] =
                "bool BaseHeroMissionCondition::rewardProcess(CUser*)";
            cMyTrace trace(traceName, 0x6e, 5);
            trace("[HeroMission] Invalid item. (charac:%u, mission:%d, item:%u)",
                  static_cast<unsigned int>(user->get_charac_no(-1)),
                  static_cast<int>(m_missionCode),
                  static_cast<unsigned int>(rewardItem));
            return false;
        }

        // ORIG 0x08165119..0x08165162：Inven_Item ctor → m_addInfo = reward 低 half
        // → make_item(vtable 槽) → set_add_info(ushort@reward+4)。
        Inven_Item item;
        item.m_addInfo = rewardItem;
        found->make_item(item);
        item.set_add_info(static_cast<int>(it->m_addInfo));

        // ORIG 0x08165169..0x08165233。
        short slot = static_cast<short>(
            user->getCurCharacInvenW()->insertItemIntoInventory(
                item, eItemAddReason_0xd, true, true));
        if (slot >= 0)
        {
            user->SendUpdateItem(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0,
                                 static_cast<int>(slot));
        }
        else
        {
            // ORIG 0x08165238..0x081652c9：插入失败时按 mailTitle/mailContent 经
            // findString(table 4) 解析，发系统邮件兜底。
            const char* mailTitle =
                g_scriptStringManager_.findString(
                    4, m_condition.m_mailTitle.c_str(), 0);
            const char* mailContent =
                g_scriptStringManager_.findString(
                    4, m_condition.m_mailContent.c_str(), 0);
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                mailTitle, item, 0,
                static_cast<unsigned int>(user->get_charac_no(-1)),
                mailContent, static_cast<unsigned int>(strlen(mailContent)),
                0, user->GetServerGroup(), false, false);
        }
    }
    return true;   // ORIG 0x08165331：循环正常结束返回 1。
}
