bool CBossTower::SendELPLPacket()
{
    bool ok = true;                            // ebp-0x11
    if (m_party->m_battleField.m_bloodState != 1)
    {
        for (int i = 0; i <= 3; ++i)
        {
            CUser* member = m_party->get_user(i);
            // ORIG 0x0814420e..0x08144221：短路物化岛无 bool 槽
            // （mov$1/jmp/mov$0 + test %al,%al;jne 循环尾）⇒ 裸条件内联（P0/P226）。
            if (member == 0 || m_party->checkValidUser(i) != 1)
                continue;

            unsigned char requireFlag = 0;       // ebp-0x21
            if (!checkStartGameCondition(requireFlag, false))
            {
                ok = false;
                break;
            }
        }
    }

    PacketGuard packet;
    packet.put_header(0, 0x1e4);
    packet.put_byte(ok ? 1 : 0);
    packet.finalize(true);
    m_party->send_to_party(packet);
    return 1;
