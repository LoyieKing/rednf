// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CUserProc/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— APSystem::CUserProc（G2-4 批次，活动点/奖励动作处理）。
// 依据 docs/class_func_reports/APSystem__CUserProc.md + ORIG 反汇编。
// 多数方法 ORIG 即为空实现（奖励/广播在目标构建中禁用）；这里按 ORIG
// 语义转录，保留真实调用与消息构造。
// ============================================================================

#include <stdio.h>
#include <string.h>

#include "CUserProc.h"   // APSystem::CUserProc / _ActionGroupIndex（唯一声明点）
#include "CUser.h"       // CUser 权威定义（m_actionPoint @+0x8d264）
#include "InterfacePacketBuf.h"
#include "PacketGuard.h"

// [b28c-4] ORIG 0x081224ac 载荷为 5 字节紧凑槽（flag@-0x19、itemIdx@-0x18）
// ⇒ pack(1) 结构体（探针 /tmp/l4/reward.cpp g2 与 ORIG 逐条全等）。
struct UserProcRewardPayload
{
    char flag;
    int itemIdx;
} __attribute__((packed));

#include "CUserProc/ClearActionAndSendtoUser.inc"
}

#include "CUserProc/SendActionClear.inc"
}

#include "CUserProc/SendActionListState.inc"
}

#include "CUserProc/SendActionListState_1.inc"
}

// [b28d-solo-CUserProc N-G] ORIG 0x081224ac：52 条与重建全等（含 EH landing / 帧 0x40 /
// 槽 -0x19/-0x18/-0x2c），p167 8/8 CLEAR；唯一残差 = put_binary 实参装载序：ORIG(4.1)
// 先 `lea -0x19(%ebp),%eax` 再 `movl $0x5,0x8(%esp)`，4.4 在直线块内恒反（先压 arg1）。
// 该序由基本块上下文决定而非源码形：同文件 SendActionClear 的 put_binary 位于 if/else
// 汇合块首，4.4 即产 ORIG 的 lea-先形（该件 AE 实证）⇒ 源码不可控。穷举 18 形（cast /
// (void*)中转 / reinterpret_cast / &首成员 / 具名指针 / volatile 指针 / 具名 int /
// sizeof / 语句表达式 / 逗号表达式 / 地址算术 / 5u / (int)5 / 具名 receiver / volatile
// 对象 / 指针+0 / 5+0 / 宏形）全败，且 4.4.6 后端读数与 4.4.7 逐条相同（非后端敏感族，
// P230 补）⇒ 判 N-G（P75 跨版本实参装载时刻）。
#include "CUserProc/SendGetRewardItem.inc"
}

#include "CUserProc/SetTodayActionAndCheckMedalReward.inc"
}

#include "CUserProc/UpdateActionPoint.inc"
}

#include "CUserProc/GetTodayRewardAndSendPostal.inc"
}

#include "CUserProc/CheckAndSendToMessageAllUser.inc"
}

#include "CUserProc/SendToMessageAllUser.inc"
}

#include "CUserProc/GM_Reset.inc"
}

#include "CUserProc/GM_Set.inc"
}
