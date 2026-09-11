// ---- getInt（ORIG 0x8acff70）：DECIMAL/HEXADECIMAL/PAIR_INT 有效。----
int CompiledDNFLex::getInt(bool* result)
{
    int tok = lastToken_;
    unsigned d = (unsigned)tok - 2;
    if (d > 1 && tok != DNF_TOK_PAIR_INT)
    {
        if (result != 0)
            *result = false;
        return 0;
    }
    if (result != 0)
        *result = true;
    // [b28d-G11 直证] ORIG 08acffac 尾段 `mov %eax,&_ZZ17readIntFromBufferRPcE9intBuffer`
    // = helper 内联尾调用 ⇒ 原码就是 `return readIntFromBuffer(offset_);`
    // （旧自写 static intBuffer 既造出发明符号 getInt()::intBuffer, ORIG 无，
    //  又把 buffer 折成栈临时 `mov %eax,0x8` 而非 ORIG 的全局 unique 槽）。
    return readIntFromBuffer(offset_);
}