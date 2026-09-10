// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Divide_C_ClassIP_EndIP/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// Divide_C_ClassIP_EndIP 实现（W7 链接收敛）。
// 忠实转写 objdump ORIG T @0x086b1dd5 / @0x086b1e85（无独立 md 报告，
// 反汇编见 /tmp/df_batches/W7_linkfix_W7E.md 证据节）。

#include "Divide_C_ClassIP_EndIP.h"

#include <string.h>

// [b28d-solo-Divide_C_ClassIP_EndIP · P306 族/跨编译环境] ORIG 的 libstdc++ 在 gcc 4.1
// 环境（glibc string.h 未定义 __CORRECT_ISO_CPP_STRING_H_PROTO）以 inline 形式发射
// std::strrchr(char*, int)（ORIG nm：W 0x08265244，体 = 转发 strrchr@plt，14 条指令）；
// 本仓 4.4 头因 __GNUC_PREREQ(4,4) 走 glibc extern "C++" 重载（asm 名 strrchr），
// std::strrchr 恒解析为 C 符号 ⇒ 调用点与 ORIG 差一个 _ZSt7strrchrPci。
// 按 ORIG 同一 inline 体在此声明（非发明符号：符号/体/弱绑定均见 ORIG 二进制）。
namespace std {
inline char* strrchr(char* s, int c)
{
    return __builtin_strrchr(s, c);
}
}

// ============================================================================
// ORIG @ 0x086b1dd5 [0x086b1dd5, 0x086b1e84]
//   cmp src,0 → 静默返回；dot=strrchr(src,'.') → NULL 返回；
//   len=dot-src（unsigned，>12 即返回，ORIG 直接 `cmpl $0xc,-0x10(%ebp); ja`）；
//   endLen=strlen(**src+len+1**)（>4 即返回，`cmpl $0x4,-0xc(%ebp); ja`）；
//   out/end 非空分别 strncpy。
//   P0 实证：ORIG 两处末段起点都不是 dot 指针槽(-0x14)，而是
//   `mov -0x10(%ebp),%eax; add $0x1,%eax; add 0x10(%ebp),%eax` 由 len 重算
//   ⇒ 原码持有的是长度局部量、末段地址按 src+len+1 表达式现算（非 dot+1）。
// ============================================================================
// [b28d-solo-Divide_C_ClassIP_EndIP 判词 · AE] 清单 NEAR(1) → fresh TU compare
// `ae`（60=60 逐条；清单 NEAR 系 .o 重定位标签伪差）。P167 调用点差集 CLEAR
// （std::strrchr wrapper 归位见文件头注释）。语义/控制流/栈槽与 ORIG 0x086b1dd5 全等。
#include "Divide_C_ClassIP_EndIP/Divide_C_ClassIP_EndIP.inc"
}

// ============================================================================
// ORIG @ 0x086b1e85 [0x086b1e85, 0x086b1f59]：同上拆分逻辑，各失败分支
// 在 *err 写入状态码后返回（src空=1 / 无'.'=2 / 前缀>12=3 / 末段>4=4 /
// out空=5 / end空=6）；完整成功（两输出非空）不写 err。
// [b28d-solo-Divide_C_ClassIP_EndIP 判词 · AE] 清单 DIFF(9) → fresh TU compare
// `ae`（69=69 逐条）。两处修法：
//   ① P222 极性翻转：`if(end!=0) goto l2; goto l6;` 在 4.4 恒反演成
//      `je l6; jmp l2`（旧判词误封 N223）；改写 `if(end==0) goto l6; goto l2;`
//      即复现 ORIG `cmpl $0xc(%ebp); jne l2; jmp l6` 的极性与 l5→l2→l6 块序。
//   ② 文件头 std::strrchr wrapper 复现 ORIG `call _ZSt7strrchrPci`（p167 归零）。
// 残余 2 条跳转目标前对齐 nop 由 compare 口径剥离（§6.1/P223 判词族），非语义差。
// ============================================================================
#include "Divide_C_ClassIP_EndIP/Divide_C_ClassIP_EndIP_1.inc"
}
