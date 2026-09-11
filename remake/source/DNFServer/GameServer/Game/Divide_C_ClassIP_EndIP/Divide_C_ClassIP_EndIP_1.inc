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
void Divide_C_ClassIP_EndIP(char* out, char* end, char* src, char* err)
{
    if (src == 0)
    {
        *err = 1;
        return;
    }

    char* dot = std::strrchr(src, '.');
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

    // ORIG 块序：[out判定 je→l5][strncpy][end判定 jne→l2; jmp→l6][l5:*err5]
    // [l2:strncpy2][l6:*err6]。复现形 = 显式双 goto + 冷标签源序 l5→l2→l6。
    if (out == 0)
        goto l5;
    strncpy(out, src, len);
    // [P222] ORIG 0x086b1f21 为 `jne l2; jmp l6` 双跳；两形互试后取
    // `if(end==0) goto l6;`（正极性写法产 `je l6; jmp l2` 反极性，勿用）。
    if (end == 0)
    {
        goto l6;
    }
    goto l2;
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