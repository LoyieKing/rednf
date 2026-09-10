// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CMaxFatigueFactorEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 最大疲劳系数事件 CMaxFatigueFactorEvent 实现。
// 报告：docs/class_func_reports/CMaxFatigueFactorEvent/ 全部 6 个方法
// （CMaxFatigueFactorEvent / ~CMaxFatigueFactorEvent /
//  ~CMaxFatigueFactorEvent_081b276e / StartEvent / StartEvent_081b27e8 /
//  EndEvent）。
#include "CMaxFatigueFactorEvent.h"

#include "GameTypes.h"             // cMyTrace
#include "CLocalChina_User.h"      // CLocalChina_User::SendFatigueEvent（static）
#include "GlobalData.h"
#include "GameWorld.h"
#include "CUser.h"                 // CUser / _Charac_info 完整定义（含 CUserCharacInfo）

extern int DEFAULT_MAX_FATIGUE;    // CUserCharacInfo.h 亦声明（ORIG B 0x9370f5c）

// ---------------------------------------------------------------------------
// ORIG 0x0812ca88（CLocalChina_User::SendFatigueEvent，报告
// CLocalChina_User/SendFatigueEvent.md）：
// GetRepeatEvent(2) → do-while 遍历 m_UserInWorld（循环头 G_GameWorld()+
// end()+!= 判定，不符即 return）→ state>2 才处理：ev 存在时经 getCurCharacW
// 写 ch->m_field35（IsEventing(0) ? factor*DEFAULT_MAX_FATIGUE/100 :
// DEFAULT_MAX_FATIGUE；ch==0 跳过 premium/SendFatigue 直入 ++it）；
// IsHavePremiumAdvantage 时按事件是否生效分两路 setCurCharacMaxPremiumFatigue
// （生效：事件缩放差 + 优惠倍率两段 /100；未生效：当前上限 × 优惠倍率 /100）；
// 最后 CUser::SendFatigue()。
// [b28d-28] DIFF | ORIG 0x0812ca88 | P13/P28 已修（while 形 + `it++` 复现
// `jmp cond` + 尾部 `ppEi`）；残差 N82：我方具名中间量（rate1/iVar3/uVar6/
// rate2/curMax/rate）致帧 0x54 vs ORIG 0x30（P0/P183 内联化未完成）。
#include "CMaxFatigueFactorEvent/SendFatigueEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x081b2718：基类构造 + vptr 置位 + m_maxFatigueFactor = 100。
#include "CMaxFatigueFactorEvent/CMaxFatigueFactorEvent.inc"
}

// ORIG 0x081b273e / 0x081b276e：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
#include "CMaxFatigueFactorEvent/dtor_CMaxFatigueFactorEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x081b278c：SetEventFlag(true) + m_maxFatigueFactor = 100 +
// cMyTrace(__FUNCTION__,0x16,0)("Max Fatigue Factor Event Start!")。
#include "CMaxFatigueFactorEvent/StartEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x081b27e8：SetEventFlag(true) + m_maxFatigueFactor = param.m_duration +
// cMyTrace(__FUNCTION__,0x1e,0)(同字符串) + SendFatigueEvent()。
#include "CMaxFatigueFactorEvent/StartEvent_1.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x081b284c：SetEventFlag(false) + m_maxFatigueFactor = 100 +
// cMyTrace(__FUNCTION__,0x2a,0)("Max Fatigue Factor Event End!") +
// SendFatigueEvent()。
#include "CMaxFatigueFactorEvent/EndEvent.inc"
}
