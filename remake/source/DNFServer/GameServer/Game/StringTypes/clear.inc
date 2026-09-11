void CharString::clear()
{
    // ORIG 0x08ad707f：`test %al,%al; jne +0x2b` + 体尾 `jmp +0x2c` + `nop`
    // ⇒ P223 goto 尾空标签复现 4.1 空 then 占位岛。
    if (isEmpty())
        goto done;
    replaceData(s_emptyCharStringData);
done:;
}