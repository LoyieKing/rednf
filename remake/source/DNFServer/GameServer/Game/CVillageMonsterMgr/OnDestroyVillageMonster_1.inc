void CVillageMonsterMgr::OnDestroyVillageMonster(ENUM_VILLAGE_ATTACKED_REWARD reward)
{
    for (std::map<Zone, CVillageMonsterArea>::iterator it = m_areas.begin();
         it != m_areas.end(); ++it)
    {
        it->second.OnDestroyVillageMonster();
    }
    m_field04 = 0;
    m_field00 = 0;
    // P95：ORIG 0x86b43d4 末段 store 先载 this 后载 reward；具名视图复现。
    struct CVMgrRewardView
    {
        char m_head[0x20];
        int  m_field20;
    };
    ((CVMgrRewardView*)this)->m_field20 = (int)reward;
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xf2);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
