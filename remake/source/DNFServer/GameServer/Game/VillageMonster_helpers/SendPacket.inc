// ============================================================================
// CVillageMonsterArea::SendPacket(PacketGuard&)
//   —— ORIG 0x86b3b9c
//   objdump: G_GameWorld()->send_to_area(+0x28, +0x2c, packet);
// ============================================================================
void CVillageMonsterArea::SendPacket(PacketGuard& packet)
{
    G_GameWorld()->send_to_area(m_field28, m_field2c, packet);
}