// ORIG T 0x08142d24：
//   m_prepareFinishFlag(+0xafc)=flag；m_towerEndTick(+0xaf8)=getCurTickCount()；
//   playSec=(m_towerEndTick-m_towerStartTick(+0xaf4))/1000；成员名表 0xff 清零后
//   party->getMemberNames(names)；槽位 0..3 有效成员：
//     未完成加载（m_memberField[i]==0，ORIG this+(i+0x274)*4+8=+0x9d8 区）时
//     hack addServerHackCnt(0x13a,1,0,0)；
//     m_historyLog.DungeonClearInfo(flag, playSec)。
//   循环后 G_TimerQueue()->InsertTimer(OBJ_TYPE 7, m_idx, msg 0xa3, 5s,
//   key 0xa3, 0xa3)。
// [b28d-54 N] ORIG 0x08142d24：槽平移 4 + 成员名表 0xff 清零展开段（N82/N78）。
// [b28d-solo-CBossTower] AE：memberNames `= ""`→`= {0}` 复现 ORIG 单段
//   `lea;mov $0x3f,%ecx;rep stos` + 去具名 invalid（裸短路条件）⇒ DIFF→AE。
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
}