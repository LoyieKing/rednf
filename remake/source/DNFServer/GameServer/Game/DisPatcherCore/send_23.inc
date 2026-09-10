void DisPatcher_SetPVPReady::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081c9bc6：param+0x4（stOnSetPvPReadyParam::m_errNo）> 0（jle 有符号）
    // → SendCmdErrorPacket(cmd=0x38, err&0xff)。
    stOnSetPvPReadyParam& out = static_cast<stOnSetPvPReadyParam&>(param);
    if (0 < out.m_errNo)
    {
        user->SendCmdErrorPacket(
            (ENUM_CMDPACKET)0x38,
            (unsigned char)(out.m_errNo & 0xff));
    }
