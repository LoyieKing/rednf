// ============================================================================
// ORIG T 0x08690b58 _ZN5CUser21SendManageLevelUpNotiEv：空包 header(0,0x19f)。
// ============================================================================
void CUser::SendManageLevelUpNoti()
{
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x19f);
    packet.finalize(true);
    Send(packet);
}