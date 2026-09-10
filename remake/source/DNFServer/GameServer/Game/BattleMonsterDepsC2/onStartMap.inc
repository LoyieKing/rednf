void CBattle_Field::onStartMap()
{
    GetCurrentMapInfo()->m_playInfo.m_startTick =
        GlobalData::s_systemTime_.getCurTickCount();
