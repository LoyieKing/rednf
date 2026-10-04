int Inter_LoadDnfHackLog::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    SIG_LOAD_DNF_HACK_LOG* log = reinterpret_cast<SIG_LOAD_DNF_HACK_LOG*>(data);
    // ORIG +0x27：`setne`+`mov $0x0,%eax`+`jmp` ⇒ 不等时早退（== 写法会翻转条件码）。
    if (user->get_acc_id() != log->m_mId)
        return 0;
    for (int i = 0x65; i <= 0xfa0; ++i)
    {
        user->getHackAnalyzer()->setServerHackAccumulatedCnt(
            (WongWork::ENUM_HACKTYPE)i,
            log->m_cnt[i - 0x65]);
    }
    return 0;
}