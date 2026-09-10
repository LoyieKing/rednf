void DisPatcher_LeaveParty::send(CUser* user, ParamBase& param)

{
    stLeavePartyParam& p = *reinterpret_cast<stLeavePartyParam*>(&param);
    if (p.m_errNo == 0x7fffffff)
        return;
    if (p.m_errNo != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd, (unsigned char)(p.m_errNo & 0xff));
}
