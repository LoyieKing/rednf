// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 UsedFatigueRewardItem/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// UsedFatigueRewardItem::Data —— ORIG W C1 @0x081a40a0 / D1 @0x081a6bf4
// （objdump 逐指令转写；无独立 md 报告，证据见头注释与
//  docs/class_func_reports/UsedFatigueRewardItem/{parsing,find}.md）
// C1：movw $0,+0x00 / movw $0,+0x02 / movl $0,+0x114 →
//     memset(+0x4,0,0xff) → memset(+0x103,0,0x10)（清零宽度实证）。
// D1：push ebp; mov esp,ebp; pop ebp; ret —— 用户定义空析构（ORIG 有 D1/D2 符号）。
#include "UsedFatigueRewardItem.h"

#include <cstring>
#include <string>

#include "Arad_Script.h"
#include "RDARScriptStringManager.h"   // g_scriptStringManager_ / findString

// [b28d-64 判词汇总 · 泳道64] 本 TU 1 件：parsing 0x081a25d2
//   214/226 N封顶（P167 EXTRA UsedFatigueRewardItem::Data::~Data ×2，4.4 对局部
//   Data 数组/临时多发析构 + EH 段；MISSING 空 ⇒ 语义完成、byte 层封顶）。

// ============================================================================
// OnlyOrigBatch11 落地：find / parsing（报告 parsing.md / find.md 全文转写）
// ============================================================================

// ORIG T 0x081a25d2：Load(path,"on_off") 失败返 0xa1；GetRowNum()==0 返 0xa4；
// GetString(0,"flag")=="true" → m_enable=1；Load(path,"item_reward") 失败返
// 0xac、GetRowNum()==0 返 0xaf；逐行 GetInt("fatigue"/"item_count"/"item_id")
// + GetString("message"/"title") 填 Data 后 push_back；成功返 0。
#include "UsedFatigueRewardItem/parsing.inc"
}

// ORIG T 0x081a28e4：遍历 m_data，命中 m_begin<=low 且 high<m_end（开区间）
// 的项：复制 m_begin/m_id/m_end；m_name 经 findString(4,...) 解析后
// strncpy 0x14；m_str 经 findString(4,...) 解析后 strncpy 0xff；返 1。
// 未命中返 0。
#include "UsedFatigueRewardItem/find.inc"
}

#include "UsedFatigueRewardItem/Data.inc"
}

#include "UsedFatigueRewardItem/dtor_Data.inc"
}
