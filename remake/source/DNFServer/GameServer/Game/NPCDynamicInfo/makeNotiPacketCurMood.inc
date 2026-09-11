// ORIG 0x08580624 T：put_short(getIndex()) + put_byte((char)getMood())。
void CNPCDynamicInfo::makeNotiPacketCurMood(PacketGuard& guard) const
{
    guard.put_short(m_script->getIndex());
    guard.put_byte((char)getMood());
}