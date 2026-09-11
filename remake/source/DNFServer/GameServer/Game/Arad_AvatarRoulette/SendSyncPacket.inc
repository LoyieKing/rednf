// ---- AvatarCoin::SendSyncPacket ORIG @ 0x0817ffe4 ----
// bool 返回：ORIG 调用点 0x0819f1cd `xor $0x1,%eax` 判据（Dispatchers 多处）。
bool AvatarCoin::SendSyncPacket(CUser* user)
{
    if (user == 0)
        return false;
    if (user->getCurCharacR() == 0)
        return false;

    PacketGuard packet;
    packet.put_header(1, 0x24b);
    packet.put_byte(1);   // ORIG 0x0817ffe4 头部后追加 1 字节
    // ORIG 0x818005f：inven 基址 add $0x658 → Get()（无 coinOf 调用、无 coin 槽）
    packet.put_int(reinterpret_cast<AvatarCoin*>(
                       const_cast<char*>(reinterpret_cast<const char*>(
                           user->getCurCharacInvenR())) + 0x658)->Get());
    packet.finalize(true);
    user->Send(packet);
    return true;
}