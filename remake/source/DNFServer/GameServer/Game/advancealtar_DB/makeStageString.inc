void DB_Update::makeStageString(std::string& out, const _Stage* arr, int count)   // [objdump 0x813f380]
{
    if (arr == 0)
        return;
    out.clear();
    char buf[128];
    for (int i = 0; i < count; ++i)
    {
        // [b28d-26] ORIG 0x0813f3f8 m_field4 两段中转（字段改 enum，P254）；
        //   体尾 `jmp +0xa6` + `nop@0xa5` 用 P223 双源 goto 尾标签串复现。
        if (arr[i].m_field0 == 0 && arr[i].m_field4 == 0)
            goto L_skip;
        std::sprintf(buf, "%d%s%d%s", arr[i].m_field0, kDelimterField,
                     arr[i].m_field4, kDelimterItem);   // ORIG 经 DAT ","/"|"
        out += buf;
        goto L_inc;
L_skip:;
L_inc:;
    }
