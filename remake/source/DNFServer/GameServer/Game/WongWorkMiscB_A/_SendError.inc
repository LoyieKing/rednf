// ---------------------------------------------------------------------------
// _SendError —— T 0x08333a76：SendCmdErrorPacket(user, 0x66, err)。
void CCompoundAvatar::_SendError(CUser* user, ENUM_ERROR err)
{
    user->SendCmdErrorPacket((ENUM_CMDPACKET)0x66, (unsigned char)err);
}