int Inter_GoblinPadSamePwdCheck::dispatch_sig(
    CUser* user, char* data, int /*len*/)
{
    char* packet = data;
    if (user != 0)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x74),
                                 static_cast<unsigned char>(0x8b));
    }
    return 0;
