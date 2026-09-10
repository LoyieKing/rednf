// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 RestrictNameDeps/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// RestrictNameDeps.cpp —— restrict_inputting_name 域 + checkValidCharacName 真实定义。
// 逐符号证据：
//   - importUnicodeRangeData：W _ZN23restrict_inputting_name25RestrictInputtingNameData
//     22importUnicodeRangeDataEPKcPSt6vectorISt4pairIiiESaIS5_EE @0x08ac2eca sz 0x29e
//     （报告 global_constructors_keyed_to_…/unicodeRangeDataList_.md 全文已读）。
//   - C1 @0x08ac3168、isInRange @0x08ac31a0、IsNoneUnicodeRange op() @0x08ac321c、
//     IsNoneUnicodeRange C1 @0x08ac3240、any_of/find_if/none_of 实例族
//     （W @0x08ac3253/0x08ac3278/0x08ac32a0）、isWideStrInUnicodeRange 报告全文已读。
//   - isUtf8StrInUnicodeRange 报告全文已读（WideString + convertFromUtf8 链）。
//   - checkValidCharacName @0x0820582b（报告缺失，objdump 全文转写）。

#include "RestrictNameDeps.h"
#include "StringTypes.h"          // WideString
#include "StringResource.h"       // convertFromUtf8
#include "DNFLexWrapper.h"        // loadRDARScriptFile/ScanType/ScanInt 权威声明
#include "LogManager.h"
#include <string.h>               // strlen
#include <wchar.h>                // wcslen
#include <algorithm>              // std::any_of（ORIG W _ZSt6any_ofIPKw…EEbT_S4_T0_ @0x08ac3253）
#include "CDataManager.h"        // G_CDataManager()/hasSpecialCharacter 族

namespace restrict_inputting_name
{

std::vector<std::pair<int, int> > RestrictInputtingNameData::unicodeRangeDataList_;
bool RestrictInputtingNameData::isLoaded_ = false;

// ORIG W 0x08ac2eca sz 0x29e 忠实转写（成员版；结构与 throw/catch/logFormat
// 路径全保留）：外层 for(;;)：seek 循环丢 token 直至 "[allow unicode range]"；
// body 循环里 "[/allow unicode range]" → out=tmp 并重回 seek；"[range]" →
// ScanInt 左右值入 tmp；其它 token 忽略。任一 ScanType 失败/越界抛 const char*，
// 被 catch 记日志后返 false。
// [b28d-D9] 修复：P222 正向包裹（token=="[range]"）+ logFormat 5 参（去 "%s"）后 AE 172/172（ORIG 0x08ac2eca sz 0x0000029e）
// [b28d-solo-RestrictNameDeps] verdict=AE（compare_tu clean 口径 172/172 逐条相等）。
//   清单旧值 NEAR/ndiff13 与 game_func_report 的 DIFF 均为 P264 假报（本件无
//   class_func_reports 报告 ⇒ md_text=='' 分支不做 AE 归一）：.o 内 .rodata 串/
//   _ZTIPKc 立即数尚未重定位显示为 0，且 4.4 在 ranges 构造前多一条循环对齐 nop
//   （strip_nop_padding 剥离后两侧等长）。ORIG sz 0x29e / 我方 0x29f 同因。
#include "RestrictNameDeps/importUnicodeRangeData.inc"
}

#include "RestrictNameDeps/RestrictInputtingNameData.inc"
}

// ORIG W 0x08ac31a0：for i<size: if (first<=ch && ch<=second) return true；
// ORIG 用 vector::operator[](i) 两次取 first/second——保持成员访问形态。
#include "RestrictNameDeps/isInRange.inc"
}

#include "RestrictNameDeps/operator.inc"
}

// ORIG T 0x08ac2de4（isWideStrInUnicodeRange.md 全文）：
//   IsNoneUnicodeRange 构造（call 0x08ac3240）→ wcslen（call 0x0807e130）结果落
//   局部槽 -0x10（P0：具名 size_t 局部）→ any_of(str, str+len, pred)（call
//   0x08ac3253）→ 取反返回。any_of 实例 ORIG W _ZSt6any_ofIPKw
//   N23restrict_inputting_name18IsNoneUnicodeRangeEEbT_S4_T0_ @0x08ac3253
//   （c6root libstdc++ 4.4.7 bits/stl_algo.h:778 有该模板，可实例化）。
//
// [b28d-solo-RestrictNameDeps] verdict=AE（compare_tu clean 27/27 逐条相等；
// 旧 [b22判词·N-EMPTYCLS-SRA]「结构不可复现」判词作废）。
//   根因=4.4 对**空类**对象做标量替换：pred 虽在 -0x11 构造，但传参用寄存器
//   `mov %bl,0x8(%esp)`（prologue `push %ebx` + `sub $0x24`），ORIG 4.1 从栈槽读
//   `movzbl -0x11(%ebp),%edx; mov %dl,0x8(%esp)`（`sub $0x28` / `leave`）。
//   修法=给 IsNoneUnicodeRange 加 1 字节成员（RestrictNameDeps.h，已 R2 广播）：
//   sizeof 仍 1（空类 sizeof==1）、基类布局/全部偏移/mangled/符号集不变、C1 体不变；
//   探针 /tmp/probe_sra2.cpp 实证空类→%bl、带 1 字节成员→movzbl -N(%ebp)。
//   加成员后 4.4 逐条复现 ORIG（sub $0x28 / 无 push ebx / movzbl -0x11(%ebp),%edx
//   / leave），同 TU any_of/find_if/none_of/__find_if 四实例一并 DIFF→AE。
#include "RestrictNameDeps/isWideStrInUnicodeRange.inc"
}

// ORIG T 0x08ac2d6c sz 0x78（isUtf8StrInUnicodeRange.md 全文）：
// WideString 构造 → convertFromUtf8(str,(int)strlen,str 的宽度) → c_str 判定 → 析构返回。
#include "RestrictNameDeps/isUtf8StrInUnicodeRange.inc"
}

}  // namespace restrict_inputting_name

// ORIG T 0x0820582b _Z20checkValidCharacNamePKc sz 0x6c（objdump 全文转写）：
// G_CDataManager()->hasSpecialCharacter / hasPreventString / hasPreventStringName 三连。
#include "RestrictNameDeps/checkValidCharacName.inc"
}

// ORIG 函数无正常退出路径（外层 for(;;) 永不离开；正常返回仅经 catch→return false）。
// 此行不可达，仅为抑制编译器 non-void 返回警告。
