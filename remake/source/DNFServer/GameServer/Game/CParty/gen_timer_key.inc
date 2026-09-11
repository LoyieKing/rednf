int CParty::gen_timer_key(TIMER_MESSAGE msg)
{
    // ORIG: sub $0xb,%eax; lea 0x50(%eax),%ecx; mov 0x8(%edx,%ecx,4)…（同上分解）
    // b28d-49 @0x85be4xx：`return ++a[i]` 单表达式形（探针 f5 逐条全等）——
    // 值经 %ebx 跨 store 保活、store 用 lea 0x50、reload 复用 add 0x50；
    // 两句式（++ 后 return a[i]）4.4 重算下标且用 %ecx（ndiff 14）。
    return ++m_field148[(int)msg - 0xb];
}