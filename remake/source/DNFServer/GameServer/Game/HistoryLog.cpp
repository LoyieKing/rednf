// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 HistoryLog/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— HistoryLog（G2-4 批次，历史日志文件写入）。
// 依据 docs/class_func_reports/HistoryLog.md + ORIG 反汇编逐函数转录。
// 另含 cHistoryTrace::operator()（ORIG 0x0854f990，权威声明 cHistoryTrace.h）：
// Guard<Mutex>(CLog::instance()+0xa8) 加锁 → CLog::logHistory(fmt, va_list)。
// 静态成员：+0x00 StaticPool<_IO_FILE,50> logfiles_（0x30）| int count_
// | char time[0x10]。格式串字节直接取 ORIG rodata（EUC-KR）。
// ============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "CGameManager.h"
#include "HistoryLog.h"
#include <cstdarg>

#include "CLog.h"
#include "GameTypes.h"       // Guard<Mutex> / Mutex（0x18）
#include "cHistoryTrace.h"

// [L32 GI 调序] ORIG `_GLOBAL__I__ZN10HistoryLog9logfiles_E`（CU=HistoryLog.cpp）
//   ⇒ 本 TU 首个强符号须为 HistoryLog::logfiles_。整块类静态定义前移到所有函数定义
//   之前；logfiles_ 仍是本 TU 唯一动态初始化对象（count_/time 为 POD），
//   相对构造序不变（P327）。
StaticPool<_IO_FILE, 50> HistoryLog::logfiles_;
int HistoryLog::count_;
char HistoryLog::time[0x10];

// ---- 外部符号声明（对应 TU 翻译后移除） ----
#include "HistoryLog/get_str_datetime.inc"
#include "HistoryLog/get_str_date.inc"
#include "HistoryLog/get_str_date_1.inc"
}
#include "HistoryLog/get_str_datetime_1.inc"
}

// ---- 格式串（ORIG rodata 逐字节） ----
static const char FMT_BUY_ITEM[] =
    "\133\045\163\135\276\306\300\314\305\333\261\270\300\324\040\111\104\072\040"
    "\045\144\054\040\103\157\165\156\164\072\045\144\015\012";
static const char FMT_CHAT[] =
    "\133\045\163\135\303\244\306\303\305\270\300\324\072\045\144\040\263\273\277"
    "\353\072\040\045\163\015\012";
static const char FMT_COMPOSITION[] =
    "\133\045\163\135\307\325\274\272\040\267\271\274\255\307\307\072\040\045\144"
    "\015\012";
static const char FMT_SELECT_CH[] =
    "\133\045\163\135\304\263\270\257\274\261\305\303\072\045\163\054\040\301\367"
    "\276\367\072\045\144\054\040\267\271\272\247\072\045\144\054\040\274\272\300"
    "\345\307\374\072\045\144\015\012";
static const char FMT_DUN_START[] =
    "\133\045\163\135\264\370\301\257\040\275\303\300\333\040\112\157\142\072\040"
    "\045\144\054\040\114\145\166\145\154\072\040\045\144\040\274\272\300\345\307"
    "\374\072\040\045\144\015\012";
static const char FMT_FINE_QUEST[] =
    "\133\045\163\135\304\371\275\272\306\256\040\277\317\267\341\072\040\045\144"
    "\015\012";
static const char FMT_GIVE_UP[] =
    "\133\045\163\135\264\370\301\257\306\367\261\342\072\040\045\163\015\012";
static const char FMT_KILL_MOB[] =
    "\133\045\163\135\113\151\154\154\040\115\157\142\124\171\160\145\072\040\045"
    "\144\015\012";
static const char FMT_LEARN_SKILL[] =
    "\133\045\163\135\275\272\305\263\300\324\274\366\040\111\104\072\045\144\054"
    "\040\114\145\166\145\154\072\045\144\015\012";
static const char FMT_LEVEL_UP[] =
    "\133\045\163\135\267\271\272\247\276\367\072\040\045\144\015\012";
