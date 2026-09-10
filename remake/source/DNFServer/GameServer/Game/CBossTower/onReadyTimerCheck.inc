int CBossTower::onReadyTimerCheck()
{
    if (checkAllMemberReady(m_party->get_member_count()))
        return 0;

    if (m_party == 0 || m_party->get_member_count() < 1)
    {
        _destroy();
        return 0;
    }

    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = m_party->get_user(i);
        // ORIG 0x081451eb..0x081451f7：短路物化岛无 bool 槽 ⇒ 裸条件（P0/P226）。
        if (member == 0 || m_party->checkValidUser(i) != 1)
            continue;
        if (m_memberReady[i] != 1)
        {
            onChargeRequireItem(member);
            handleLeaveUser(member);
        }
    }

    if (m_party->get_member_count() > 0)
    {
        PacketGuard packet;
        packet.put_header(0, 0x1e);
        packet.put_int(0);
        packet.finalize(true);
        m_party->send_to_party(packet);
    }
    else
    {
        _destroy();
    }
    return 1;
