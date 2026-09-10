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
