// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CodeHackChecksum/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ===========================================================================
// CodeHackChecksum.cpp —— CodeHack 校验域 ORIG 忠实实现（df_game_r 反汇编逐指令转写）
//
// 符号地址证据见 CodeHackChecksum.h 文件头。外部依赖：
//   sha256_starts/update/finish、StrCpyMaxPath → 权威 producer 见
//   ShaSupport.h/.cpp（W7AB 组新建；sha256_process 为其内部压缩函数）。
// ===========================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__Z18ZeroMemoryChecksumP16CodeHackChecksum`（ORIG 0x808ac67）。
#include <iostream>


#include "CodeHackChecksum.h"

#include <cstring>

#include "SyncScriptDeps.h"   // toTString(const char*)

// W7_BuildHarmonize：struct/extern 由权威头 ShaSupport.h 取代（原 TU 局部
// 定义移出，保证唯一 owner 与全局命名空间 mangled 匹配）。
#include "ShaSupport.h"

static_assert(sizeof(sha256_context) == 0x68, "sha256_context layout drift");

// ---------------------------------------------------------------------------
// ZeroMemoryChecksum(CodeHackChecksum*)  T @0x0808a49c
//   starts(ctx); finish(ctx, ck->m_data);
// ---------------------------------------------------------------------------
#include "CodeHackChecksum/ZeroMemoryChecksum.inc"
}

// ---------------------------------------------------------------------------
// CalculateMemoryChecksum(CodeHackChecksum*, unsigned long, void*) T @0x0808a4c4
//   starts(ctx); update(ctx, data, size); finish(ctx, ck->m_data);
// ---------------------------------------------------------------------------
#include "CodeHackChecksum/CalculateMemoryChecksum.inc"
}

// ---------------------------------------------------------------------------
// CalculateFileChecksum(CodeHackChecksum*, FILE*, ulong, ulong, void*) T @0x0808a505
//   total==0 → ZeroMemoryChecksum；否则按 chunkSize 循环 fread（每次 1 元素），
//   fread 失败即 finish 收尾；成功则 update 并累计，读满 total 后 finish。
// 第 4 参为每轮读取块大小（ORIG 分支：chunk = min(chunkSize, total - done)）。
// ---------------------------------------------------------------------------
#include "CodeHackChecksum/CalculateFileChecksum.inc"
}

// ===========================================================================
// class CodeHackChecksum —— POD；~CodeHackChecksum W @0x0808b7da 空体。
// ===========================================================================
// [b28d-Main 代落·S329 P299/P317] 用户 ctor/dtor 定义删除（声明端同步）：
// ORIG 无 C1/C2 符号，D1/D2 由使用点（new_allocator::destroy）发射 W。

// ===========================================================================
// class CodeHackChecksumData
// ===========================================================================
#include "CodeHackChecksum/CodeHackChecksumData.inc"
}
#include "CodeHackChecksum/SetCodeHackCheckRange.inc"
}

#include "CodeHackChecksum/dtor_CodeHackChecksumData.inc"
}

#include "CodeHackChecksum/CodeHackChecksumData_1.inc"
}

#include "CodeHackChecksum/Clear.inc"
}

#include "CodeHackChecksum/Reserve.inc"
}

#include "CodeHackChecksum/AddCodeHackChecksum.inc"
}

// [b28d-solo-CodeHackChecksum AE] ORIG 0x0808a662：显式 `(const char*)` 实参形
// 闭合 memcmp 装载序（见函数体内 P268 cast 旋钮注释）；仅余 call 重定位伪差。
#include "CodeHackChecksum/IsRightChecksum.inc"
}

// ===========================================================================
// class CodeHackChecksumEx
// ===========================================================================
#include "CodeHackChecksum/CodeHackChecksumEx.inc"
}

#include "CodeHackChecksum/dtor_CodeHackChecksumEx.inc"
}                                                         // 随后隐式基类/vector 析构

// [b28d-lane16 N84②] 拷贝构造不定义：ORIG 0x08575f10 为 W 隐式合成（基类 C1ERKS
// + m_fileName rep movsl 0x41 + m_range 双 int 直拷）；显式体恒 call memcpy（24 vs 32）。
// 隐式发射由本 TU PopData 的 `CodeHackChecksumEx tmp(m_queue.front())` 保证（ODR 使用点）。

#include "CodeHackChecksum/SetFileName.inc"
}

#include "CodeHackChecksum/GetFileName.inc"
}


#include "CodeHackChecksum/GetCodeHackCheckRange.inc"
}

// setData(char*) T @0x0808a802。
// 序列化布局：<int8 len> <len 字节文件名> <8 字节 range> <int8 count>
//             <count × 32 字节 CodeHackCheckpoint>……（长度字节按 ORIG 以
//             movzbl/movsbl 有符号读取转写）。
// [b28d-solo-CodeHackChecksum N-P75 封顶] ORIG 0x0808a802：唯一残差 = Add 调用点
// 的 this/实参装载序（ORIG this-first，4.4 实参-first，见循环内注释），八形探针
// 均不可翻 ⇒ 跨版本求值序，语义等价。
#include "CodeHackChecksum/setData.inc"
}

// [b28d-solo-CodeHackChecksum AE] ORIG 0x0808a8c8：与 Data 版同形，显式
// `(const char*)` 实参形闭合（见函数体内注释）；仅余 call 重定位伪差。
#include "CodeHackChecksum/IsRightChecksum_1.inc"
}

// ===========================================================================
// class CodeHackCheckRangeEx
// ===========================================================================
#include "CodeHackChecksum/CodeHackCheckRangeEx.inc"
}

#include "CodeHackChecksum/SetFileName_1.inc"
}

#include "CodeHackChecksum/GetFileName_1.inc"
}

#include "CodeHackChecksum/setData_1.inc"
}

// ===========================================================================
// class CodeHackCheckStorage
// ===========================================================================
#include "CodeHackChecksum/CodeHackCheckStorage.inc"
}

#include "CodeHackChecksum/dtor_CodeHackCheckStorage.inc"
}

#include "CodeHackChecksum/IsEmpty.inc"
}

#include "CodeHackChecksum/IsFull.inc"
}

#include "CodeHackChecksum/PushData.inc"
}

#include "CodeHackChecksum/PopData.inc"
}

#include "CodeHackChecksum/PopData_1.inc"
}

#include "CodeHackChecksum/IsRightChecksum_2.inc"
}

#include "CodeHackChecksum/reset.inc"
}
