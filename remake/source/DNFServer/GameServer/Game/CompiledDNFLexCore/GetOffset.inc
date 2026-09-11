// ---- GetOffset（ORIG 0x8acff20）：--finish 后取 buffer；空/指针 0/buffer 0 → -1。----
int CompiledDNFLex::GetOffset() const
{
    // [b28d-solo-CompiledDNFLexCore 判词·AE] 已修：同 isEnd 的块序旋钮。
    // 原 goto-fail 形被 4.4.6 -O2 反转成 `jne have` 且 fail 块前移；
    // `__builtin_expect(p == begin._M_cur, 0)` + `expect(p->buffer == 0, 0)`
    // 使 fail 块回到 ORIG 0x8acff20 的「主路径 → fail → have → boundary」
    // 顺序（含 xchg 对齐填充），逐条同形。
    stream_t* p = streamStack_.end()._M_cur;
    if (__builtin_expect(p == streamStack_.begin()._M_cur, 0))
        goto fail;
    {
        stream_t** node = streamStack_.end()._M_node;
        if (p == streamStack_.end()._M_first)
            p = *(node - 1) + STREAM_NODE_ELEMS;
        p = p - 1;
        if (p != 0)
            goto have;
    }
fail:
    return -1;
have:
    if (__builtin_expect(p->buffer == 0, 0))
        goto fail;
    return (int)(offset_ - p->buffer);
}