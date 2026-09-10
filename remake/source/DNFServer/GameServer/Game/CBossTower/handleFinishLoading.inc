int CBossTower::handleFinishLoading(CUser* user)
{
    const int slot = static_cast<int>(m_party->GetMemberSlotNo(user));
    // ORIG 0x08143f86..0x08143fb5：短路物化岛无 bool 槽（mov$1/jmp/mov$0 +
    // test %al,%al;je）⇒ 裸条件内联（P0/P226）。
    if (slot != -1 && m_party->checkValidUser(slot) != 0)
    {
        setMemberReady(slot, true);
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x28);
        if (checkAllMemberReady(m_party->get_member_count()))
        {
            PacketGuard packet;
            packet.put_header(0, 0x1e);
            packet.put_int(0);
            packet.finalize(true);
            m_party->send_to_party(packet);
        }
        return 1;
    }
    return 0;
