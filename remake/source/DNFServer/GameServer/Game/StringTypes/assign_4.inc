void WideString::assign(const WideString& src)
{
    // ORIG 0x08ad9feb：`cmp; sete %al; test %al,%al; jne +0x44(nop 岛)`
    // ⇒ goto 尾空标签形（P223，同 CharString::assign）。
    if (getData() == src.getData())
        goto done;
    replaceData(src.getData());
done:;
