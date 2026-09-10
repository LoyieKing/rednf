// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 GmMacro/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// GmMacro.cpp — game_master::CMacro / stMacroData 完整还原（第2批）。
// 对照 docs/class_func_reports/game_master__CMacro/ 10 函数报告 + stMacroData ctor/dtor。
#include "GameMasterCmd.h"
#include "CUser.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include <string.h>

namespace game_master
{

#include "GmMacro/Reset.inc"
}

// ---- stMacroData ----
#include "GmMacro/stMacroData.inc"
}

// b28d-solo-GmMacro：stMacroData 无用户析构（隐式 trivial）。
// ORIG nm 0x084b5db4 W 0x5 = 编译器隐式 trivial D1；ORIG Recording 0x084a8802 正常
// 路径无 dtor call、vector 清理走 _Destroy_aux<true>（trivial 分支）。我方旧用户空
// dtor 使 4.4 判 non-trivial：正常路径多 1 次 dtor call + push_back 处 EH landing
// （~stMacroData ×2 + _Unwind_Resume + push esi/ebx），Recording 53 vs ORIG 35。
// m_data 释放仍由 CMacro::Reset 循环负责（浅拷贝 push_back，析构释放会 double-free）。

// ---- CMacro ----

#include "GmMacro/StartMacro.inc"
}

#include "GmMacro/StopMacro.inc"
}

#include "GmMacro/RudMacro.inc"
}

#include "GmMacro/NextCmd.inc"
}

#include "GmMacro/GetCurCmd.inc"
}

// b28d-solo-GmMacro verdict=AE ORIG 0x084a8802 orig/ours=35/36（1 条尾 nop，
// §6.1 对齐填充豁免），p167 CLEAR 4/4。修法：删除 stMacroData 用户空析构
// （GameMasterCmd.h 声明 + 本文件定义）——ORIG nm 0x084b5db4 W 0x5 系编译器隐式
// trivial D1，ORIG 正常路径无 dtor call、vector 清理走 _Destroy_aux<true>；我方旧
// 用户 dtor 使 4.4 判 non-trivial，正常路径多 1 次 dtor call 且 push_back 处布
// EH landing（~stMacroData ×2 + _Unwind_Resume + push esi/ebx），53 vs 35。
// 旧 b28 判词 N-EH/N83（P216 throw() 法不可用）作废：非平凡性源自我方用户 dtor，
// 删声明即复现 ORIG（P12/P71 判据；dtor 符号由 vector 使用点隐式发射，nm 仍在）。
#include "GmMacro/Recording.inc"
}

#include "GmMacro/OnRecv.inc"
}

#include "GmMacro/OnSend.inc"
}

// b28d-solo-GmMacro verdict=AE ORIG 0x084a8924 orig/ours=97/97，p167 CLEAR 18/18。
// 修法：P222 正向包裹 + P171 外层已判后的死重测照抄 + 三支 Acquire 内联直传；
// 本轮 p167 命中 1 处重载选错：operator<<(GetUID()) 原码选 operator<<(int)
// （ORIG +0x104 call 0x080c8c56；不 cast 会选 0x080e2292 unsigned 版），
// 实参处 static_cast<int> 后调用点差集清零（线格式同为 4 字节，位模式不变）。
#include "GmMacro/SendPacket.inc"
}

} // namespace game_master
