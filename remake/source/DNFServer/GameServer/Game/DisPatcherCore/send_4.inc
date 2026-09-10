void DisPatcher_SetPartyInfo::send(CUser* user, ParamBase& param)

{
    stSetPartyInfo& p = *reinterpret_cast<stSetPartyInfo*>(&param);
    if (p.m_errNo == 0x7fffffff)
        return;
    if (p.m_errNo != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc, (unsigned char)(p.m_errNo & 0xff));
}
