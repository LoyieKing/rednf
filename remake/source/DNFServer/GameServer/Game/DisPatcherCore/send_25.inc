void DisPatcher_AddFriend::send(CUser* user, ParamBase& param)
{
    stAddFriendParam& p = *reinterpret_cast<stAddFriendParam*>(&param);
    // ORIG 0x081cb4e6：call 后显式 jmp + je 目标含 nop 空块 → 首条件为
    // void early-return（探针 c6root gcc4.4.7 sD 形态实证）。
    if (p.m_field4 == 0x7fffffff)
        return;
    if (p.m_field4 != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x3f,
                                 (unsigned char)(p.m_field4 & 0xff));
