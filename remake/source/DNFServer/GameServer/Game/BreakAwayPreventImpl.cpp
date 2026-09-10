// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 BreakAwayPreventImpl/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— break_away_prevent::CBreakAwayPreventSystem 实现批次
// 权威头 Game/BreakAwayPreventSystem.h（GlobalData.cpp ctor/dtor/loadDbData、
// W6_CeraShopDeps.cpp updateUsedCera 归属不变）。
//
// 已完整阅读的报告（docs/class_func_reports/break_away_prevent__CBreakAwayPreventSystem/）：
//   getDiffConnectDay.md / calBreakAwayUserPoint.md / selectReward.md /
//   startBreakAwayPreventSystem.md / updateUvList.md /
//   loadDbData.md / CBreakAwayPreventSystem.md / ~CBreakAwayPreventSystem.md /
//   updateUsedCera.md
// 本 TU 落地（ORIG nm -S --defined-only 核实地址/大小）：
//   getDiffConnectDay              T 0x0831c794 (0x7b)
//   calBreakAwayUserPoint          T 0x0831c810 (0x61)
//   selectReward                   T 0x0831c872 (0x1e7)
//   updateUvList                   T 0x0831cb56 (0x308)
//   startBreakAwayPreventSystem    T 0x0831ce78 (0x2ec)
//   DB_UpdateBreakAwayUvList::makeRequest T 0x0843873a (0x107)
//     —— 独立报告缺失，按指南 §3.2 以 objdump 区间转写并标注。
//
// ⚠ 需 CMake 登记（新建 TU，加入 game 目标源列表）。
// ============================================================================

#include "BreakAwayPreventSystem.h"
#include "CUser.h"

#include "SigTypes.h"
#include "GlobalData.h"
#include "DB_UpdateGiftCount.h"      // DB_UpdateBreakAwayUvList 权威声明


char* NumberToString(unsigned int value, int radix);   // ORIG T 0x0810904b

#include "BreakAwayPreventImpl/getPrevScheduleTime.inc"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "LogManager.h"
#include "InterDispatcher.h"   // Inter_LoadEtc::giveBreakAwayReward（giveBreakAwayDungeonClearReward）

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <string>


namespace break_away_prevent
{

// ---- getDiffConnectDay（ORIG 0x0831c794）-----------------------------------
// timeSec==0 直接返 0；否则按当日 0 点（getPrevScheduleTime(hour=0)）差折算天，
// 与 curSec 差/0x15180 后取 max(0, days)。asm 的 magic-multiply 为除以 0x15180。
#include "BreakAwayPreventImpl/getDiffConnectDay.inc"
}

// ---- calBreakAwayUserPoint（ORIG 0x0831c810）-------------------------------
// UV 名单 idx∈[0xf,0x1c]：'1'(0x31) 计数恰好 1 且落在 (0x15,0x1c] 高位区即命中。
#include "BreakAwayPreventImpl/calBreakAwayUserPoint.inc"
}

// ---- selectReward（ORIG 0x0831c872）----------------------------------------
// key=m_keys[keyIndex] 查 m_rewardMap；keyIndex∈{0,2,4}（startBreakAway 三分支）。
// point<0 返 0；遍历 list：
//   rewards ← {item.m_field8, m_fieldc, m_field10, point}（每轮重写暂存）
//   if (point < item.m_field0) return false;
//   if (item.m_field0 <= point <= item.m_field4) set + return true;
// 循环耗尽：仍按最后一轮暂存值调用 set 并返回 1（ORIG asm fallthrough 行为）。
#include "BreakAwayPreventImpl/selectReward.inc"
}

// ---- updateUvList（ORIG 0x0831cb56）----------------------------------------
#include "BreakAwayPreventImpl/updateUvList.inc"
}

// ---- startBreakAwayPreventSystem（ORIG 0x0831ce78）-------------------------
#include "BreakAwayPreventImpl/startBreakAwayPreventSystem.inc"
}

} // namespace break_away_prevent

// ---- DB_UpdateBreakAwayUvList::makeRequest（ORIG 0x0843873a；报告缺失，objdump
//      逐指令转写）------------------------------------------------------------
// [b28c-31] AE：Acquire 结果实参内联复现 ORIG 0x0843874x 寄存器直传（70=70）。
#include "BreakAwayPreventImpl/makeRequest.inc"
}

// ============================================================================
// LinkFinal2 核销 —— break_away_prevent::CBreakAwayPreventSystem::GM_UpdateUvList
// （ORIG T @0x0831ca5a，0xfb）objdump 逐指令转写。
//   this+0x30b UV 名单缓冲（0x1f 字节）：先清零，再按 day 选择预设表：
//     day 1/2/3 → 0x8c242c0（31 字节字符串 "0000…111000010001"）
//     day 4     → 0x8c242e0（"0000…111000001000"）
//   然后 DB_UpdateBreakAwayUvList::makeRequest(accId, this+0x30b) +
//   CLocalChina_DB_BreakAwayReset::makeRequest(accId)；恒返 1。
// 同 TU 补齐两个 CLocalChina DB makeRequest（成员 mangled 与基类无关，
// W8LT_DBTask.cpp 的 DBDispatcher 派生类仅提供 C1/D1/vtable）：
//   CLocalChina_DB_BreakAwayReset::makeRequest(uint)    ORIG T 0x08127f42 (0xd7)
//   CLocalChina_DB_LastDay_BreakAway::makeRequest(uint,int,int) ORIG T 0x08128152 (0x10b)
// ============================================================================

class CLocalChina_DB_BreakAwayReset
{
public:
    static void makeRequest(unsigned int accId);
};

class CLocalChina_DB_LastDay_BreakAway
{
public:
    // ORIG 0x08128152 无 this：三参落 ebp+0x08/0x0c/0x10 → 静态成员
    // （静态/非静态成员 mangled 相同：_ZN32...11makeRequestEjii）。
    static void makeRequest(unsigned int accId, int day, int flag);
};

#include "BreakAwayPreventImpl/makeRequest_1.inc"
}

#include "BreakAwayPreventImpl/makeRequest_2.inc"
}

namespace break_away_prevent
{

static const char* const kUvListDay123 = "000000000000000000111000010001";  // 0x8c242c0
static const char* const kUvListDay4   = "000000000000000000111000001000";  // 0x8c242e0

#include "BreakAwayPreventImpl/GM_UpdateUvList.inc"
}
// ---- StubS3A_deps 批 ----
// ORIG T 0x0831d164（报告 break_away_prevent__CBreakAwayPreventSystem/
// giveBreakAwayDungeonClearReward.md 全文转写）：order==1 → selectReward(1,
// accCera, user)；否则 selectReward(3, accCera, user)；selectReward 命中
// （返回非 0）则 Inter_LoadEtc::giveBreakAwayReward(user)。
#include "BreakAwayPreventImpl/giveBreakAwayDungeonClearReward.inc"
}

}  // namespace break_away_prevent
