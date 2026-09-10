bool CCreatureMgr::RecvMail(Inven_Item* item, int slot, int reason)
{
    if (!IsCreatureEquipmentScope(slot))
        return true;
    if (item->m_field1 != 5)  // ITEM_TYPE_CREATURE_EQUIP（movzbl +0x1 cmp $5）
        return true;

    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, m_user);
    int mapSize = (int)m_creatureItems.size();
    // [b28d-solo-CCreatureMgr P0/P183] ORIG 0x833ac0f `call getCurCharacNo` 结果
    // 经 %eax 直压 0x8(%esp)（无 characNo 具名槽）⇒ 删局部、内联实参。
    if (!sender.RecvMail(item, m_user->getCurCharacNo(), slot, reason, mapSize))
    {
        // [b28d-solo P221] 全表达式临时链（ORIG trace 槽 -0x2c 浅于 sender 邻域）
        cMyTrace(__PRETTY_FUNCTION__, 0xc09, 5)(
            "cannot write db uid(%d)\n", m_user->get_unique_id() & 0xffff);
        return false;
    }
    sender.Send();
    // WriteCreatureLog(INVEN_TYPE 3, item+0x02, item+0x07, 1, item+0x01, reason)
    m_user->getCurCharacInvenR()->WriteCreatureLog(
        (INVEN_TYPE)3, item->m_addInfo, item->m_addInfo2, 1,
        item->m_field1, (eItemAddReason)reason);
    return true;
}
