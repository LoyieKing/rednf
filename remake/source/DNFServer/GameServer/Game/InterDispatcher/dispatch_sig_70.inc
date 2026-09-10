int Inter_UserRepel::dispatch_sig(CUser* user, char* /*data*/, int /*len*/)
{
    user->DisConnSig(static_cast<DISCONN_SIG>(0x14), true, 0);
    return 0;
