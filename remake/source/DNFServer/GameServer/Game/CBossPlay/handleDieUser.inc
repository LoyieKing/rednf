bool CBossPlay::handleDieUser(CUser* user)
{
    // [b28d-46 实测] ORIG 双 `movl $0,-0xc`（P191）+ 尾部 `cmpl $3; setle; test;
    // jne`（P51）+ 两独立守卫各 `je INC`（P133）⇒ for + continue 形。
    int i = 0;
    for (i = 0; i <= 3; ++i)
    {
        if (!m_party->checkValidUser(i))
            continue;
        if (!(m_party->get_user(i) == user))
            continue;
        if (isMemberAlive(i))
        {
        setMemberAlive(i, false);

        PacketGuard packet1;
        packet1.put_header(0, 0x20);
        packet1.put_short(user->get_unique_id() & 0xffff);
        packet1.put_byte(0);
        packet1.finalize(true);
        m_party->send_to_party(packet1);

        if (user->GetRevivalTime() > GlobalData::s_systemTime_.getCurSec())
        {
            user->SetRevivalTime(0);
            user->SetUseAPCPotionTime(0);
            setMemberAlive(i, true);

            PacketGuard packet2;
            packet2.put_header(0, 0x20);
            packet2.put_short(user->get_unique_id() & 0xffff);
            packet2.put_byte(2);
            packet2.finalize(true);
            m_party->send_to_party(packet2);
        }
        return 1;
        }
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x2b, 0x12);
        return 0;
    }
    return 1;
