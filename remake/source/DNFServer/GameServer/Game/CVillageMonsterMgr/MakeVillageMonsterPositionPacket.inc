// ============================================================================
// CVillageMonster::MakeVillageMonsterPositionPacket —— ORIG T 0x86b36e6
//   仅 m_field10==1（本轮移动过）时：put_short(m_id/m_field04/m_field08)，
//   清零 m_field10，返回 true；否则返回 false。
// ============================================================================
bool CVillageMonster::MakeVillageMonsterPositionPacket(PacketGuard& packet)
{
    // [b28-L14] ORIG T 0x86b36e6 = `movzbl 0x10(%eax),%eax; xor $0x1,%eax;
    // test %al,%al; je body` ⇒ 字段为 bool 且源码写 `if (!m_field10) return false;`
    // （P168a 裸形；旧 L11 判词「裸 != 1 反演」实为字段类型 char 所致，m_field10
    // char→bool 后 4.4 直产 xor$1 物化形）。
    if (!m_field10)
        return false;
    // 以下保持原序：三次 put_short 后清零 m_field10。
    packet.put_short(m_id);
    packet.put_short(m_field04);
    packet.put_short(m_field08);
    m_field10 = 0;
    return true;
}