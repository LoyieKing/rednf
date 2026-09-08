// Divide_C_ClassIP_EndIP 实现（W7 链接收敛）。
// 忠实转写 objdump ORIG T @0x086b1dd5 / @0x086b1e85（无独立 md 报告，
// 反汇编见 /tmp/df_batches/W7_linkfix_W7E.md 证据节）。

#include "Divide_C_ClassIP_EndIP.h"

#include <cstring>

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
void Divide_C_ClassIP_EndIP(char* out, char* end, char* src)
{
    if (src == 0)
        return;

    char* dot = strrchr(src, '.');      // +0x14: call _ZSt7strrchrPci
    if (dot == 0)
        return;

    unsigned int len = dot - src;                   // -0x10(%ebp)
    if (len > 12)                                   // cmpl $0xc,-0x10; ja
        return;

    unsigned int endLen = strlen(src + len + 1);    // -0xc(%ebp)
    if (endLen > 4)                                 // cmpl $0x4,-0xc; ja
        return;

    if (out != 0)
        strncpy(out, src, len);
    if (end != 0)
        strncpy(end, src + len + 1, endLen);
    return;
}

// ============================================================================
// ORIG @ 0x086b1e85 [0x086b1e85, 0x086b1f59]：同上拆分逻辑，各失败分支
// 在 *err 写入状态码后返回（src空=1 / 无'.'=2 / 前缀>12=3 / 末段>4=4 /
// out空=5 / end空=6）；完整成功（两输出非空）不写 err。
// ============================================================================
void Divide_C_ClassIP_EndIP(char* out, char* end, char* src, char* err)
{
    if (src == 0)
    {
        *err = 1;
        return;
    }

    char* dot = strrchr(src, '.');
    if (dot == 0)
    {
        *err = 2;
        return;
    }

    unsigned int len = dot - src;
    if (len > 12)
    {
        *err = 3;
        return;
    }

    unsigned int endLen = strlen(src + len + 1);
    if (endLen > 4)
    {
        *err = 4;
        return;
    }

    // [b28 P223·v5 探针 /tmp/l2_p9.cpp] ORIG 0x086b1e85 块序
    // [out判定 je→l5][strncpy][end判定 jne→l2; jmp→l6][l5:*err5][l2:strncpy2][l6:*err6]
    // 复现形 = 显式双 goto + 冷标签源序 l5→l2→l6；if/else 嵌套形产 `je 尾岛` 反极性
    // + `jmp` 中继缺失（ndiff 23→~8）。两 nop 岛（je/jne 目标块前对齐垫）与 ORIG
    // 直落为链接布局伪差（b22 判词族）。语义不变。
    if (out == 0)
        goto l5;
    strncpy(out, src, len);
    // [b28 N 判词] ORIG 0x086b1f21 end 守卫为 `jne COPY2; jmp ERR6` 双跳形；4.4
    // CFG 对 `je→ERR6 岛` 化简恒产反向单跳（空块垫 P223 亦被 fold，实测无效），
    // 冷块物理位置由 GCC BB 序决定不可源码控制。残余 1 条助记符（je↔jne）+
    // 2 nop 岛差，三分支可观察行为逐条相等。禁 asm 桥（P0）。
    if (end != 0)
    {
        goto l2;
    }
    goto l6;
l5:
    *err = 5;
    return;
l2:
    strncpy(end, src + len + 1, endLen);
    return;
l6:
    *err = 6;
    return;                                          // 成功路径不写 err
}
