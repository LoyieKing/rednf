// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CAdvanceAltarLog/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// CAdvanceAltarLog —— advancealtar 统计日志实现（Wave5 AltarProc 续批）。
// 证据：docs/class_func_reports/CAdvanceAltarLog/ 全文（10 函数）。
// 全部字段化；不使用 GameStubs 桥。
// ============================================================================
// [b28d-E03 2026-09-10 本波收口] 四数 68/102/0/5 → **68/107/0/2**：
//   · incrementEntranceData DIFF→AE：if(it==end){装配}else{累加} 块序翻转
//     （ORIG 0x08156c95 `je +0xc1`：未命中块 fall-through 在前）。
//   · updateDatabase DIFF→AE（**258=258**）：P259 删 raw/msgq 具名槽 + P132
//     `(*stream)->operator<<` + P328⑩ cMyTrace 单语句临时链 + P13 begin/end 缓存
//     + P96 非 const iterator + P0 size 具名 int + 自拷贝守卫 memcpy 0xe。
//   · incrementLayerStatData 111 vs 112 → N18a/N7 封顶（c5root 4.1.2 探针：具名/
//     const/register 三形亦必 spill；试形读数 113/115/112/113，保留 112 最优）。
//   · getIndex 16 vs 12 → N18a 封顶（9 形探针 12~21 条）。
//   · 真 bug 修正：装配写 m_field2(+0x2) → **m_itemLevel(+0x8)**（ORIG 0x08156e1d
//     写 pair+0x8）；删 SigTypes_E.h 显式 m_padE ⇒ W 模板实例
//     `_ZNSt4pairIKx27STAdvanceAltarLayerStatDataEC1IRS0_S1_EEOT_OT0_` 复现 ORIG
//     0x081583e2 的 3×mov + 尾 `movzwl 0xc(%eax); mov %ax,0x14(%edx)`（25=25 归零）。
#include "CAdvanceAltarLog.h"

#include <cstring>

#include "CStreamGuard.h"
#include "GlobalData.h"      // GlobalData::s_stream_pool / s_msgq_mgr
#include "MsgQueueMgr.h"
#include "StreamPool.h"

// ORIG makeKey @0x08157298：低 32 = itemIndex，高 32 = (itemType<<16)|keyC
// （ORIG 汇编实证：组合 (itemType<<16)|keyC 后 sar $0x1f 符号扩展为 64 位；
// 高 32 位即该组合的有符号扩展——与低 32 位 itemIndex 拼合）。
#include "CAdvanceAltarLog/makeKey.inc"
}

// ORIG getType @0x081572ea：返回 key 高 32 位的高 16 位（= key>>48，符号扩展位型扩展）。
#include "CAdvanceAltarLog/getType.inc"
}

// ORIG getIndex @0x0815730e：返回 (long long)(int)key（低 32 位符号扩展；
// mov $0,%eax 后 mov %edx,%eax 残留，sar $0x1f 符号扩展为 64 位）。
// [b28d-lane6] DIFF·N18a：ORIG 0x0815730e 的 (int)key→long long 截断-扩链残留 4 条死序（mov -0x4,%edx; mov %eax,%edx; mov $0,%eax; mov %edx,%eax），4.4 恒折（b28c-27 复核）。
#include "CAdvanceAltarLog/getIndex.inc"
}

// ORIG getLevel @0x08157336：shld+shl 组合 (key<<16) 后取高 32 位 sar 16
// —— 即 (int)((key<<16)>>48) = 高 32 位（(itemType<<16)|keyC）的低 16 位（keyC）。
#include "CAdvanceAltarLog/getLevel.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x08156aaa（ctor）：Mutex() → map(+0x18)/map(+0x30) 构造 → initStatistics。
// ----------------------------------------------------------------------------
#include "CAdvanceAltarLog/CAdvanceAltarLog.inc"
}

// ORIG T 0x08156b38（dtor）：ORIG 汇编开头即调 initStatistics（Guard 清双 map），
// 然后成员自动逆序析构。
#include "CAdvanceAltarLog/dtor_CAdvanceAltarLog.inc"
}

// ORIG T 0x08156bca（initStatistics.md）：Guard 内双 map clear。
#include "CAdvanceAltarLog/initStatistics.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x08156c30（incrementEntranceData.md）：guard 内 find(stageIndex)；
// 命中累加 enter/success/star 三计数；未命中经栈上 16B 构造后 operator[] 写入
// {stageIndex, deltas}（data[0] 冗余存键，与 ORIG 二进制布局一致）。
// ----------------------------------------------------------------------------
// [b28d-lane6] DIFF·N93/N82：槽号/寄存器分配（99=99 长度同，逐条语义等价；N82 帧族）。
#include "CAdvanceAltarLog/incrementEntranceData.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x08156d60（incrementLayerStatData.md）：guard 内 makeKey 后 find；
// 命中累加 useCount/useStar；未命中 14B 结构装配后 memcpy 运算式写入新槽
// （新建分支计数亦含增量，与 hit 分支累计一致）。
// ----------------------------------------------------------------------------
// [b28d-lane6] DIFF·N93/N82：槽号/寄存器分配（111 vs 123，语义闭合；N82 帧族）。
#include "CAdvanceAltarLog/incrementLayerStatData.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x08156ed2（updateDatabase.md 全文）：guard 内 Acquire 流（msg 0x33b，
// 广播 -1）；put entrances.size() + 逐条 GetInBuffer<ST> memcpy 0x10 并 trace；
// clear；同序 layers：size + memcpy 0x10（pair+8 起，超读 2 字节 pad 为 ORIG
// 原行为）/trace/clear；尾 MsgQueueMgr put 队列 4。
// ----------------------------------------------------------------------------
// [b28d-lane6] DIFF·N93/N82：槽号/寄存器分配（258 vs 266，语义闭合）。
#include "CAdvanceAltarLog/updateDatabase.inc"
}
