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
