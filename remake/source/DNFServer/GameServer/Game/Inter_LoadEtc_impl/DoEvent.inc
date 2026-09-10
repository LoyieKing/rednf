void CAradRyosikaEvent::DoEvent(CUser* user, int param2, int param3, int param4)
{
    // [b28c-L18 代落] ORIG 0x081a107c body 尾有 `jmp <统一exit>` 中继岛 ⇒ P223
    // goto 尾空标签形（then 尾跳出）；虚调用 `call *%edx` 差记 N7。
    if (user == 0)
        goto done;
    if (IsEventing(0) && ApplyDatePeriod(user))
    {
        SendMail(user, param2, 1, param4);
    }
done:;
