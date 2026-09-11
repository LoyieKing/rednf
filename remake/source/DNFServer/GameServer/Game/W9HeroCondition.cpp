#define _GLIBCXX_IOSTREAM 1
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

#include "W9HeroCondition/HeroMissionCondition.inc"


#include "W9HeroCondition/HeroMissionCondition_1.inc"


#include "W9HeroCondition/dtor_HeroMissionCondition.inc"


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

#include "W9HeroCondition/clear.inc"


#include "W9HeroCondition/BaseHeroMissionCondition.inc"


#include "W9HeroCondition/dtor_BaseHeroMissionCondition.inc"


#include "W9HeroCondition/setConditionData.inc"


#include "W9HeroCondition/getMissionCode.inc"


#include "W9HeroCondition/getMissionType.inc"


#include "W9HeroCondition/isRepeatMission.inc"


#include "W9HeroCondition/isResetState.inc"


#include "W9HeroCondition/getMailTitle.inc"


#include "W9HeroCondition/getMailContent.inc"


// ORIG rewardProcess 0x0816502e：遍历 rewards；逐项 find_item → make_item →
// set_add_info → insertItemIntoInventory(item, eItemAddReason_0xd, true, true)；
// 成功 SendUpdateItem(1, 0, slot)；失败经 RDARScriptStringManager::findString
// (table 4) 解析邮件标题/正文并发系统邮件。找不到物品则 trace 并整体 return false；
// 循环正常结束返回 true。
             
#include "W9HeroCondition/rewardProcess.inc"
