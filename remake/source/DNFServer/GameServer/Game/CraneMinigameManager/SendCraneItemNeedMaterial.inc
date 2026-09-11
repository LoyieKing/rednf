// ORIG 0x080ebf2e（报告 CraneMinigameManager/SendCraneItemNeedMaterial.md）：
// PacketGuard → put_header(0,0x1a7) → put_byte(m_selectIdx +0x68) →
// finalize(true) → CUser::Send。
void CraneMinigameManager::SendCraneItemNeedMaterial(CUser* user)
{
    PacketGuard packet;
    packet.put_header(0, 0x1a7);
    packet.put_byte(m_selectIdx);
    packet.finalize(true);
    user->Send(packet);
}