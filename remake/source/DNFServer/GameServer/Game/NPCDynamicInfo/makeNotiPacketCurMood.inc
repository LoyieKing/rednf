void CNPCDynamicInfo::makeNotiPacketCurMood(PacketGuard& guard) const
{
    guard.put_short(m_script->getIndex());
    guard.put_byte((char)getMood());
