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
void Divide_C_ClassIP_EndIP(char* out, char* end, char* src)
{
    if (src == 0)
        return;

    char* dot = std::strrchr(src, '.');  // +0x14: call _ZSt7strrchrPci
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