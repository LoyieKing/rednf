void CharString::assign(const CharString& src)
{
    // ORIG 0x08ad6fed：`cmp %eax,%ebx; sete %al; test %al,%al; jne +0x44(nop 岛)`
    // ⇒ goto 尾空标签形（P223）。while+break 空体被 4.4 折掉 jne（实测 -3 条）。
    if (getData() == src.getData())
        goto done;
    replaceData(src.getData());
done:;
