// ----------------------------------------------------------------------------
// ORIG 0x0868f0e8 CUser::sendCharacOption() void
void CUser::sendCharacOption()
{
    if (getCurCharacR() == 0) return;
    PacketGuard packet;
    packet.put_header(0, 0x186);
    packet.put_int(0x200);
    packet.put_binary(const_cast<_Charac_info*>(getCurCharacR())->m_characterOption.m_pad, 0x200);  // +0x12b7 CharacterOption 具名区 // 角色选项块
    packet.finalize(true);
    Send(packet);
}