// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 COnTimeEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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

// ORIG C1 @0x081bb826：基类 ctor → CRewardUserList ctor(+0x14) →
// +0x0c=0、+0x10=1 → clear()。
#include "COnTimeEvent/COnTimeEvent.inc"
}

#include "COnTimeEvent/StartEvent.inc"
}

#include "COnTimeEvent/EndEvent.inc"
}

#include "COnTimeEvent/StartEvent_1.inc"
}

// ===================== getter/setter =====================

#include "COnTimeEvent/set_ontime_id.inc"
#include "COnTimeEvent/get_ontime_id.inc"
#include "COnTimeEvent/set_reward_state.inc"
#include "COnTimeEvent/get_reward_state.inc"
#include "COnTimeEvent/set_reward_end_time.inc"
}

#include "COnTimeEvent/get_reward_end_time.inc"

#include "COnTimeEvent/set_ontime_reward_item.inc"
}
#include "COnTimeEvent/get_ontime_reward_item_no.inc"
#include "COnTimeEvent/get_ontime_reward_item_count.inc"

#include "COnTimeEvent/get_ontime_reward_user_count.inc"
}

// ===================== 业务流程 =====================

// ORIG @0x081bbce0（process_set_reward_start）。
#include "COnTimeEvent/process_set_reward_start.inc"
}

// ORIG @0x081bb9dc。双日志 + 已领过判定。
// 汇编要点：① 两个 256 数组为 { '\0' } 初始化（rep stos 内联，非 call memset）；
// ② 比较 setg（get_ontime_id() > last_recv_idx()，先求左操作数）；
// ③ insert 实参右→左内联求值 get_ontime_id→get_charac_no→get_acc_id；
// ④ insert 后先 set_ontime_last_recv_idx(get_ontime_id()) 再 set_update；
// ⑤ else 分支求值序 get_charac_no→get_acc_id→NumberToString→last_recv→ontime_id。
// [b28c-lane17] fresh：158=158 助记符全等 ⇒ NEAR；残差为互斥块内同类 cMyTrace/
// char[0x100] 临时槽深方向与 callee-saved 分配差（N82附/N86 族），
// 调用点差集与槽位集无缺项 ⇒ 语义完成、byte 层 N 封顶。
#include "COnTimeEvent/insert_reward_user.inc"
}

// ORIG @0x081bbc64。
#include "COnTimeEvent/delete_reward_user.inc"
}

// ORIG @0x081bbd46：遍历 key 队列（上限 0x33 个/次），逐个投递奖励。
// 汇编要点：① while (!m_index.empty()) 前置 jmp 循环头（P25 while 形态）；
// ② 循环尾 ++processed 先、pop_front 后、cmpl $0x32; jg break；
// ③ iterator 函数头默认构造（P28 声明序 it→user→processed）；
// ④ 条件嵌套 if（两个独立 je，非 && 汇聚）；⑤ 比较用 it->second.a（charac_no）。
#include "COnTimeEvent/process_send_reward_to_user.inc"
}
