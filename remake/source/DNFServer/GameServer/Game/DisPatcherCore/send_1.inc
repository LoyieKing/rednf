void DisPatcher_CreateCharac::send(CUser* user, ParamBase& param)

{
    stCreateCharac& p = *reinterpret_cast<stCreateCharac*>(&param);
    if (p.m_errNo == 0x7fffffff)
        return;
    if (p.m_errNo != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)5, (unsigned char)(p.m_errNo & 0xff));
}
