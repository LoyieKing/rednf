void StatisticsPvP::incPvPRoomResult(int keyA, int keyB, int value)
{
    Guard<Mutex> guard(&m_mutex);

    PvP_Key key;
    key.m_a = keyA;
    key.m_b = keyB;

    std::map<PvP_Key, PvP_Room>::iterator it = m_map.find(key);
    if (it == m_map.end())
    {
        // ORIG 0x860c653：PvP_Room 四字段 {keyA, keyB, 1, value}。
        PvP_Room room;
        room.m_key = key;
        room.m_field8 = 1;
        room.m_fieldC = value;
        m_map.insert(std::pair<const PvP_Key, PvP_Room>(key, room));
    }
    else
    {
        // ORIG 0x860c6a6：单次 operator*（deEv）后取 &second 入具名槽 -0xc。
        PvP_Room* pRoom = &(*it).second;
        pRoom->m_field8 = pRoom->m_field8 + 1;
        pRoom->m_fieldC = pRoom->m_fieldC + value;
    }
