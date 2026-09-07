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

    // ORIG 正向条件（`cmpl $0,0x8(%ebp); je +0xa4` 前跳到冷块 *err=5，
    // fall-through 才是 strncpy）⇒ 原码是 if(out){...}else *err=5，非守卫式早退。
    // [b22判词·N]（本函数非 idx67 分派件，同文件同根因一并修）残余 1 insn：
    //   ORIG 块序为 [out判定][strncpy(out)][end判定][*err=5][strncpy(end)][*err=6]，
    //   我方为 [...][end判定][strncpy(end)][*err=6][*err=5]——三个错误分支的
    //   可观察行为逐条相等（out==0→*err=5 且不动 end；end==0→strncpy(out) 后
    //   *err=6；两者非空→双 strncpy 且不写 err），仅 GCC 基本块放置序不同
    //   （多一条 jmp、冷块位置不同），源码层无稳定控制手段。
    if (out != 0)
    {
        strncpy(out, src, len);

        if (end != 0)
            strncpy(end, src + len + 1, endLen);
        else
            *err = 6;
    }
    else
    {
        *err = 5;
    }
    return;                                          // 成功路径不写 err
}
