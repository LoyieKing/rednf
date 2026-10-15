// ORIG CCreatureMgr::InsertCreatureItem @ 0x833b204（InsertCreatureItem.md）：
// 槽位/类型过滤后 CDBMsgSender(0x87, m_user)，size=m_creatureItems.size()，
// sender.InsertCreatureItem(m_user,item,slot,reason,size,days,expireTick)：
// 成功 → sender.Send() + 到期时间（days==0 且 expireTick!=0 取
// expireTick-GetDateTimeTick()，否则 days*0x15180）写 m_tempExpireDelta 返回 1；
// 失败 → cMyTrace("...InsertCreatureItem(...)",0xcaf,5) 返回 0。
// [b28d-23 N·N82] ⇒ N。ORIG 0x0833b204
// [b28d-solo-CCreatureMgr AE] P248 裸 register tick=expireTick 令值升格 callee-saved %ebx 跨 GetDateTimeTick call 保活（非绑定，P0 合规）；ORIG 0x833b204
bool CCreatureMgr::InsertCreatureItem(Inven_Item* item, int slotNo, int reason,
                                      int days, int expireTick)
{
    if (!IsCreatureEquipmentScope(slotNo))
        return true;
    if (item->m_itemType != 5)  // ITEM_TYPE_CREATURE_EQUIP（movzbl +0x1 cmp $5）
        return true;

    CDBMsgSender sender((ENUM_INTERNALPACKET)0x87, m_user);
    int mapSize = (int)m_creatureItems.size();
    if (!sender.InsertCreatureItem(m_user, item, slotNo, reason, mapSize,
                                   days, expireTick))
    {
        // [b28d-solo P221] 全表达式临时链（ORIG trace 槽 -0x20）
        // [R2PrettAClass 2026-09-11 · prett A 类落地] 手写签名串 → __PRETTY_FUNCTION__
        //   （ORIG .rodata @0x08c2ee60 size 0x56 = "bool user_creature::CCreatureMgr::"
        //   "InsertCreatureItem(Inven_Item*, int, int, int, int)"；非 static 成员 ⇒ 无前缀）。
        cMyTrace(__PRETTY_FUNCTION__, 0xcaf, 5)(
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
}
