void WideString::clear()
{
    // ORIG 0x08ada0dd：`test %al,%al; jne +0x2b` + `jmp +0x2c` + `nop`
    // ⇒ P223 goto 尾空标签（同 CharString::clear）。
    if (isEmpty())
        goto done;
    replaceData(s_emptyWideStringData);
done:;
}