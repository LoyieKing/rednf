int WarRoom::Join(CUser* user, int& seat)
{
    int count = GetWaiterCount();
    PacketGuard packet1;
    PacketGuard packet2;
    packet1.put_header(0, 2);
    packet2.put_header(0, 0xb);
    packet1.put_byte(1);
    packet1.put_short(count);
    packet2.put_byte(count);
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
        {
            (m_slotsUser[i])->make_basic_info((char*)&packet1, 1);
            packet2.put_short((m_slotsUser[i])->get_unique_id());
            packet2.put_int((m_slotsUser[i])->m_network.get_inner_ip());
            packet2.put_int((m_slotsUser[i])->m_network.get_ip());
            packet2.put_short((m_slotsUser[i])->m_network.get_port());
            packet2.put_int((m_slotsUser[i])->get_acc_id());
            packet2.put_byte((m_slotsUser[i])->m_network.get_nat_type());
            packet2.put_int((m_slotsUser[i])->m_network.get_mtu());
            // ORIG 0x86bf2xx：setne+je ⇒ `!= 0` 真路径 fall-through 到 stamina。
            if ((m_slotsUser[i])->getCurCharacR() != 0)
            {
                packet2.put_byte((m_slotsUser[i])->getCurCharacR()->m_stamina);
            }
            else
            {
                packet2.put_byte(0);
            }
        }
    }
    packet1.finalize(true);
    packet2.finalize(true);
    int slot = -1;
    for (int i = 0; i <= 5; ++i)
    {
        if ((m_slots[i] == 0) && (m_slotState[i] == 0xff))
        {
            slot = i;
            m_slots[i] = user;
            user->SetWarRoomIndex((short)m_index);
            seat = i;
            break;
        }
    }
    if (slot == -1)
    {
        return 4;
    }
    user->Send(packet1);
    user->Send(packet2);
    if (m_pDungeon != 0)
    {
        user->m_historyLog.EnterDungeon(m_pDungeon->GetDungeonName(),
                                        (unsigned char)m_levelBand);
    }
    // ORIG 0x86bf3xx（EnterDungeon 后 call 之后）：`add $0x50` 槽写 0 = m_slotState[slot]=0，
    // 位于 m_liveFlag 置 1 之前；原重建缺失此具名赋值。
    m_slotState[slot] = 0;
    m_liveFlag[slot] = 1;
    packet1.clear();
    packet1.put_header(0, 2);
    packet1.put_byte(1);
    packet1.put_short(1);
    user->make_basic_info((char*)&packet1, 1);
    packet1.finalize(true);
    SendToRoom(packet1);
    packet2.clear();
    packet2.put_header(0, 0xb);
    packet2.put_byte(1);
    packet2.put_short(user->get_unique_id());
    packet2.put_int(user->m_network.get_inner_ip());
    packet2.put_int(user->m_network.get_ip());
    packet2.put_short(user->m_network.get_port());
    packet2.put_int(user->get_acc_id());
    packet2.put_byte(user->m_network.get_nat_type());
    packet2.put_int(user->m_network.get_mtu());
    // ORIG 同型 setne ⇒ != 0。
    if (user->getCurCharacR() != 0)
    {
        packet2.put_byte(user->getCurCharacR()->m_stamina);
    }
    else
    {
        packet2.put_byte(0);
    }
    packet2.finalize(true);
    SendToRoom(packet2);
    packet1.clear();
    packet1.put_header(1, 0x5d);
    packet1.put_byte(1);
    packet1.put_int(user->getCurCharacMoney());
    packet1.put_short(GlobalData::s_systemTime_.getCurSec() - m_fpBase);
    packet1.finalize(true);
    user->Send(packet1);
    return 0;
}