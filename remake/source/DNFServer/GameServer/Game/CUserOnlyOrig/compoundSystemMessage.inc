// ---------------------------------------------------------------------------
// 5. CUser::compoundSystemMessage(Inven_Item const&) T 0x08692f62
void CUser::compoundSystemMessage(Inven_Item const& item)
{
    PacketGuard pkt;
    pkt.put_header( 0, 0x56);
    pkt.put_byte( 0xc);
    pkt.put_short( (unsigned)get_unique_id() & 0xffff);
    pkt.put_int( item.getKey());
    pkt.put_byte( (unsigned)item.GetUpgrade() & 0xff);
    pkt.finalize( true);
    G_GameWorld()->send_all(pkt);
}