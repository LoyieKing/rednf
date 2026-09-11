void GameWorld::SendPowerUpList(ENUM_POWER_UP_ACTIVATE_TYPE activateType, std::vector<CUser*>& list)
{
    PacketGuard packet;
    packet.put_header(0, 0xd8);
    // ORIG 0x86cf498-0x86cf4a8: `movzbl %al,%eax` = uchar cast (非 char)
    packet.put_byte((unsigned char)activateType);
    // ORIG 0x86cf4b3-0x86cf4b8: size() 结果直落 int 形参，无 (short) 截断
    packet.put_short(list.size());
    for (std::vector<CUser*>::iterator it = list.begin(); it != list.end(); ++it)
    {
        // ORIG 0x86cf4ee-0x86cf4f6: get_unique_id(ushort)→int 仅 movzwl，无 (short)
        packet.put_short((*it)->get_unique_id());
    }
    packet.finalize(true);
    // ORIG 0x86cf54e-0x86cf565: call G_GameWorld 后 %eax 直落 this 槽 ⇒ 自由函数形
    G_GameWorld()->send_to_vill(7, packet);
}