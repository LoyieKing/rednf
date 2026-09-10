void DisPatcher_SelectCharac::send(CUser* user, ParamBase& param)

{
    // ORIG 0x081c6826 T：param+4 == 0x7fffffff → void early-return（call 尾
    // 显式 jmp + je→nop 空块岛实证）；!=0 时 SendCmdErrorPacket(4, (uchar)值)。
    stSelectCharac& out = static_cast<stSelectCharac&>(param);
    if (out.m_errNo == 0x7fffffff)
        return;
    if (out.m_errNo != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)4, (unsigned char)out.m_errNo);
}
