int BattleData::ClearEventDungeonRoom(
    int param_1, int param_2, const CMDPacketStruct::_STReqEventDungeonClearRoom& st)
{
    // b28d-40: 真语义 bug 修正——ORIG 0x085be82b `cmp 0x10(%ebp),%eax; jl fail`
    // ⇒ 成功条件是 st.m_monsterIndex >= param_2（原码写成 <= 反向）。
    // 同时按 P183 去 idx 缓存（ORIG 三次独立全量重算索引）、P133 拆独立守卫。
    if (st.m_roomIndex > 2)
        return 0;
    if (m_eventClear[param_1 * 3 + st.m_roomIndex].m_monsterIdx == st.m_monsterIndex)
    {
        if (m_eventClear[param_1 * 3 + st.m_roomIndex].m_roomIdx == st.m_monsterCount)
        {
            if (st.m_monsterIndex >= param_2)
            {
                m_eventClear[param_1 * 3 + st.m_roomIndex].m_clearFlag = 1;
                return 1;
            }
        }
    }
    return 0;
