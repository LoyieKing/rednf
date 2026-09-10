void CBattle_Field::onClearMap(bool isClear)
{
    MapInfo* mi = GetCurrentMapInfo();
    mi->m_playInfo.m_clearTick = GlobalData::s_systemTime_.getCurTickCount();

    if (isClear)
    {
        if (m_dungeonIndex == 0x29)
        {
            // 限时阈值：单人 120000ms；多人 0x1d4c1（getPlayTick<=limit 判定）
            unsigned int limitTick = 120000;
            if (m_party->get_member_count() > 1)   // ORIG *(CParty**)this == m_party(+0x00)
                limitTick = 0x1d4c1;

            if (m_party->checkBossRoom())
            {
                if (mi->m_playInfo.getPlayTick() <= limitTick)
                    m_party->enumPartyMember(addUserHackCount149);
            }
            if (m_party->checkBossRoom())
            {
                if (mi->m_playInfo.m_useItemType2Count == 0)
                {
                    if (mi->m_playInfo.m_useItemOtherCount == 0)
                        m_party->enumPartyMember(addUserHackCount150);
                }
            }
        }
    }
