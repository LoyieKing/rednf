// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CConditionEventManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================

#include "CConditionEventManager.h"
// ============================================================================
// b28d-52 判词表（本 TU）：
//   已修：_getRequiredParameter 0x083356a4（P236/P293 内联实参翻槽序）、
//     IsFreeGoldCard 0x0833579e（P0 单变量复用）、ProcessCheckStepUp 0x08335566
//     （P286 单 case switch）⇒ 均转 AE。
//   剩余 DIFF：
//   GetConditionRewardInfo 0x083353a0 —— N86（4.1 iterator 临时槽+拷贝两段；
//     4.4 copy-elision 直构 ⇒ 槽序 -0x28/-0x24 不可复现）
//   RequestConditionEventReward 0x08334850 —— 已修：删发明 helper insertFailSendMail
//     （ORIG nm 0 命中）改两处内联 + inv.m_addInfo(+0x02) 缓存 itemIdx + guard 内联短路
//     + GetItemType 双调/否定守卫块序 + slot 两段式(-1 初始化) + bool ok 内联 ⇒
//     gap 76→4、帧 0x25c→0x24c 全对齐、p167 双向差集仅剩 C1/C2·D1/D2 同址别名（P93）。
//     残余 4 条 = ORIG `mov slot,%eax; shr $0x1f,%eax; test %al,%al; je` vs 4.4
//     `cmpl $0x0,slot; jns`（2 处 ×2 条；`slot<0` 与 `(unsigned)slot>>31` 两形实测
//     4.4 均折 cmpl/jns ⇒ 跨版本 lowering，记 N）。
// ============================================================================


#include <string.h>

#include "GlobalData.h"
#include "CGuildServerProxy.h"
#include "CDataManager.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CParty.h"
#include "CItemList.h"
#include "Inven_Item.h"
#include "CInventory.h"
#include "CCeraShop.h"
#include "CSystemTime.h"
#include "RDARScriptStringManager.h"
#include "CMailBoxHelper.h"
#include "PacketGuard.h"


// ORIG T 0x08334844：空体 C1（push/mov/pop/ret；C1/C2 同址 T，类无虚函数）。
// 调用点：CGameManager::GetConditionEventManager 的 placement new。
// 置于 TU 首个成员定义位，使 _GLOBAL__I_ 键名按 C2Ev（匹配 ORIG 键名，P108）。
#include "CConditionEventManager/CConditionEventManager.inc"
}

// ORIG T 0x0833484a：空体 D1（push/mov/pop/ret；D1/D2 同址，类无虚函数）。
#include "CConditionEventManager/dtor_CConditionEventManager.inc"
}

// ORIG T 0x08335264
// _ZN22CConditionEventManager30ConditionRewardSameItemCombineERSt6vectorI26stConditionEventRewardInfoSaIS1_EERSt3mapIiiSt4lessIiESaISt4pairIKiiEEE
// 遍历 infos，按 info.m_field0（物品 id）聚合 m_field4（数量）到 outMap：
// 已存在则累加 second，否则插入 {m_field0, m_field4}。
#include "CConditionEventManager/ConditionRewardSameItemCombine.inc"
}

// ORIG T 0x08334850
// _ZN22CConditionEventManager27RequestConditionEventRewardEP5CUserscRSt3mapIiiSt4lessIiESaISt4pairIKiiEEE
// 主奖励链：repeat 事件 0x71 未开启 → 0；curIdx 与 m_conditionEvent.m_field0 不符
// → 1；step<1 / step 超前 / rewardStep != step-1 → 0x13；物品缺失 → 0x11。
// 按 rewardMap 逐条发放：key==1 且 count>=0 → 事件币（EventCoinAdd 日志+保存+发送）；
// 其余 → find_item+create_item，guard（itemIdx==0 或 get_add_info()==0）跳过；
// 非特殊物品直接入库（失败邮件回退），特殊物品按 GetItemType()==0x10 / key==1
// 分支：0x10 直接入库，否则 ProcessSpecialItem（outRestrict!=0 → 0x11）。
// 全部成功 → IncCurConditionEventRewardStep() 并返回 0。
#include "CConditionEventManager/RequestConditionEventReward.inc"
}

