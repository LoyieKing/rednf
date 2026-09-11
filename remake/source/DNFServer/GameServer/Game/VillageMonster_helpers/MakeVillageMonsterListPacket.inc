// ============================================================================
// CVillageMonster::MakeVillageMonsterListPacket(PacketGuard&)
//   —— ORIG 0x86b3666（0x80 字节）
//   objdump: put_short(m_id); put_short(m_zone); put_byte(m_state);
//            put_short(m_field04); put_short(m_field08); return 1。
// ============================================================================
// [b28d-L8] AE | 修法：m_state int→ENUM_VILLAGE_MONSTER_STATE（P254 enum 字段两段装载 mov 0x14(%eax),%eax;mov %eax,%edx，ORIG 0x86b3666）；旧 N18a 判词作废
bool CVillageMonster::MakeVillageMonsterListPacket(PacketGuard& packet)
{
    // [b28-L11 判 N·N18a] ORIG put_short(m_field04) 实参装载为 `mov 0x14(%eax),%eax;
    // mov %eax,%edx` 两段死 regmove（4.1 冗余产物）；4.4 恒折单段直落 %edx，
    // 「反向亦不可造」判词在册（负例表 N18a）。ndiff 3/gap 1 封顶。
    packet.put_short(m_id);
    packet.put_short(m_zone);
    packet.put_byte(m_state);
    packet.put_short(m_field04);
    packet.put_short(m_field08);
    return true;
}