void MakeNotiSetDurability(PacketGuard& packet, int value);

// ORIG 0x082f85ea：m_field3e8(+0x3e8)==0 直接返回；否则
// MakeNotiSetDurability(packet, (int)(m_durCurrent(+0x43c double) * 1000.0))
// （常量 1000.0 = _DAT_08c1fc58，.rodata 双精度 1000.0），CUser::Send。
