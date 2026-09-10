void CCompoundAvatar::_SendError(CUser* user, ENUM_ERROR err)
{
    user->SendCmdErrorPacket((ENUM_CMDPACKET)0x66, (unsigned char)err);
