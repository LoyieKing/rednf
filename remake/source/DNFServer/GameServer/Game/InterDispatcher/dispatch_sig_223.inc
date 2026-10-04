int Inter_Hack_CheckMyAccount::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    SIG_CHECK_MY_ACCOUNT_OK* ok = reinterpret_cast<SIG_CHECK_MY_ACCOUNT_OK*>(data);
    if (ok->m_a == 0)
    {
        // ORIG +0x3c 一步 `mov %eax,%ebx`（call 结果直落 callee-saved）＝register 变量形态；
        // const 普通局部会先落栈再取（多两行）。
        register int etc = user->SetETC(0x2c5, ok->m_id);
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x2c5, 1, (unsigned int)etc, 0);
    }
    return 0;
}