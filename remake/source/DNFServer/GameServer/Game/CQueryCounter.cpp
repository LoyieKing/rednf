// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CQueryCounter/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— CQueryCounter（D4 批次）
// 参照 docs/class_func_reports/CQueryCounter.md；源文件 QueryCounter.cpp。
// [G5 2026-09-10 · L12 round2 组5 复核] 5 件全核（判据⑩/⑪ + p167 + 归一化 asm 对比）：
//  · **真缺口修复 1**：IncreaseQueryCount —— p167 显示我方多 1×
//    `_ZNKSt17_Rb_tree_iteratorISt4pairIKiN13CQueryCounter10QUERY_INFOEEEptEv`
//    （const operator->）；`(*m_current).second` 形反劣（换出 +3 deEv 且 ORIG 需 ptEv），
//    改 `m_current->second.count++`（后置自增，P38 反向）后 calls 30→29、
//    **CALLSITE CLEAN**（真缺口清零）。
//  · LoadQueryTable/SaveQuery/RegisterQuery 的 7 处具名 cMyTrace 改全表达式临时链（P227）：
//    LoadQueryTable 实测四数不变（中性，保留）。
//  · 残余 N 族：SaveQuery/LoadQueryTable 的 `memset(buf,0,N)` 常量经 %edx 中转
//    （ORIG 0x085ed3a2/0x085ed18a `mov $N,%edx; mov %edx,0x8(%esp)`）—— 本波探针
//    /tmp/probe_memset.cpp f1..f4（普通/sizeof/unsigned/cast）四形恒直写
//    `movl $N,0x8(%esp)` ⇒ N 封顶（P238 实参变体，无源形）；UpdateQueryCounter 的
//    nop/尾岛；RegisterQuery 槽分区（N95/N82，槽集合两侧全等）；IncreaseQueryCount
//    残余槽差 -0x1c↔-0x18（CStreamGuard 对象区）。
#include "CQueryCounter.h"

// [T2 最后 1 条 · P338] ORIG 本 TU 有 GI `_GLOBAL__I_query_table`
// （t 0x1c @0x085ed735，键名 = query_table），其 static_init @0x085ed6f5 体内含
// `movl $0x947305e,(%esp); call _ZNSt8ios_base4InitC1Ev` + `__cxa_atexit`
// （即 __ioinit @0x947305e）⇒ ORIG 该 TU 含 iostream 的 __ioinit。补本 include
// 以恢复 ORIG 实有符号并使 4.4 发射 GI（缺 __ioinit 时 GI 体塌成 sub $0x8 且无键名）。
#include <iostream>

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "CStreamGuard.h"
#include "GameBase.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"

// ---- ORIG 全局工具函数（属 DB 工具 TU，0x083fbc66）----
#include "CQueryCounter/GetIdentityFromDB.inc"
};

#include "CQueryCounter/CQueryCounter.inc"
}

#include "CQueryCounter/dtor_CQueryCounter.inc"
}

#include "CQueryCounter/Reset.inc"
}

// [b28d-62] 修：P220 极性翻转（`it == end` 直落未命中分支）+ m_bLogDB 正极性。
//   ORIG 0x085ecc5e
#include "CQueryCounter/IncreaseQueryCount.inc"
}

// [b28d-62] 修：迭代器增量改后置 `it++`（P28 增量侧：ORIG 调 operator++(int)）。
//   ORIG 0x085ecee6
#include "CQueryCounter/UpdateQueryCounter.inc"
}

#include "CQueryCounter/SetResponseTime.inc"
}

// [b28d-62] N82：p167 CLEAR。ORIG 0x085ed18a
#include "CQueryCounter/LoadQueryTable.inc"
}

// [b28d-62] N82：p167 CLEAR。ORIG 0x085ed3a2
#include "CQueryCounter/SaveQuery.inc"
}

// [b28d-62] N95/N82：槽基整体差 0x10（4.4 槽基自 ebp-8 起，N95）；p167 CLEAR。
//   ORIG 0x085ed5ae
#include "CQueryCounter/RegisterQuery.inc"
}
