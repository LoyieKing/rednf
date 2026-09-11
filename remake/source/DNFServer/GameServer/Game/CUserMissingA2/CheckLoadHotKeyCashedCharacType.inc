// ----------------------------------------------------------------------------
// 2. ORIG T 0x08692e9c sz 0x55。
// cashedCharacJob==-1 → 1（首次加载放行）；否则两次调 GetCharacHotKeyType 比较，
// 相同→0、不同→1（类型变化才允许重新加载热键）。
// [P210] 本函数定义前置到 callee GetCharacHotKeyType 定义之前：4.4 对 TU 内
// 已见定义的 callee 精确继承 outgoing（帧塌缩 0x14→0x8），前置后恢复 ORIG
// 保守帧 sub $0x14（b28 P210 派生附注同法）。
// ----------------------------------------------------------------------------
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
}