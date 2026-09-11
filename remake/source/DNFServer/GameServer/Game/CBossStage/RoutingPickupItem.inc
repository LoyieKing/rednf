// ---- RoutingPickupItem（ORIG 0x0814e57a）----
// [b28d-62] N82：p167 CLEAR。ORIG 0x0814e57a
// [b28d-solo-CBossStage] AE：删具名 bool failed（P226/P158）；`while (ret != 0)
//   {…return false;}` 复现 ORIG setne;test;je 物化（ndiff 33→0）。
bool CBossStage::RoutingPickupItem(CUser* user, unsigned int uid,
                                   map_item& item)
{
    CSpecialItemRoutingManager* mgr =
        G_CGameManager()->GetSpecialItemRoutingManager();

    int ret = 0;
    ret = mgr->StartRouting(user, item.m_item, uid);
    while (ret != 0)
    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(1, 0x2e);
        packet.put_byte(0);
        packet.put_byte(ret);
        packet.finalize(true);
        user->Send(packet);
        return false;
    }
    return true;
}