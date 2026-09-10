// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CEventCeraShopRewardPoint/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Cera 商城奖励点数事件 CEventCeraShopRewardPoint 实现。
// 报告：docs/class_func_reports/CEventCeraShopRewardPoint/（C1 0x0816459a /
// D1 0x081645ca / D0 0x081645fa / StartEvent 0x08164618 /
// StartEvent(Word_Param) 0x08164684 / EndEvent 0x081646b8 /
// SendReward 0x0816471e）。
#include "CEventCeraShopRewardPoint.h"

#include <cstring>              // strncpy / strlen
#include "GameTypes.h"          // cMyTrace
#include "RDARScriptStringManager.h"   // g_scriptStringManager_ / findString
#include "CMailBoxHelper.h"     // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail
                                //   （含 Inven_Item / ENUM_SERVER_GROUP）
#include "CUser.h"              // GetServerGroup / get_charac_no

// ORIG 0x0816459a：基类构造 + vptr 置位 + 清 m_field10(+0x10) / m_fieldC(+0x0c)
// （GCC 自动按声明序：先 +0x10 后 +0x0c，与 ORIG movl $0,0x10 → movl $0,0xc 同形）。
#include "CEventCeraShopRewardPoint/CEventCeraShopRewardPoint.inc"
}

// ORIG 0x081645ca / 0x081645fa：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
#include "CEventCeraShopRewardPoint/dtor_CEventCeraShopRewardPoint.inc"
}

// ORIG 0x08164618：SetEventFlag(true) + cMyTrace(PRETTY, 0x13a, 0)
//   ("[Taiwan, CeraShopRewardPoint] Start event. Timing:%d, CeraPoint:%d ",
//    m_field10, m_fieldC)。
#include "CEventCeraShopRewardPoint/StartEvent.inc"
}

// ORIG 0x08164684：m_field10 = param.m_duration（低 ushort），
// m_fieldC = param.m_field2（高 ushort），再经 vptr slot2 虚调 StartEvent()。
// [b28c-31] N7 复核：末尾虚调用 callee ORIG %edx vs 4.4 %eax（20=20）。
#include "CEventCeraShopRewardPoint/StartEvent_1.inc"
}

// ORIG 0x081646b8：cMyTrace(PRETTY, 0x147, 0) + tr("[Taiwan, CeraShopRewardPoint]
// End event.") + 清 m_field10 / m_fieldC + SetEventFlag(false)。
#include "CEventCeraShopRewardPoint/EndEvent.inc"
}

// ORIG 0x0816471e：RDAR 字符串取标题/正文，附道具（idx 0x28ec3d @ m_addInfo，
// add_info = m_fieldC @ m_addInfo2）经 ReqDBSendNewSystemMail 发系统邮件。
#include "CEventCeraShopRewardPoint/SendReward.inc"
}
