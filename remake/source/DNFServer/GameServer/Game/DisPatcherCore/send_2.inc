void DisPatcher_DeleteCharac::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081c14b8：指针局部 -0xc；条件与实参各自独立读 m_errNo（两次 field load），
    // 非 0 → SendCmdErrorPacket(cmd=6, (uchar)err)。
    stDeleteCharac* pParam = static_cast<stDeleteCharac*>(&param);   // ORIG -0xc(%ebp)
    if (pParam->m_errNo != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)6,
                                 (unsigned char)pParam->m_errNo);
    }
