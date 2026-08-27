// Divide_C_ClassIP_EndIP 实现（W7 链接收敛）。
// 忠实转写 objdump ORIG T @0x086b1dd5 / @0x086b1e85（无独立 md 报告，
// 反汇编见 /tmp/df_batches/W7_linkfix_W7E.md 证据节）。

#include "Divide_C_ClassIP_EndIP.h"

#include <cstring>

// ============================================================================
// ORIG @ 0x086b1dd5 [0x086b1dd5, 0x086b1e84]
//   cmp src,0 → 静默返回；dot=strrchr(src,'.') → NULL 返回；
//   len=dot-src，(unsigned)len>12 → 返回；endLen=strlen(dot+1)，
//   (unsigned)endLen>4 → 返回；out/end 非空分别 strncpy。
// ============================================================================
void Divide_C_ClassIP_EndIP(char* out, char* end, char* src)
{
    if (src == 0)
        return;

    char* dot = strrchr(src, '.');      // +0x14: call _ZSt7strrchrPci
    if (dot == 0)
        return;

    int len = static_cast<int>(dot - src);          // -0x10(%ebp)
    if (static_cast<unsigned int>(len) > 12)        // cmpl $0xc,ja
        return;

    int endLen = strlen(dot + 1);                   // -0xc(%ebp)
    if (static_cast<unsigned int>(endLen) > 4)      // cmpl $0x4,ja
        return;

    if (out != 0)
        strncpy(out, src, len);
    if (end != 0)
        strncpy(end, dot + 1, endLen);
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

    int len = static_cast<int>(dot - src);
    if (static_cast<unsigned int>(len) > 12)
    {
        *err = 3;
        return;
    }

    int endLen = strlen(dot + 1);
    if (static_cast<unsigned int>(endLen) > 4)
    {
        *err = 4;
        return;
    }

    if (out == 0)
    {
        *err = 5;                                    // jne f31 → je f29: movb $5
        return;
    }
    strncpy(out, src, len);

    if (end == 0)
    {
        *err = 6;                                    // jne f31 → else jmp f52: movb $6
        return;
    }
    strncpy(end, dot + 1, endLen);
    return;                                          // 成功路径不写 err
}
