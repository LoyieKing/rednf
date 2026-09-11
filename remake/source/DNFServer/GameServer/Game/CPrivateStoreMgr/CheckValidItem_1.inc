// CheckValidItem(INVEN_TYPE, int slot, int count) @0x85c9bb8：
// 槽位有效且非空；type==1 校验装备外数量余量；type==2 仅 avatar 且非临时键、未过期。
// [b28d-24] DIFF 残24: P167 MISSING CInventory::GetAvatarItemMgrR×1（真缺口待补）。ORIG 0x085c9bb8
bool CPrivateStore::CheckValidItem(INVEN_TYPE type, int slot, int count)
{
    const CInventory* inventory = m_pSeller->getCurCharacInvenR();  // ORIG 读路径（InvenR）
    if (!inventory->CheckValidSlot((int)type, slot))
        return false;

    Inven_Item item = inventory->GetInvenSlot(type, slot);
    if ((int)item.m_addInfo == 0)
        return false;

    if ((int)type == 1)
    {
        // P225 推广：ORIG 两条件各发独立 je 直跳成功出口（0x85c9c1a/0x85c9c27），
        // 合并 `&&` 会被 4.4 物化成 setcc 岛 ⇒ 拆嵌套 if。
        if (!item.isEquipableItemType())
        {
            if ((int)item.m_addInfo2 - count < 0)
                return false;
        }
    }
    else if ((int)type == 2)
    {
        if ((int)item.m_field1 != Inven_Item::ITEM_TYPE_AVATAR)
            return false;
        // P48：ORIG 两次独立 call GetAvatarItemMgrR（0x85c9c8a/0x85c9cb4），
        // 4.4 无 CSE ⇒ 不设 mgr 缓存局部，逐次写完整链（P52 实参先物化）。
        if (inventory->GetAvatarItemMgrR()->IsTempKey((int)item.m_addInfo2))
            return false;
        if (inventory->GetAvatarItemMgrR()->GetExpireDate((int)item.m_addInfo2) != 0)
            return false;
    }
    return true;
}