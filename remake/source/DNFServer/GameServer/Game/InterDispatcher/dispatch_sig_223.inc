int Inter_Hack_CheckMyAccount::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_Hack_CheckMyAccountPktView* pPkt = reinterpret_cast<Inter_Hack_CheckMyAccountPktView*>(data);
    if (pPkt->m_0 == 0)
    {
        // ORIG +0x3c 一步 `mov %eax,%ebx`（call 结果直落 callee-saved）＝register 变量形态；
        // const 普通局部会先落栈再取（多两行）。
        register int etc = user->SetETC(0x2c5, pPkt->m_8);
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x2c5, 1, (unsigned int)etc, 0);
    }
    return 0;
