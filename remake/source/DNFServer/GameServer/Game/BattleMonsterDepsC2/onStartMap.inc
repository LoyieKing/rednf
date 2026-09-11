// ORIG 0x0830dc74：当前图 m_playInfo.m_startTick(+0x7c) ← 系统 tick。
void CBattle_Field::onStartMap()
{
    GetCurrentMapInfo()->m_playInfo.m_startTick =
        GlobalData::s_systemTime_.getCurTickCount();
}