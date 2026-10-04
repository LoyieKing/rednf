// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0864d646 | _ZN5CUser12GetInventoryEP18SIG_LOAD_INVENTORY
bool CUser::GetInventory(SIG_LOAD_INVENTORY* sig)
{
    // P183：ORIG 每字段访问独立 getCurCharacInvenR()/getCurCharacCargoR() 重物化
    //（P167 缺 getCurCharacInvenR×11 + getCurCharacCargoR×1），禁指针缓存。
    sig->m_money = getCurCharacInvenR()->get_money();
    sig->m_coin = getCurCharacInvenR()->GetCoin();
    sig->m_payCoin = getCurCharacInvenR()->GetPayCoin();
    sig->m_eventCoin = getCurCharacInvenR()->GetEventCoin();
    getCurCharacInvenR()->GetInvenData(1, &sig->m_invenType1, 0x4a58);
    getCurCharacInvenR()->GetInvenData(2, &sig->m_invenType2, 0x1905);
    getCurCharacInvenR()->GetInvenData(3, &sig->m_invenType3, 0x39aa);
    // 装备镜像 = CInventory::m_invenItem 前 22 项（0x53e B）
    memcpy(&sig->m_equipMirror, getCurCharacInvenR()->m_invenItem, 0x53e);
    // creature tail：m_invenType3 区内 +0x38b6 跨条目 0xf4 B（源 m_invenItem[22..25]，
    // ORIG 0x0864d646 忠实保留跨对齐写）。
    memcpy(&sig->m_invenType3[0xee],
           &getCurCharacInvenR()->m_invenItem[22], 0xf4);
    getCurCharacInvenR()->GetClearAvatar(sig->m_clearAvatar);   // b28d-solo-CUser：数组退化免 cast（P255/g17 形）
    sig->m_cargoCapacity = getCurCharacCargoR()->GetCapacity();   // 同型直存（GetCapacity 已 unsigned 化）
    getCurCharacCargoR()->get_cargo(&sig->m_cargo);
    sig->m_avatarCoin = getCurCharacInvenR()->m_avatarCoin.Get();   // CInventory::m_avatarCoin@+0x658
    sig->m_inventoryCapacity = getCurCharacInvenR()->get_inventory_capacity();
    return true;
}