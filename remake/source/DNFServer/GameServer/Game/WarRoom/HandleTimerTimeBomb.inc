void WarRoom::HandleTimerTimeBomb()
{
    m_field.m_mapInfo.m_monsterMap.clear();
    m_flag10 = 1;
    if (m_field.m_stageIdx == m_pDungeon->m_warRoomData.m_50)
        ChangeState(WARROOM_STATE_5);
    else
        ChangeState(WARROOM_STATE_4);
}