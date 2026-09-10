void user_creature::CCreatureItem::Set(user_creature::CCreatureItem* src)
{
    // ORIG 0x0833ef20：经 getter 逐字段读 src（-O0 产生 call），且
    // m_expireTime(+0x18) 置 0 而非拷贝（蛋→宠物项复制，新宠无到期时间）。
    m_itemId = src->GetItemId();
    m_creatureItemType = src->GetCreatureItemType();
    m_uid = src->GetUid();
    m_slotNo = src->GetSlotNo();
    m_noCharge = src->GetNoCharge();
    m_stat = src->GetStat();
    m_expireTime = 0;
}
