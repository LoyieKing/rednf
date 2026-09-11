// ============================================================================
// _NoticeUpgrade @ 0x8546824
//   广播强化通知包（header 0x56）。
// ============================================================================
void CItemUpgrade::_NoticeUpgrade(const CUser* user, const Inven_Item& item,
                                  bool isAmplify)
{
    PacketGuard packet;
    packet.put_header(0, 0x56);
    packet.put_byte(1);
    packet.put_byte(isAmplify);
    packet.put_short((int)user->get_unique_id());
    packet.put_int(item.m_addInfo);
    packet.put_byte((int)item.GetUpgrade());
    item.m_random.m_random.put_packet_random_option(packet);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
}