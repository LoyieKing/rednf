int Inter_SelectAllowProxyUser::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0 || data == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__,
            0x6e1e, 5)("[%s] pUser or p is null.",
              __PRETTY_FUNCTION__);
        return 0x6e1f;
    }
    user->SendCmdErrorPacket((ENUM_CMDPACKET)2, 7);
    user->DisConnSig((DISCONN_SIG)0x1e, true, 0);
    return 0;
