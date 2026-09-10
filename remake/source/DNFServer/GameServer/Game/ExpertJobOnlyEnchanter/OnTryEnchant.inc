void CEnchanter::OnTryEnchant(CUser* user, MSG_STATIC_COMMAND* msg)
{
    int err = 0;   // ORIG 首条 movl $0,-0xc：err 声明置函数头
    m_field40 = G_CDataManager()->find_item(msg->m_value0eU);
    if ((err = _TryEnchant(
             user, reinterpret_cast<stItemPos_t*>((char*)msg + 0x13),
             (ENUM_TRY_ENCHANT_TYPE)(unsigned char)msg->m_type0d)) != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xac,
                                 (unsigned char)(err & 0xff));
    }
    if (err == 0)
    {
        APSystem::CUserProc::ClearActionAndSendtoUser(
            user, APSystem::_ActionGroupIndex(0x1f), 0,
            (ENUM_PACKETCLASS)0);
    }
