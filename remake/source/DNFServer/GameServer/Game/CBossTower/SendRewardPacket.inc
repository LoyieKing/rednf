bool CBossTower::SendRewardPacket()
{
    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = m_party->get_user(i);
        // ORIG 0x08144378..0x08144384：短路物化岛无 bool 槽 ⇒ 裸条件（P0/P226）。
        if (member == 0 || m_party->checkValidUser(i) != 1)
            continue;

        PacketGuard packet;
        packet.put_header(0, 0x1e3);
        packet.put_int(m_rewardExp[i]);              // this+(i+4)*4+8 = +0x18
        for (int j = 0; j <= 3; ++j)
        {
            packet.put_byte(m_memberReward[j].m_count);   // +j*0x268+0x28
            for (int k = 0; k < m_memberReward[j].m_count; ++k)
            {
                // P157/P183：ORIG 每字段重算完整 imul 寻址，无具名引用缓存。
                packet.put_int(m_memberReward[j].m_items[k].m_addInfo);
                packet.put_int(m_memberReward[j].m_items[k].get_add_info());
            }
        }
        packet.finalize(true);
        member->Send(packet);
    }
    return 1;
