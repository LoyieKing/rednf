bool CompiledDNFLex::isEnd()
{
    // [b28d-solo-CompiledDNFLexCore 判词·AE] 已修：本 TU 路由 4.4.6 -O2，
    // 原形把共享 `return false` 块放函数尾（4.1.2 放在 `_M_first` 边界块之前）。
    // 关键旋钮 = `__builtin_expect(p == begin._M_cur, 0)` 标记失败路径 unlikely，
    // 4.4.6 即把 L_zero 移到 boundary 块之后，与 ORIG 0x8ad0000 逐条同形
    // （含边界块 `mov -0x4(%ecx),%edx; add $0x1f4,%edx; jmp` 的中间位置）。
    stream_t* p = streamStack_.end()._M_cur;
    if (!__builtin_expect(p == streamStack_.begin()._M_cur, 0))
    {
        stream_t** node = streamStack_.end()._M_node;
        if (p == streamStack_.end()._M_first)
            p = *(node - 1) + STREAM_NODE_ELEMS;
        p = p - 1;
        if (__builtin_expect(p == 0, 0))
            return false;
        return p->buffer + p->size <= offset_;
    }
    return false;
