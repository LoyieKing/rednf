// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 W9HeroCondition/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "HeroMissionCondition.h"

#include <algorithm>
#include <iterator>
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

// ---------------------------------------------------------------------------
// HeroMissionCondition C1/C2 —— ORIG 0x081681c8：仅依次构造 +0x0c/+0x10
// std::string 与 +0x14/+0x20 vector；标量区不初始化。
#include "W9HeroCondition/HeroMissionCondition.inc"
    : m_mailTitle(), m_mailContent(), m_rewards(), m_extra()
{
}

// ORIG copy ctor 0x08964f5c：拷贝 +0..+0x9 标量后逐成员拷贝构造两 string、
// 两 vector（vector<RewardData> copy ctor = ORIG C1ERKS3_）。
#include "W9HeroCondition/HeroMissionCondition_1.inc"
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
#include "W9HeroCondition/dtor_HeroMissionCondition.inc"
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
    std::copy(other.m_rewards.begin(), other.m_rewards.end(),
              std::back_inserter(m_rewards));
    std::copy(other.m_extra.begin(), other.m_extra.end(),
              std::back_inserter(m_extra));
    return *this;
}

// ORIG clear 0x08961376：置 +0/+4 为 0、+8/+9 为 0，再 clear 两个 string 与
// 两个 vector。
#include "W9HeroCondition/clear.inc"
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
#include "W9HeroCondition/BaseHeroMissionCondition.inc"
    HeroMissionCondition::MissionNo::T no)
    : m_condition(), m_missionCode(no)
{
}

// ORIG D1 0x08164f3a：写回基类 vptr 后析构内嵌 condition；D0 0x08164f6e 为
// deleting 形态。虚析构定义于本 TU 保证唯一 vtable/typeinfo 输出。
#include "W9HeroCondition/dtor_BaseHeroMissionCondition.inc"
{
}

// ORIG setConditionData 0x08164f8c：HeroMissionCondition::operator= 拷到 this+8。
#include "W9HeroCondition/setConditionData.inc"
{
    m_condition = data;
}

// ORIG getMissionCode 0x08164faa：返回 *(unsigned int*)(this+0x4)。
#include "W9HeroCondition/getMissionCode.inc"
{
    return m_missionCode;
}

// ORIG getMissionType 0x08164fb6：mov 0xc(%eax) → this+0xc = condition+0x04，
// 即 m_field04（头注释原误标 +0x00，objdump 实证修正）。
#include "W9HeroCondition/getMissionType.inc"
{
    return static_cast<unsigned int>(m_condition.m_field04);
}

// ORIG isRepeatMission 0x08164fc2：movzbl *(this+0x10) = condition 字节 +0x08。
// [b28c-L26 档①] 定义返回类型随头改 bool（P159：mangled 不含返回类型，ABI 不变）。
#include "W9HeroCondition/isRepeatMission.inc"
{
    return m_condition.m_isRepeatMission;
}

// ORIG isResetState 0x08164fce：movzbl *(this+0x11) = condition 字节 +0x09。
#include "W9HeroCondition/isResetState.inc"
{
    return m_condition.m_isResetState;
}

// ORIG getMailTitle 0x08164fda：sret，std::string 拷贝构造自 this+0x14 =
// condition +0x0c。
#include "W9HeroCondition/getMailTitle.inc"
{
    return m_condition.m_mailTitle;
}

// ORIG getMailContent 0x08165004：sret，拷贝构造自 this+0x18 = condition +0x10。
#include "W9HeroCondition/getMailContent.inc"
{
    return m_condition.m_mailContent;
}

// ORIG rewardProcess 0x0816502e：遍历 rewards；逐项 find_item → make_item →
// set_add_info → insertItemIntoInventory(item, eItemAddReason_0xd, true, true)；
// 成功 SendUpdateItem(1, 0, slot)；失败经 RDARScriptStringManager::findString
// (table 4) 解析邮件标题/正文并发系统邮件。找不到物品则 trace 并整体 return false；
// 循环正常结束返回 true。
             
// [b28d-7 N7/N18a] ORIG 0x0816502e：`mov (%eax),%eax; mov %eax,%ebx` 两段物化 + 间接 call 目标 %edx，4.4 折单段并改 %eax；P167 CLEAR ⇒ 封顶。
#include "W9HeroCondition/rewardProcess.inc"
{
    for (std::vector<HeroMissionCondition::RewardData>::iterator it =
             m_condition.m_rewards.begin(),
         end = m_condition.m_rewards.end();
         it != end; ++it)
    {
        // ORIG 0x0816507e：pt() → ebx（跨 G_CDataManager 调用缓存，P52），
        // G_CDataManager() 直返调用 find_item（无判空，P15 形态）。
        CItem* found = G_CDataManager()->find_item(it.operator->()->m_itemIdx);
        if (found == 0)
        {
            // ORIG 0x081650a3..0x08165109：临时链式 cMyTrace（P37/f2 探针实证：
            // operator() 实参右→左先求值并 edi/esi/ebx 缓存，然后才构造临时）。
            cMyTrace(__PRETTY_FUNCTION__,
                     0x6e, 5)(
                "[HeroMission] Invalid item. (charac:%u, mission:%d, item:%u)",
                user->get_charac_no(-1), static_cast<int>(m_missionCode),
                it->m_itemIdx);
            return false;
        }

        // ORIG 0x08165119..0x08165162：Inven_Item ctor → m_addInfo = reward 低 half
        // → make_item(vtable 槽) → set_add_info(ushort@reward+4)。
        Inven_Item item;
        item.m_addInfo = it->m_itemIdx;
        found->make_item(item);
        item.set_add_info(static_cast<int>(it->m_addInfo));

        // ORIG 0x08165169..0x08165233。
        short slot = -1;
        slot = static_cast<short>(
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