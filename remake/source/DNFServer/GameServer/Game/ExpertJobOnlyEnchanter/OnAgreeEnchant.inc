void CEnchanter::OnAgreeEnchant(CUser* user, MSG_STATIC_COMMAND* msg)
{
    int err = 0;
    if ((err = _AgreeEnchant(
             user,
             (ENUM_AGREE_ENCHANT_TYPE)(unsigned char)msg->m_type0d)) != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xab,
                                 (unsigned char)(err & 0xff));
        if (m_user != 0)
            m_user->SendCmdErrorPacket((ENUM_CMDPACKET)0xab,
                                       (unsigned char)(err & 0xff));
    }
