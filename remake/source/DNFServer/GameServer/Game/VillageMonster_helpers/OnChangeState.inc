// ============================================================================
// CVillageMonster::OnChangeState(ENUM_VILLAGE_MONSTER_STATE)
//   —— ORIG 0x86b35a0
//   objdump: m_state = state; PacketGuard 打包：put_header(0,0xf0); put_byte(m_state);
//            put_short(m_id); finalize(true); m_area->SendPacket(pg);
// ============================================================================
void CVillageMonster::OnChangeState(ENUM_VILLAGE_MONSTER_STATE state)
