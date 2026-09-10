// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 BitTools/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 全局位工具 + SameBuffer（ORIG 自由函数，objdump 直转）。
// ----------------------------------------------------------------------------
// ORIG 符号（df_game_r，全部 T 强符号，同一工具区域 0x088921c2-0x0889253a）：
//   setBit_Byte(unsigned char&, bool, int)   _Z11setBit_ByteRhbi  0x088921c2
//   isSetBit_Byte(unsigned char, int)        _Z13isSetBit_Bytehi  0x0889221f
//   setBit_Word(unsigned short&, bool, int)  _Z11setBit_WordRtbi  0x0889224a
//   isSetBit_Word(unsigned short, int)       _Z13isSetBit_Wordti  0x088922a9
//   SameBuffer(unsigned char*, unsigned long, unsigned char*)
//                                           _Z10SameBufferPhmS_   0x088924f1
// isSetBit_Uint / setBit_Uint（0x0889232e / 0x088922d7）已由 BingoEventLink4.cpp
// 定义，本 TU 不再重复。
// ----------------------------------------------------------------------------
// 反汇编形态：
//   setBit_Byte：bool 形参 spill（cmpb $0）后两分支各自 movzbl 读 → shl → or/not+and
//                → movb 写；isSetBit_*：mov $1 → shl %cl → and → test → setne；
//   SameBuffer：i 无符号（movl $0 + setb 循环界），不等即返回 0，等完返回 1。
// ============================================================================

#include "BitTools/setBit_Byte.inc"
}

#include "BitTools/isSetBit_Byte.inc"
}

#include "BitTools/setBit_Word.inc"
}

#include "BitTools/isSetBit_Word.inc"
}

#include "BitTools/SameBuffer.inc"
}
