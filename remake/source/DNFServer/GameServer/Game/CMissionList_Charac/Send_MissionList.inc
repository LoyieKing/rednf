    // N7 封顶：唯一残差=虚调用 callee 寄存器互换（N7，探针 /tmp/l22p/vprobe*.cpp）。
int CMissionList_Charac::Send_MissionList(CUser& user)
{
    PacketGuard packet;
    packet.put_header(0, 0x140);
    int index = packet.get_index();
    packet.put_byte(0);
    int count = 0;
    for (int kind = 1; kind < 0x24; ++kind)
    {
        if (m_missionList[kind].m_index != 0)
        {
            packet.put_short((int)m_missionList[kind].m_index);
            if (kind == 0x1b)
            {
                short combo = 0;
                for (int i = 0; i < 5; ++i)
                {
                    if (((int)m_missionList[kind].m_count >> i) & 1)
                    {
                        ++combo;
                    }
                }
                packet.put_short((int)combo);
            }
            else
            {
                packet.put_short((int)m_missionList[kind].m_count);
            }
            CMission* mission = (CMission*)G_CDataManager()->find_mission(
                (int)m_missionList[kind].m_index);
            if (!mission)
            {
                m_missionList[kind].clear();
            }
            else
            {
                if ((kind == 0x1b) || (kind == 0x1f) || (kind == 0x20))
                {
                    packet.put_short((int)m_missionList[kind].m_streak);
                }
                else
                {
                    packet.put_short(mission->getClearCondition());
                }
                ++count;
            }
        }
    }
    packet.put_byte(index, count);
    unsigned char buf[0x20];
    BitsetToStr(m_clearedFlags, buf);
    for (int i = 0; i < 0x20; ++i)
    {
        packet.put_byte((int)buf[i]);
    }
    packet.finalize(true);
    user.Send(packet);
    return 1;
}