void Teleport::send_teleport_effect(CUser* user, int type)
{
    // ORIG 0x08283eba：PacketGuard put_header(0,0x17b)/put_byte(type)/put_byte(1)
    // /put_short(GetUID)/finalize(true)；按 vill/area send_to_area。
    if (user != 0)
    {
        PacketGuard packet;
        packet.put_header(0, 0x17b);
        packet.put_byte((int)(char)type);
        packet.put_byte(1);
        packet.put_short(user->GetUID());
        packet.finalize(true);
        G_GameWorld()->send_to_area(
            (int)user->getCurCharacVill(), user->get_area(false), packet);
    }
    // ORIG send_teleport_effect 尾岛：call send_to_area 后有 jmp 到共享
    // epilogue；return 写在 if 块后（技巧 7 尾置 return 岛）。
    return;
