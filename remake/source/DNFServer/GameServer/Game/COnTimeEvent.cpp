// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 COnTimeEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// COnTimeEvent.cpp —— 按报告 docs/class_func_reports/COnTimeEvent/*.md 忠实转写。
#include "COnTimeEvent.h"

#include <cstdio>

#include "GameTypes.h"          // cMyTrace
#include "CMyFileLog.h"
#include "CEnvironment.h"
#include "CUser.h"
#include "GameWorld.h"
#include "DB_InsertOnTimeRewardUserLog.h"

// ORIG 自由函数（权威声明 CInventory.h:408；按项目惯例局部声明）。
char* NumberToString(unsigned int value, int radix);

// ===================== 生命周期 =====================

#include "COnTimeEvent/COnTimeEvent.inc"


#include "COnTimeEvent/StartEvent.inc"


#include "COnTimeEvent/EndEvent.inc"


#include "COnTimeEvent/StartEvent_1.inc"


// ===================== getter/setter =====================

#include "COnTimeEvent/set_ontime_id.inc"
#include "COnTimeEvent/get_ontime_id.inc"
#include "COnTimeEvent/set_reward_state.inc"
#include "COnTimeEvent/get_reward_state.inc"

#include "COnTimeEvent/set_reward_end_time.inc"


#include "COnTimeEvent/get_reward_end_time.inc"

#include "COnTimeEvent/set_ontime_reward_item.inc"

#include "COnTimeEvent/get_ontime_reward_item_no.inc"
#include "COnTimeEvent/get_ontime_reward_item_count.inc"

#include "COnTimeEvent/get_ontime_reward_user_count.inc"


// ===================== 业务流程 =====================

#include "COnTimeEvent/process_set_reward_start.inc"


#include "COnTimeEvent/insert_reward_user.inc"


#include "COnTimeEvent/delete_reward_user.inc"


#include "COnTimeEvent/process_send_reward_to_user.inc"

