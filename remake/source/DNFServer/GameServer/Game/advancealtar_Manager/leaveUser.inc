// ---- ORIG T 0x081310b2（advancealtar__Manager/leaveUser.md 全文）----
void Manager::leaveUser(CUser* user)
{
    if (user == 0)
    {
        return;
    }
    // [b28d-D33] P0：ORIG 0x081310b2 段 `call G_GameWorld; mov %eax,(%esp); call
    // out_from_dungeon` 无 -0xc 槽往返 ⇒ 原码无 world 具名局部（去缓存）。
    G_GameWorld()->out_from_dungeon(user);

    PacketGuard packet;
    packet.put_header(0, 2);
    packet.put_byte(0);
    packet.put_short(1);
    user->make_basic_info(reinterpret_cast<char*>(&packet), '\0');
    packet.finalize(true);
    G_GameWorld()->send_all(packet, user);
}