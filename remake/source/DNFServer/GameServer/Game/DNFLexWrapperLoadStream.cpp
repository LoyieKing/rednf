// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DNFLexWrapperLoadStream/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// DNFLexWrapper 流装载两方法（G4 脚本批次；ORIG 地址与汇编证据见
// docs/class_func_reports/DNFLexWrapper.md）
// ----------------------------------------------------------------------------
// 本 TU 提供：
//   LoadStream()       _ZN13DNFLexWrapper10LoadStreamEPKcS1_PN6DNFLex13stream_data_tE
//                                                                ORIG 0x8ace2b0
//   setFileStreamData() _ZN13DNFLexWrapper17setFileStreamDataEPN6DNFLex13stream_data_tEPKc
//                                                                ORIG 0x8acede0
//
// 依赖符号（由 DNFLexWrapperStream.cpp / DNFLexCore.cpp 提供）：
//   DNFLex::stream_data_t::clear()   _ZN6DNFLex13stream_data_t5clearEv  ORIG 0x8acd140
//   setStringStreamData()            _ZN13DNFLexWrapper19setStringStreamDataEPN6DNFLex13stream_data_tEPcj
//                                                                ORIG 0x8ace9b0
// ============================================================================

#include <fstream>
// ============================================================================
// b28d-52 判词表（本 TU，唯一 DIFF）：
//   DNFLexWrapper::setFileStreamData 0x08acede0 —— 本 TU 走 per-source SRC_OPTS
//     `-O2 -std=gnu++98`（dnf_game_cxx.sh DNFLexWrapperLoadStream 分支 + build.make
//     EXTRA 已由 check_tu_routed.sh 注入）；残差为 -O2 下寄存器分配/帧形与 ORIG
//     （同 -O2 口径）的结构性差异，判 N（跨版本 -O2 调度）。
// ============================================================================

#include "DNFLexWrapper.h"

// LoadStream（ORIG 0x8ace2b0）：若 SetHandler 注册了 LoadStream 回调
// （+0x610c 非空），以 (dir, filename, s) 尾调用之并返回其 bool；
// 否则返回 false。ORIG 汇编：mov 0x610c(%eax),%eax → test/je → jmp *%eax。
#include "DNFLexWrapperLoadStream/LoadStream.inc"
}

// setFileStreamData（ORIG 0x8acede0）：以 in|binary 打开 path 的 ifstream。
//   - path/s 任一为空 → 直接 false；
//   - 先 stream_data_t::clear(s)（stream/size/buffer 清零）；
//   - new ifstream(path, in|binary)；未打开（open 失败置 failbit）或 badbit
//     置位 → delete 流并返回 false；
//   - seekg(0, end) → tellg 取 size → seekg(0, beg)；
//   - size==0 → delete 流并委托 setStringStreamData(s, 0, 0)（空串流）；
//   - 否则 s->stream = ifs; s->size = size; 返回 true。
#include "DNFLexWrapperLoadStream/setFileStreamData.inc"
}
