// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 BestClearTime/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r BestClearTime 实现（2026-08-19）。
// 逐函数对照 docs/class_func_reports/BestClearTime.md 与 ORIG objdump：
//   ctor 0x80c851c / dtor 0x80c8596 / _reset 0x80c8758 /
//   setBestClearTime 0x80c8928 / getBestClearTime 0x80c8ac0 /
//   setNewRecord 0x80c8b44 / checkNewRecord 0x80c8b5c。
// loadData/_saveData/getData 已由本 TU 实现（StubS3A 批，ORIG 报告转写）。
//   loadData 0x80c8776 / _saveData 0x80c8616 / getData 0x80c883c。

#include "GlobalData.h"      // GlobalData::{s_big_stream_pool,s_msgq_mgr}
#include "CStreamGuard.h"
#include "CUser.h"           // CUser::{GetUID,get_charac_no}
#include "MsgQueueMgr.h"
#include "BestClearTime.h"

// ============================================================================
// 构造 / 析构 / 重置
// ============================================================================

#include "BestClearTime/BestClearTime.inc"


#include "BestClearTime/dtor_BestClearTime.inc"


#include "BestClearTime/_reset.inc"


// ============================================================================
// 最佳通关时间读写
// ============================================================================

#include "BestClearTime/setBestClearTime.inc"


#include "BestClearTime/getBestClearTime.inc"


// ============================================================================
// 新纪录标志
// ============================================================================

#include "BestClearTime/setNewRecord.inc"
{
    // ORIG 0x80c8b44：写 this+0x20。
    m_bNewRecord = bNew;
}

#include "BestClearTime/checkNewRecord.inc"
{
    // ORIG 0x80c8b5c：读 this+0x20。
    return m_bNewRecord;
}

// ============================================================================
// loadData / _saveData / getData（StubS3A 批）
// SIG_BEST_CLEAR_TIME（0x5dc8）：characNo@+0 / count@+4 /
//   entries[1000]@+8，每条 0x18 = key(4) + SIG_BEST_CLEAR_TIME_DIFFICULTY(0x14)。
// ============================================================================
struct SIG_BEST_CLEAR_TIME_ENTRY
{
    int m_key;                              // +0x00（dungeon idx）
    SIG_BEST_CLEAR_TIME_DIFFICULTY m_data;  // +0x04（5 难度时间）
};

// 线格式整体视图：characNo@+0 / count@+4 / entries[1000]@+8（每条 0x18）。
// 用结构体数组下标访问（ORIG 的 i*0x18 强度归约 + 成员 disp 折叠）。
struct SIG_BEST_CLEAR_TIME_VIEW
{
    int m_characNo;                                 // +0x00
    int m_count;                                    // +0x04
    SIG_BEST_CLEAR_TIME_ENTRY m_entries[1000];      // +0x08
};


#include "BestClearTime/loadData.inc"


#include "BestClearTime/getData.inc"


#include "BestClearTime/_saveData.inc"

