int Dispatcher_Mercenary_Competition::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081ddd00 (0xd4 insn)
    MsgMercenaryCompetition& req = reinterpret_cast<MsgMercenaryCompetition&>(msg);
    SendMercenaryCompetitionResult& result = static_cast<SendMercenaryCompetitionResult&>(param);
    if (user == 0)
        return -1;
    if (user->get_state() != 3)
        return -1;

    unsigned int targetNo = *reinterpret_cast<unsigned int*>(req.m_pad + 3);   // msg+0x10
    const char b0x15 = static_cast<char>(req.m_pad[8]);                        // msg+0x15
    const char b0x14 = static_cast<char>(req.m_pad[7]);                        // msg+0x14

    if (user->GetParty() != 0 && user->getCurCharacNo() == static_cast<int>(targetNo))
    {
        *reinterpret_cast<char*>(reinterpret_cast<char*>(&result) + 0x04) = 0; // param+4 = 0
        return -1;
    }

    char* pResult = reinterpret_cast<char*>(&result);
    // ORIG: Competition_Mercenary_Charac(user, msg+0x10, (char)msg+0x15, (char)msg+0x14)
    const int ret = user->Competition_Mercenary_Charac(
        targetNo, b0x15, b0x14);
    pResult[0x04] = static_cast<char>(ret & 0xff);   // ORIG mov %al,0x4(%edx)
    *reinterpret_cast<int*>(pResult + 0x08) = static_cast<int>(targetNo);
    pResult[0x0c] = b0x15;
    pResult[0x0d] = b0x14;
    // ORIG: cUserHistoryLog::MercenaryCompetition(user->m_historyLog, targetNo, b0x15, b0x14)
    user->m_historyLog.MercenaryCompetition(targetNo, b0x15, b0x14);
    return 0;
