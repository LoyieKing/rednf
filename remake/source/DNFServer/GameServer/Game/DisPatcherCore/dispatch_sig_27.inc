int DisPatcher_Cera::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x081fd360：s_pIPGHelper 虚表 +0x2c（slot11 IPGQuery——vtable
    // 0x08b420a8 逐槽实证：+0x28=slot10 IPGOutputConfirm(SIPGData*,SIPGData*)
    // 0x08100e0a，+0x2c=slot11 IPGQuery 0x08100790）；随后 LoadMileage(user)。
    GlobalData::s_pIPGHelper->IPGQuery(user);
    user->LoadMileage();
    return 0;
