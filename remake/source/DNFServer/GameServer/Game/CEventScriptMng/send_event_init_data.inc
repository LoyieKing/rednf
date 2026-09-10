void CEventScriptMng::send_event_init_data(CUser* user, bool flag)
{
    if (user == 0)
        return;

    PacketGuard packet;
    packet.put_header(0, 0x176);
    packet.put_int(m_todayTime);
    int countIdx = packet.get_index();
    packet.put_byte(0);
    unsigned char count = 0;
    // ORIG 0x0810c487：end() 在循环初始化区求值一次（P13），it 槽 -0x30 /
    // endIt 槽 -0x34；每轮重调 end() 会多占 sret 临时槽并顶偏 PacketGuard 槽。
    for (std::map<int, CEventEntity*>::iterator it = m_entities.begin(),
             endIt = m_entities.end();
         it != endIt; ++it)
    {
        if (is_eventing(it->first))
        {
            if (it->second->send_init_data(user, packet,
                                           static_cast<short>(it->first),
                                           m_todayTimeDay, flag))
                ++count;
        }
    }
    packet.put_byte(countIdx, count);
    packet.finalize(true);
    user->Send(packet);
