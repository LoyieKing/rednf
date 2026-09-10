bool CCreatureMgr::InsertCreatureItem(Inven_Item* item, int slotNo, int reason,
                                      int days, int expireTick)
{
    if (!IsCreatureEquipmentScope(slotNo))
        return true;
    if (item->m_field1 != 5)  // ITEM_TYPE_CREATURE_EQUIP（movzbl +0x1 cmp $5）
        return true;

    CDBMsgSender sender((ENUM_INTERNALPACKET)0x87, m_user);
    int mapSize = (int)m_creatureItems.size();
    if (!sender.InsertCreatureItem(m_user, item, slotNo, reason, mapSize,
                                   days, expireTick))
    {
        // [b28d-solo P221] 全表达式临时链（ORIG trace 槽 -0x20）
        cMyTrace("bool user_creature::CCreatureMgr::InsertCreatureItem("
                 "Inven_Item*, int, int, int, int)", 0xcaf, 5)(
            "cannot write db uid(%d)\n", m_user->get_unique_id() & 0xffff);
        return false;
    }
    sender.Send();

    int tempTime = 0;
    if (days == 0 && expireTick != 0)
    {
        // [b28d-solo-CCreatureMgr P243/P0] ORIG 0x833b325 `mov 0x1c(%ebp),%ebx;
        // call GetDateTimeTick; mov %ebx,%edx; sub %eax,%edx` = 单句
        // `expireTick - GetDateTimeTick()`（两句式会多一次槽回读）。
        register int tick = expireTick;   // P248 裸 register（非绑定，P0 合规）
        tempTime = tick - OS_API::GetDateTimeTick();
    }
    else
    {
        tempTime = days * 0x15180;
    }
    setTempExpireTime(tempTime);
    return true;
