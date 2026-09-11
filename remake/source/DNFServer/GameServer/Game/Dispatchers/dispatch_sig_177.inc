int Dispatcher_AvartarCoinCount::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x819e660：AvatarCoin::SendSyncPacket(user)；成功 → 0，失败 → 0x353。
    (void)packet;
    // ORIG xor $0x1,%eax;test %al,%al：bool 直判形
    if (!AvatarCoin::SendSyncPacket(user))
    {
        return 0x353;
    }
    return 0;
}