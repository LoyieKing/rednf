bool eventReward::createCharacEquipReward(Inven_Item* item, rewardKey key, int count)
{
    bool ok = false;
    if (G_CDataManager()->m_eventReward.rewardWriteUserEquip(item, key, count))
        ok = true;
    key.m_a = 0;                                    // +0x11
    key.m_sub.m_action = 2;                         // +0x14（equip）
    if (G_CDataManager()->m_eventReward.rewardWriteUserEquip(item, key, count))
        ok = true;
    key.m_job = 0xb;                                // +0x10
    key.m_a = 0;                                    // +0x11
    key.m_sub.m_action = 2;                         // +0x14
    if (G_CDataManager()->m_eventReward.rewardWriteUserEquip(item, key, count))
        ok = true;
    return ok;
