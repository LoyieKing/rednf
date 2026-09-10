int Dispatcher_BossDieCheck::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stBossDieCheckParam& out = static_cast<stBossDieCheckParam&>(param);
    out.m_4 = check_error(user, msg, param);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x259e,
                        __PRETTY_FUNCTION__,
                        out.m_4, user->get_acc_id());
    }

    // [round5 A01] P0/P183：ORIG 仅 -0x24(out 引用)/-0x20(msg 指针) 两槽，无 req/bossSeq/
    //   unkKind 引用别名（各占指针槽 ⇒ 帧 0x4c→0x6c）；msg 指针在**首用点**才声明。
    char* pReq = reinterpret_cast<char*>(&msg);            // -0x20
    if (user->getCurCharacSkillCheckSum() !=
        *reinterpret_cast<int*>(pReq + 0x14))              // +0x14 int
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x25c, 1, 0, 0);
    }

    CParty* party =
        G_CGameManager()->GetParty((int)(short)user->GetPartyIndex());
    // party+0xc3c = CBattle_Field 起始（m_battleField 偏移 0）；每次重发 `lea 0xc3c`（P183）。
    std::vector<int>::iterator it = std::find(party->m_battleField.m_apcList.begin(),
                                              party->m_battleField.m_apcList.end(),
                                              *reinterpret_cast<unsigned short*>(pReq + 0x10));
    if (it != party->m_battleField.m_apcList.end())
    {
        party->m_battleField.m_apcList.erase(it);
        out.m_a = (short)*reinterpret_cast<unsigned short*>(pReq + 0x10);
    }
    else
    {
        out.m_4 = 0x15;
    }

    if ((int)party->m_battleField.m_apcList.size() == 0)   // party+0xc48 = BF+0x124
    {
        party->m_battleField.m_apcAllKilledSec = GlobalData::s_systemTime_.getCurSec();
        int characNo = user->get_charac_no(-1);
        G_TimerQueue()->InsertTimerInMilisecond(
            (TimerEntry::OBJ_TYPE)1, party->GetPartyIndex(),
            (TIMER_MESSAGE)0x64, (long long)1000,
            party->m_battleField.m_apcAllKilledSec, (unsigned int)characNo);
        out.m_8 = 1;
    }

    if (*reinterpret_cast<unsigned short*>(pReq + 0xe) == 0xffff)   // movzwl 直证
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0xcb, 1, 0, 0);
    }
    return 0;