static const char FMT_DUN_CLEAR[] =
    "\133\045\163\135\264\370\301\257\040\305\254\270\256\276\356\015\012";
static const char FMT_TRADE_DOWN[] =
    "\133\045\163\135\306\256\267\271\300\314\265\345\040\264\331\277\356\270\256"
    "\275\272\306\256\040\111\104\072\045\144\040\103\157\165\156\164\072\045\144"
    "\015\012";
static const char FMT_TRADE_UP[] =
    "\133\045\163\135\306\256\267\271\300\314\265\345\040\276\367\270\256\275\272"
    "\306\256\040\111\104\072\045\144\040\103\157\165\156\164\072\045\144\015\012";
static const char FMT_USE_COIN[] =
    "\133\045\163\135\304\332\300\316\273\347\277\353\040\055\076\040\045\163\015"
    "\012";
static const char FMT_SELL_ITEM[] =
    "\133\045\163\135\276\306\300\314\305\333\306\307\270\305\040\111\104\072\040"
    "\045\144\054\040\103\157\165\156\164\072\045\144\015\012";
static const char FMT_DIE[] =
    "\133\045\163\135\307\303\267\271\300\314\301\337\040\273\347\270\301\015\012";
static const char FMT_DISJOINT[] =
    "\133\045\163\135\307\330\303\274\040\111\104\072\040\045\144\015\012";
static const char FMT_DELETE_CH[] =
    "\133\045\163\135\304\263\270\257\273\350\301\246\072\045\163\054\040\301\367"
    "\276\367\072\045\144\054\040\267\271\272\247\072\045\144\054\040\274\272\300"
    "\345\307\374\072\040\045\144\015\012";
static const char FMT_CREATE_CH[] =
    "\133\045\163\135\304\263\270\257\273\275\274\272\072\045\163\040\301\367\276"
    "\367\072\045\144\015\012";

// ----------------------------------------------------------------------------
// HistoryLog 类声明见 HistoryLog.h（唯一声明点）
// ----------------------------------------------------------------------------

// [L32 GI 调序] 类静态定义已前移到文件首（须保持 ORIG 的 GI 键名），此处不再重复定义。

#include "HistoryLog/Init.inc"
}

#include "HistoryLog/CheckFD.inc"
}

#include "HistoryLog/LogClose.inc"
}

// [b28d-19] N：N82 —— 帧/槽序（对象临时物化差）；ORIG 0x084b9b32。
#include "HistoryLog/LogOpen.inc"
}

#include "HistoryLog/WriteCreateCh.inc"
}


#include "HistoryLog/WriteDeleteCh.inc"
}

#include "HistoryLog/WriteSelectCh.inc"
}

#include "HistoryLog/WriteLevelUp.inc"
}

#include "HistoryLog/WriteChat.inc"
}

#include "HistoryLog/WriteFineQuest.inc"
}

#include "HistoryLog/WriteLearnSkill.inc"
}

#include "HistoryLog/WriteDunStart.inc"
}

#include "HistoryLog/WriteKillMob.inc"
}

#include "HistoryLog/WriteDie.inc"
}

#include "HistoryLog/WriteGiveUp.inc"
}

#include "HistoryLog/WriteUseCoin.inc"
}

#include "HistoryLog/WriteDunClear.inc"
}

#include "HistoryLog/WriteTradeUp.inc"
}

#include "HistoryLog/WriteTradeDown.inc"
}

#include "HistoryLog/WriteDisJoint.inc"
}

#include "HistoryLog/WriteComposition.inc"
}

#include "HistoryLog/WriteBuyItem.inc"
}

#include "HistoryLog/WriteSellItem.inc"
}

// ============================================================================
// cHistoryTrace::operator()(char const*, ...) —— ORIG 0x0854f990
//   CLog::instance() 后对 +0xa8（m_mutexes[7]，Mutex 0x18）构造 Guard<Mutex>，
//   再调 CLog::logHistory(fmt, va_list)，Guard 析构解锁。
// ============================================================================
#include "HistoryLog/operator.inc"
}
