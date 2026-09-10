void BattleData::IncEventDungeonDestoryObjectPoint(
    int param_1, const CMDPacketStruct::_STReqEventDungeonDestoryObject& st)
{
    if (st.m_roomIndex >= 3)
    {
        return;
    }
    // b28d-40: P183 去 idx 缓存（ORIG 0x085be711/0x085be761 两臂各一次全量重算索引）。
    if (st.m_destroyType == 1)
    {
        m_eventClear[param_1 * 3 + st.m_roomIndex].m_monsterIdx += st.m_point;
    }
    else
    {
        m_eventClear[param_1 * 3 + st.m_roomIndex].m_roomIdx += st.m_point;
    }
}
