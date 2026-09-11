void CAssaultPlace::MakeViewAssaultIconOnePlayer(PacketGuard* packet, bool flag)
{
    // ORIG T 0x82e89c0：put_byte 直传 flag 零扩展（0x82e89xx `movzbl -0x1c,%edx`
    // 无三元物化）；`cmpb $0; je 尾` ⇒ 正向 if(flag) 块（尾 call 自然落 leave，
    // 无 return 岛）；countIdx/count 双 dead init（-0x14/-0x10）后 get_index 覆写。
    packet->put_int(m_field104);
    packet->put_byte((unsigned char)flag);
    if (flag)
    {
        packet->put_short((int)m_posBase.m_x);
        packet->put_short((int)m_posBase.m_y);
        int countIdx = 0;
        int count = 0;
        countIdx = packet->get_index();
        packet->put_short(count);
        for (int i = 0; i < 8; ++i) {
            if (m_assaulters[i].Empty())
                continue;
            packet->put_short((int)m_assaulters[i].GetUserUniqueId());
            packet->put_byte(GetDirectionByTeam(m_assaulters[i].GetTeam()));
            count += 1;
        }
        packet->put_short(countIdx, count);
    }
}