int CEnchanter::_AgreeEnchant(CUser* user, ENUM_AGREE_ENCHANT_TYPE type)
{
    int ret = 0;   // -0xc：ORIG movl $0 开头
    if ((unsigned short)user->get_unique_id() !=
        (unsigned short)m_peer->get_unique_id())   // ORIG cmp %ax,%bx（16 位）
        return 3;

    // ORIG 汇编：type 加载一次入 eax 后 cmp $1/je、cmp $2/je、test/jne
    // （线性比较链）→ switch，case 书写序 = 叶块排放序（P46）。
    switch (type)
    {
    case ENUM_AGREE_ENCHANT_TYPE_0:
        if (m_materials[1].Empty())
            return 0x15;
        m_materials[1].Reset();
        m_field40 = 0;

        if (m_user != 0 && m_user->getCurCharacR() != 0)
        {
            PacketGuard packet;
            packet.put_header(1, 0xac);
            packet.put_byte(1);
            packet.put_byte(3);
            packet.finalize(true);
            m_user->Send(packet);
        }
        break;
    case ENUM_AGREE_ENCHANT_TYPE_1:
        ret = _DoEnchant(ENUM_TRY_ENCHANT_TYPE_1);
        OnCloseMission(user, ENUM_CLOSE_MISSION_TYPE_0);
        return ret;
    case ENUM_AGREE_ENCHANT_TYPE_2:
        OnCloseMission(user, ENUM_CLOSE_MISSION_TYPE_2);
        break;
    default:
        break;
    }
    return 0;
