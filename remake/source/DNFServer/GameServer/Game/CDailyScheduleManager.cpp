// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CDailyScheduleManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ CDailyScheduleManager 实现（批3：从 GameStubs.cpp 迁移）。
#include "CDailyScheduleManager.h"

#include "CDungeon.h"
#include "GameWorld.h"
#include "CSystemTime.h"
#include "GlobalData.h"
#include "CGuildServerProxy.h"

#include <ctime>

#include "CDailyScheduleManager/AddDailSchedule.inc"
}

// ORIG 0x81266e2：size() 缓存 + while 循环；命中条件 (m_idx==idx && type==m_type)
// 且 m_state==1 时返回其 state；未命中返回 0。
#include "CDailyScheduleManager/GetScheduleState.inc"
}

// ORIG 0x8126dc0：找到 (m_idx==idx && m_type==type) 条目，把其拷贝到 out（+0x20 清 0），
// 并置该条目 m_wday=7、起始时分秒(0,0,0)、结束时(23,50,50)；随后第二遍处理 m_type==1
// （进行中）的同类条目（GM 强制开启）。
// [b28d-solo-CDailyScheduleManager] AE：m_type 经匿名 union 的 enum 视图 m_typeE 与形参比较，
// 复现 ORIG `mov 0x4(%eax),%eax; cmp 0x10(%ebp),%eax`（P254 enum 字段保留直载；int 字段 4.4
// 恒两段 `mov,%edx; mov,%eax; cmp`）；m_idx unsigned 化（P211 档③）令 idx 形参出 ebx 缓存形。
// p167 CLEAR。
#include "CDailyScheduleManager/GM_OpenSchedule.inc"
}

// ORIG 0x812707c：找到 (m_idx==idx && m_type==type && idx==out.m_idx) 条目，按其当前
// m_state 写 out.state（进行中则 1，否则 0），memcpy 回条目；第二遍对
// (m_idx==idx && m_type==idx && idx==out.m_idx) 条目做 memcpy（ORIG 第二遍用 idx 比较
// m_type，历史行为照抄）。
// [b28d-solo-CDailyScheduleManager] AE：循环体内 `register long curIdx = idx;`（P231 纯
// register，每轮重载 %ebx，与 ORIG 装载点同层；long 与 unsigned m_idx 异型使 4.4 出
// `cmp %ebx,%eax`）+ m_typeE 视图 + 第三处比较回退形参 idx（复现 ORIG `mov 0xc(%ebp),%edx`）。
// p167 CLEAR。
#include "CDailyScheduleManager/GM_CloseSchedule.inc"
}

// ORIG 0x8126c5c / 0x8126c8a：KingHorn 打开/关闭（id=1, type=3）。
#include "CDailyScheduleManager/GM_OpenKingHorn.inc"
#include "CDailyScheduleManager/GM_CloseKingHorn.inc"
#include "CDailyScheduleManager/GM_OpenBossTower.inc"
}
#include "CDailyScheduleManager/GM_CloseBossTower.inc"
}

// ORIG 0x8126d60 / 0x8126d90：AdvanceAltar 打开/关闭（id=10, type=4）。
#include "CDailyScheduleManager/GM_OpenAdvanceAltar.inc"
#include "CDailyScheduleManager/GM_CloseAdvanceAltar.inc"
#include "CDailyScheduleManager/GM_OpenSouthernDale.inc"
}

#include "CDailyScheduleManager/GM_CloseSouthernDale.inc"
}
#include "CDailyScheduleManager/CheckAndSendToAllUserSchedule.inc"
}

// ORIG 0x8125cb8：CheckAndSendToUserSchedule(&user, false)。
#include "CDailyScheduleManager/CheckAndSendToOneUserSchedule.inc"
}

// ORIG 0x8125ff4：按 _DailySchedule 时间窗与 CEventManager::GetRepeatEvent
// 判定条目应处状态；返回 1=进入（或进行中），0=不进入。
// [b28d-solo-CDailyScheduleManager] 已收：两处 GetRepeatEvent 实参经 m_eventIdE（enum 视图）
// 复现 ORIG `mov (%eax),%eax; mov %eax,%edx` 两段（P254）。
// 残差 1 条 mov 判 N18b 封顶：ORIG `mov 0x14(%eax),%eax; mov %eax,%edx; mov -0x18(%ebp),%eax;
// cmp %eax,%edx`，4.4 对成员==局部比较恒折 `mov 0x14(%eax),%edx` 单段；字段 int/unsigned/
// enum 视图 × 局部 int/unsigned/long/enum 十二形实测均不翻（P248 三档试尽）。
#include "CDailyScheduleManager/CheckScheduleState.inc"
}

// ORIG 0x81261a8：在 m_schedules 中查找与 entry 同 m_idx+m_type 的相邻条目。
// 返回 true 并回填 prev（前一条）或 next（后一条）；两条均未命中返回 false。
// [b28d-solo-CDailyScheduleManager] AE：①5 个 tm 声明序倒置（ORIG 槽浅→深=声明序，P309）；
// ②time_t 序 cur/startT/endT（ORIG -0x28/-0x2c/-0x30）；③m_typeE 视图；
// ④tm 成员比较左置（`tmEnd.tm_hour == m_schedules[i].m_startHour` 复现 ORIG 先载 tm 成员
// 入 ebx 跨 operator[] call 保活）；⑤isNext/isPrev 块互换 + `cur >= s`（P222/P258）；
// ⑥wday2 unsigned（P211 档③）；⑦while→for 复现 ORIG 循环增量落在分支 jmp 之后的底部条件
// 布局（P283 同族）。p167 CLEAR。
#include "CDailyScheduleManager/CheckContinueSchedule.inc"
}

// ORIG 0x8125cda 主循环，逐指令转写：
//  - 仅 user==0（广播模式）时做 CheckScheduleState 状态翻转；
//  - 翻转后经 CheckContinueSchedule 联动 prev/next 条目的 m_state；
//  - changed → 广播 0x1ed {type,state,idx,out}；!changed 且条目进行中且 user!=0
//    → 单播 0x1ed {type,2,idx,out}。
// [b28d-solo-CDailyScheduleManager] AE：①body/bodyU 移入各自块内（packet 之后声明），
// 复现 ORIG 数组占块内深槽（4.4 聚合优先深槽，N82附）；②cur/out/prev/next 声明序倒置
// （P309 先声明浅）；③单播块改 `else if (!changed && ...)` 复现 ORIG 广播块尾 jmp +
// 冗余 !changed 检查。p167 CLEAR。
#include "CDailyScheduleManager/CheckAndSendToUserSchedule.inc"
}
