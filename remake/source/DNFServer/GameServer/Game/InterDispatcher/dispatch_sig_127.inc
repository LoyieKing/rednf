int Inter_LoadDnfHackLog::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_LoadDnfHackLogPktView* pPkt = reinterpret_cast<Inter_LoadDnfHackLogPktView*>(data);
    // ORIG +0x27：`setne`+`mov $0x0,%eax`+`jmp` ⇒ 不等时早退（== 写法会翻转条件码）。
    if (user->get_acc_id() != pPkt->m_0)
        return 0;
    for (int i = 0x65; i <= 0xfa0; ++i)
    {
        user->getHackAnalyzer()->setServerHackAccumulatedCnt(
            (WongWork::ENUM_HACKTYPE)i,
            pPkt->m_4[i - 0x65]);
    }
    return 0;
}