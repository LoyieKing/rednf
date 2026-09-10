// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 BingoEventLink4/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// BingoEventLink4.cpp —— Bingo 域实现批次（OrigBingoEvt）
//
// BingoEvent 权威声明：BingoEvent.h（继承 CEventBase，成员
//   boost::shared_ptr<CBingo> m_bingo @ +0x0c；ORIG ctor 0x080c9984 new
//   CBingo(0x1c) 后挂到该槽，getBingoSystem 0x080c9fea 返回其地址）。
// CBingo 唯一类定义：CBingoTest.h/.cpp（Main 属主收敛；C1/D1/
//   getBingoRandomVaule/calNewValue/getIndexTodayBoard/calBoradMatchLine/
//   getToday×2/resetBoard/resetQuiz）。
// 本 TU 提供：BingoEvent C1/D1/D0/StartEvent×2/EndEvent/StartAction/
//   dailyresetData/reqBingoMark/getBingoSystem/sendOneMatchLineReward，
//   CUser::sendBingoData，以及 ORIG 全局位工具 isSetBit_Uint/setBit_Uint。
//
// 报告：docs/class_func_reports/{BingoEvent,CBingo,CUser}/（全文已读）。
// ============================================================================

#include "BingoEvent.h"
#include "CBingoTest.h"
#include "CEnvironment.h"
#include "CSystemTime.h"
#include "CGameManager.h"
#include "Action.h"

#include "GlobalData.h"
#include "CGuildServerProxy.h"
#include "CDataManager.h"
#include "CUser.h"
#include "Inven_Item.h"
#include "CItem.h"
#include "CStackableItem.h"   // m_vec3a0（+0x3a0 标记号 vector 具名起点）
#include "CMailBoxHelper.h"
#include "RDARScriptStringManager.h"
#include "LogManager.h"
#include "PacketGuard.h"

#include <bitset>
#include <boost/foreach.hpp>   // ORIG：StartAction/dailyresetData 用 BOOST_FOREACH 遍历 +0x22c map
#include <map>
#include <vector>
#include <time.h>
#include <string.h>

// C1 @ 0x080c9984：CEventBase() + vptr + new CBingo(0x1c) 包 shared_ptr<CBingo>
// 挂到 this+0xc。初始化列表 m_bingo(new CBingo()) 顺序与 ORIG 一致。
#include "BingoEventLink4/BingoEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 全局位工具（source 无既有 producer，objdump 直转）：
//   _Z13isSetBit_Uintji T 0x0889232e：(value & (1<<bit)) != 0
//   _Z11setBit_UintRjbi T 0x088922d7：set ? |= : &=~
#include "BingoEventLink4/isSetBit_Uint.inc"
}

#include "BingoEventLink4/setBit_Uint.inc"
}

// ---------------------------------------------------------------------------
// BingoEvent 方法（docs/class_func_reports/BingoEvent/ 13 报告全文已读）。
// ---------------------------------------------------------------------------

// D1 @ 0x080c9a06：vptr + shared_ptr<CBingo> 析构 + CEventBase 析构（编译器按
// 声明逆序生成：m_bingo 先于 CEventBase）。D0 @ 0x080c9a68 由编译器生成。
#include "BingoEventLink4/dtor_BingoEvent.inc"
}

// getBingoSystem @ 0x080c9fea：返回 this+0xc —— 成员 boost::shared_ptr<CBingo>
// 实例地址（ORIG ctor 将 new CBingo(0x1c) 挂到该槽；调用方经 operator-> 解引用）。
#include "BingoEventLink4/getBingoSystem.inc"
}

// StartEvent @ 0x080c9a86：置事件标志；按当前秒与疲劳小时槽（G_CEnvironment
// +0x37c）取日号，1..31 内重置棋盘与 quiz，打日志。
#include "BingoEventLink4/StartEvent.inc"
}

// StartEvent(Word_Param) @ 0x080c9b68：经 vptr+0x8（slot 2）调无参 StartEvent()，
// 打日志。param 未参与计算（ORIG 如此）。
#include "BingoEventLink4/StartEvent_1.inc"
}

// StartAction @ 0x080c9bb8：遍历 CGameManager+0x22c（accId→CUser* map），
// 对每个在线用户发 bingo 数据包。
// [b28d-D9] 修复/N89：BOOST_FOREACH 复现（102→5）。原「残 = getUserByAccIdMap
// inline getter out-of-line call」已消除：ORIG 无该 getter 符号（直读 +0x22c），
// 调用点改 friend 直访 m_userByAccId。
#include "BingoEventLink4/StartAction.inc"
}

// EndEvent @ 0x080c9d30：置事件标志 + 日志。
#include "BingoEventLink4/EndEvent.inc"
}

// dailyresetData @ 0x080c9d82：按日重置棋盘/quiz；随后遍历在线用户，清
// BingoData、重发数据包并刷新装饰物计数。
// [b28d-D9] 修复/N89：BOOST_FOREACH 复现（143→5）。原「残 = getUserByAccIdMap
// inline getter out-of-line call」已消除（同上：friend 直访 m_userByAccId）。
#include "BingoEventLink4/dailyresetData.inc"
}

// reqBingoMark @ 0x080c9ff6（docs/class_func_reports/BingoEvent/reqBingoMark.md）：
// user/item 判空 → 0x13；已标记 3+ 次 → 0x7；物品请求号越界 → 0x17；随机值
// 无效 → 0x13；标记成功且新增连线时逐线发奖励，回 0x1c3 包，返回 0。
// [b28d-D9] P209/P238：unsigned 局部 for 尾测 4.4 恒 mem 直比 + 常量 store 中转形（ORIG 0x080c9ff6 sz 0x0000026c）
#include "BingoEventLink4/reqBingoMark.inc"
}

// sendOneMatchLineReward @ 0x080ca3d4：读 m_bingo.m_map30[CDataManager+0xb568+0x30
// =+0xb598] 的 key=1（单线奖励配置 <type, value>），按 type 构造 Inven_Item 后
// 以系统邮件发放。
// [SweepBingoEventLink4] 已闭合（AE 143/143，原 DIFF 144/145）：count 由
// unsigned int 改 int ⇒ 复现 ORIG 0x080ca5cd `mov -0x10(%ebp),%edx`（压栈前
// 提前装载 count，与形参 unsigned int 异型触发 4.4 同形）；unsigned 版 4.4
// 恒延迟到 `mov %eax,0x8(%esp)` 前装载（P211 档③/P75 反例，探针实测）。
#include "BingoEventLink4/sendOneMatchLineReward.inc"
}

// ---------------------------------------------------------------------------
// sendBingoData @ 0x0869134e：取 repeat 事件 0x6a（BingoEvent）当日棋盘号，
// 连同用户 bingo 数据（棋盘值 +0x00、连线数 +0x05）打包 0x1c2 发送。
#include "BingoEventLink4/sendBingoData.inc"
}
