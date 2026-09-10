bool CUser::CheckLoadHotKeyCashedCharacType(int cashedCharacJob, int characJob)
{
    if (cashedCharacJob == -1)
        return true;

    // P0/N96 去绑定 + P226 裸语句表达式升格：`({ callee(); })` 单表达式形令 4.4
    // 将首结果升格 %ebx 跨次 call 保活并出 `mov %eax,%ebx; cmp %eax,%ebx;
    // sete %al; test; je` 物化形（probe p7.t7 实证 push ebx+sub 0x14 帧同 ORIG）。
    if (({ GetCharacHotKeyType(cashedCharacJob); }) == GetCharacHotKeyType(characJob))
        return false;
    return true;
