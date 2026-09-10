void DisPatcher_RemoveFriend::send(CUser* user, ParamBase& param)
{
    stRemoveFriendParam& p = *reinterpret_cast<stRemoveFriendParam*>(&param);
    if (p.m_errNo == 0x7fffffff)
        return;
    if (p.m_errNo != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x40, (unsigned char)(p.m_errNo & 0xff));
