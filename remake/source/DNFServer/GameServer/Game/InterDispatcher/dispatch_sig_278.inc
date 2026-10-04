int Inter_GoldControlMoneyActionCancel::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    SIG_GOLD_CONTROL_EXCEPTION_MID* gold = reinterpret_cast<SIG_GOLD_CONTROL_EXCEPTION_MID*>(data);
    if (user != 0)
    {
        // ORIG 直接 user+0x8e980 成员访问（无 GetGoldControl() 调用）。
        if (gold->m_flag != 0)
            user->m_goldControl.MailSameMid();
        else
            user->m_goldControl.MailComplete(
                gold->m_char);
    }
    return 0;
}