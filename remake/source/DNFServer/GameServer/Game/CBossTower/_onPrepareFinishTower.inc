void CBossTower::_onPrepareFinishTower(bool flag)
{
    m_prepareFinishFlag = flag;

    int playSec = 0;
    m_towerEndTick = GlobalData::s_systemTime_.getCurTickCount();
    playSec = (m_towerEndTick - m_towerStartTick) / 1000;

    char memberNames[255] = {0};              // ebp-0x113（ORIG 单段 rep stos）
    m_party->getMemberNames(memberNames);

    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = m_party->get_user(i);
        // ORIG 0x08142e4f..0x08142e5b：短路物化岛无 bool 槽 ⇒ 裸条件（P0/P226）。
        if (member == 0 || m_party->checkValidUser(i) != 1)
            continue;

        if (m_memberField[i] == 0)
        {
            member->getHackAnalyzer()->addServerHackCnt(
                member, (WongWork::ENUM_HACKTYPE)0x13a, 1, 0, 0);
        }

        member->m_historyLog.DungeonClearInfo(flag, playSec);
    }

    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)7, m_idx,
                                (TIMER_MESSAGE)0xa3, 5, 0xa3, 0xa3);