// ORIG 0x08335754
// _ZN22CConditionEventManager16GetCurEventIndexEv
// 若当前重复事件 0x71 处于进行中（IsEventing(0) 真）返回 CDataManager
// m_conditionEvent.m_field0（+0x4d98，当前条件事件索引）；否则返回 0。
#include "CConditionEventManager/GetCurEventIndex.inc"
}

// ORIG T 0x083354f0 _ZN22CConditionEventManager16GetMaxRewardStepEs
// 事件 0x71 进行中且 (short)step == m_conditionEvent.m_field0 时返回
// m_conditionEvent.m_map8.size()（+0x4da0 map 的奖励步数上限）；否则 0。
#include "CConditionEventManager/GetMaxRewardStep.inc"
}

// ORIG T 0x083356a4 _ZN22CConditionEventManager21_getRequiredParameterEs
// sret pair<int,int>：查 m_conditionEvent.m_map8（G_CDataManager()+0x4da0，
// map<int,pair<int,int>>）键=step；命中 → {second.first, second.second}；
// 未命中 → {-1,-1}。ret $0x4（sret 指针随 ret 清栈）。
#include "CConditionEventManager/_getRequiredParameter.inc"
}

// ORIG T 0x083355e6 _ZN22CConditionEventManager31_processProperDungeonClearEventEP5CUsers
// 取当前条件事件步 curStep；_getRequiredParameter(step+1) 得 (required,-)；
// required != -1 且 required <= GetProperDungeonClearCount() → 步进 + 发送条件事件信息；
// 无条件发送清剿计数。ORIG 中 PacketGuard 局部对象构造后未使用（照抄保留）。
#include "CConditionEventManager/_processProperDungeonClearEvent.inc"
}

// ORIG T 0x08335a8a _ZN22CConditionEventManager21CheckErrorCardForStepEP5CUserc
// user 非空、IsProperDungeonUser()、step>=0 三关；_getRequiredParameter(step) 得
// required：==-1 或 < GetProperDungeonClearCount() → false；否则 true。
#include "CConditionEventManager/CheckErrorCardForStep.inc"
}

// ORIG T 0x083353a0
// _ZN22CConditionEventManager22GetConditionRewardInfoEcRSt6vectorI26stConditionEventRewardInfoSaIS1_EEsc
// 事件 0x71 未进行中直接返回；(short)curIdx 与 m_conditionEvent.m_field0 不符
// 直接返回；out.clear() 后按 useQuickPartyAdd 选 m_multiMap38(+0x4dd0)/m_multiMap20
// (+0x4db8)，取 equal_range(step) 全部 value push_back 进 out。
#include "CConditionEventManager/GetConditionRewardInfo.inc"
}

// ORIG T 0x083358bc _ZN22CConditionEventManager16IsDoubleGoldCardEP5CUserc
// CheckErrorCardForStep 不过 → false；普通集合（useQuickPartyAdd=0）按 step 收集
// 奖励信息，IsDoubleGoldCardProcess 为真 → 双倍；否则须为快速组队
// （GetParty 非空且 is_quick_party），再以快速集合（useQuickPartyAdd=1）更新判定。
#include "CConditionEventManager/IsDoubleGoldCard.inc"
}

// ORIG T 0x08335a3a
// _ZN22CConditionEventManager23IsDoubleGoldCardProcessERSt6vectorI26stConditionEventRewardInfoSaIS1_EE
// 任一 info 的 m_flag9（+0x09 字节）非零 → true。
#include "CConditionEventManager/IsDoubleGoldCardProcess.inc"
}

// ORIG T 0x0833586c：任一 info.m_flag8（+0x08 字节）非零即免费。
#include "CConditionEventManager/IsFreeGoldCardProcess.inc"
}


// ORIG T 0x0833579e _ZN22CConditionEventManager14IsFreeGoldCardEP5CUserc
// CheckErrorCardForStep 不过 → false；取当前条件事件索引，按 step 收集奖励信息，
// IsFreeGoldCardProcess 判定免费金卡。
#include "CConditionEventManager/IsFreeGoldCard.inc"
}

// ORIG T 0x08335566 _ZN22CConditionEventManager18ProcessCheckStepUpEP5CUsers
// 事件 0x71 进行中且 (short)step == m_conditionEvent.m_field0 == 2 时推进晋级流程。
#include "CConditionEventManager/ProcessCheckStepUp.inc"
}
