// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 StatisticsNatType/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— StatisticsNatType（G2-4 批次，统计域）。
// 依据 docs/class_func_reports/StatisticsNatType.md + ORIG 反汇编。
// 布局：+0x00 map<NAT_TYPE_PEER_TO_PEER, RESULT_COUNT*>。
// NAT_TYPE_PEER_TO_PEER(0x8)/RESULT_COUNT(0x8)/SIG_NAT_TYPE(0x14) 见下。
// 类定义统一到权威头 GameWorld.h（StatisticsNatType/嵌套 key/value），
// 本 TU 仅保留方法定义。
// ============================================================================

#include <map>
// ============================================================================
// Lane33 判词表（2026-09-10；本 TU 两件 DIFF 均已闭合，剩余 diff 仅
// _Z41__static_initialization_and_destruction_0ii = N123 结构性豁免）：
//   StatisticsNatType::IncreaseNatType 0x0860c86c —— AE（78/78 指令、size 0xfc 全等）
//     ① 帧 +0x10 差（0x64 vs 0x54）真因 = `insert(std::make_pair(key, rc))`：
//        4.4 额外发 `_ZSt9make_pairIR…` + `pair<const K,V>::C1IS1_S4_EEOS_IT_T0_E`
//        （move 转换 ctor）两调用并多占 pair<K,V> 临时槽 ⇒ 改 value_type 直构
//        `m_counts.insert(value_type(key, rc))` 后复原 ORIG 的
//        `_ZNSt4pairIK…EEC1IRS1_RS4_EEOT_OT0_`（K&/V& 转发 ctor，P192 配套）。
//     ② 尾部计数 ORIG 单加载形 `mov -0x30(%ebp),%eax; mov (%eax),%edx; add $0x1,%edx;
//        mov %edx,(%eax)` ⇒ 源写前缀 `++rc->m_countA;`（`+= 1` 时 4.4 发
//        `mov slot,%eax; mov slot,%edx; mov (%edx),%edx` 双加载 +1 条）。
//   StatisticsNatType::UpdateNatType 0x0860c968 —— AE（161/161、jcc 目标逐位 0 差）
//     ① 迭代器【先声明后赋值】（ORIG 入口 iterator::C1Ev @0x0860c970 + begin 经
//        sret 临时 -0x2c 拷贝进 it；P50/P314）。
//     ② null 检查落 ORIG 空 then 占位岛（0x0860cb43 nop）+ body 尾 jmp 0xcb44 ⇒ 源写
//        负条件 `if (it->second == 0) goto L_inc; { ... } L_inc:;`（P223；标签置复合块
//        外以合法跨声明），4.4 即产 `test; sete %al; test %al,%al; jne <岛>`。
//     ③ it->second 每处重读（ORIG 条件 3 次 + 写侧 4 次 ptEv，无 rc 缓存槽；P183）。
//     ④ 循环尾后置 `it++`（ORIG ppEi + sret 临时 -0x14；P28）。
//   SIG_NAT_TYPE 写侧已由 TU 局部视图 cast（SIG_NAT_TYPE_VIEW）改为 SigTypes_E.h
//   权威字段直写；ORIG 写序/偏移表：
//     +0x00 m_natType      ← it->first.m_a          0x0860caa8
//     +0x04 m_natTypeOther ← it->first.m_b          0x0860caaf
//     +0x08 m_failCnt      ← it->second->m_countB   0x0860cac3（unsigned 异型 ⇒ eax→edx 中转）
//     +0x0c m_successCnt   ← it->second->m_countA   0x0860cadc（同上）
//     +0x10 m_time         ← now                    0x0860caf4（time_t 同型 ⇒ LHS-first 直存）
// ============================================================================
#include <string.h>
#include <time.h>

#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include "GameWorld.h"
#include "SigTypes_E.h"

// SIG_NAT_TYPE：权威定义已字段化于 SigTypes_E.h（原 char m_pad[0x14] 占位移除；
// ORIG GetInBuffer 符号 _ZN11CStreamGuard11GetInBufferI12SIG_NAT_TYPEEPT_v @0x08610f6e
// 使用该权威类型）。字段语义见 UpdateNatType 判词（对齐/偏移 static_assert 在头内）。

// ORIG 08610262：参数对按升序存储（m_a <= m_b）
#include "StatisticsNatType/NAT_TYPE_PEER_TO_PEER.inc"
}

// ORIG 08610294：两个计数清零
#include "StatisticsNatType/RESULT_COUNT.inc"
}

#include "StatisticsNatType/StatisticsNatType.inc"
}

#include "StatisticsNatType/dtor_StatisticsNatType.inc"
}

#include "StatisticsNatType/Reset.inc"
}

#include "StatisticsNatType/IncreaseNatType.inc"
}

#include "StatisticsNatType/UpdateNatType.inc"
}
