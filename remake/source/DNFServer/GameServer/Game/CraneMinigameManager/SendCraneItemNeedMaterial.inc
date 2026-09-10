void CraneMinigameManager::SendCraneItemNeedMaterial(CUser* user)
{
    PacketGuard packet;
    packet.put_header(0, 0x1a7);
    packet.put_byte(m_selectIdx);
    packet.finalize(true);
    user->Send(packet);
