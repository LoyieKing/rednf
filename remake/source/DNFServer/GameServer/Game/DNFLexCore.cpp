// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DNFLexCore/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// DNFLex 基础方法（词法引擎批次；ORIG 地址与汇编证据见
// docs/class_func_reports/DNFLex.md 与 DNFLex__stream_data_t.md）
// ----------------------------------------------------------------------------
// 本 TU 必须以 gnu++98 -O2 编译：ORIG 的 vector::push_back 是 C++98
// finish!=end 路径；gnu++0x 会走 rvalue emplace_back，指令对不上。
//
// DNFLex 本类收口（2026-08-23）：yylex / get_next_token / getStreamName 在同一
// 4.4.6-3 -O2 下为寄存器分配 / NRVO 帧 DIFF，禁止 __asm__ 后无法再压到逐指令
// IDENTICAL；语义已对齐 ORIG。用户视为 identical（checker IDENTICAL_AE）。
// [b28BM13 复核] fresh（-std=gnu++98 -O2 路由）：PushInputStream AE、
// get_next_token DIFF ndiff=12（4.1 callee-saved 栈帧头保存形 + O2 寄存器
// 分配序差，块内条数/助记符集全等，N7/N82 跨版本封顶；P226/局部形试穷举无解）。
// ============================================================================

#include <cstdio>
#include <cstring>
#include <string>
#include <iostream>

#include "DNFLex.h"
#include "SyncScriptDeps.h"

static int dnf_yylex(DNFLex* self)
{
    void** vptr = *reinterpret_cast<void***>(self);
    typedef int (*Fn)(DNFLex*);
    return ((Fn)vptr[0x18 / sizeof(void*)])(self);
}

// ---- DNFLex::stream_data_t::clear()（ORIG 0x8acd140）----
#include "DNFLexCore/clear.inc"
}

// ---- DNFLex::GetInputStream()（ORIG 0x8acd280）----
#include "DNFLexCore/GetInputStream.inc"
}

// ---- DNFLex::getStreamName()（ORIG 0x8acd310，tstring RVO）----
// [b28d-19] N：N7/N82 —— 同上（get_next_token 同族）；ORIG 0x08acd310。
#include "DNFLexCore/getStreamName.inc"
}

// ---- DNFLex::CloseInputStream()（ORIG 0x8acd5b0）----
#include "DNFLexCore/CloseInputStream.inc"
}

// ---- DNFLex::stream_data_t::destroy()（ORIG 0x8acd450）----
#include "DNFLexCore/destroy.inc"
}

// ---- DNFLex::PopInputStream(bool)（ORIG 0x8acd490）----
#include "DNFLexCore/PopInputStream.inc"
}

// ---- DNFLex::LoadStream(...)（ORIG 0x8acd220）----
#include "DNFLexCore/LoadStream.inc"
}

// ---- DNFLex::PushInputStream(...)（ORIG 0x8acd7d0）----
// [b28d-19] AE（真实构建口径 = flags.make + build.make per-source `-O2 -std=gnu++98`；该口径下 178=178 逐条对齐。check_tu_routed.sh 曾漏 per-source 追加 flags 按 -O0 假报 DIFF，Main 已修）；ORIG 0x08acd7d0。
#include "DNFLexCore/PushInputStream.inc"
}

// ---- DNFLex::SwitchInputStream(...)（ORIG 0x8acda50）----
#include "DNFLexCore/SwitchInputStream.inc"
}

// ---- DNFLex::SetHandler(...)（ORIG 0x8acd160）----
#include "DNFLexCore/SetHandler.inc"
}

#include "DNFLexCore/LexerOutput.inc"
}

#include "DNFLexCore/LexerError.inc"
}

#include "DNFLexCore/EnableInclusion.inc"
}

#include "DNFLexCore/DisableInclusion.inc"
}

#include "DNFLexCore/IsStream.inc"
}

#include "DNFLexCore/tok_str.inc"
}

#include "DNFLexCore/tok_str_len.inc"
}

#include "DNFLexCore/getLineNumber.inc"
}

// ---- DNFLex::get_next_token（ORIG 0x8acdaa0）----
// [b28d-19] N：N7/N82 —— 4.1 callee-saved 栈帧头保存形 + 寄存器分配序差，块内条数/助记符集全等；ORIG 0x08acdaa0。
#include "DNFLexCore/get_next_token.inc"
}

#include "DNFLexCore/DNFLex.inc"
}

#include "DNFLexCore/dtor_DNFLex.inc"
}
