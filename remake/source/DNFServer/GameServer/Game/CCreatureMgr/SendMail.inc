// ORIG _ZN13user_creature12CCreatureMgr8SendMailEP10Inven_Itemii T 0x0833aa32
// （docs/class_func_reports/user_creature__CCreatureMgr/SendMail.md）。
// 逻辑：非宠物装备槽 → 1；item 类型非 5（CREATURE_EQUIP）→ 1；
// CDBMsgSender sender(0x8a,m_user) → mapSize=m_creatureItems.size()，
// characNo=m_user->getCurCharacNo()；SendMail 写库失败 → cMyTrace 并返回 0；
// 成功 → sender.Send() 后 DeleteCreatureItem(item->m_addInfo2, reason)。
// [b28d-23 N·N82] ⇒ N。ORIG 0x0833aa32
// [b28d-solo-CCreatureMgr AE] P194 尾判 `if(!DeleteCreatureItem())return false;return true;` + P221 临时链 + P183 去 characNo 槽；ORIG 0x833aa32
bool CCreatureMgr::SendMail(Inven_Item* item, int slot, int reason)
{
    if (!IsCreatureEquipmentScope(slot))
        return true;
    if (item->m_field1 != 5)  // ITEM_TYPE_CREATURE_EQUIP（movzbl +0x1 cmp $5）
        return true;

    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, m_user);
    int mapSize = (int)m_creatureItems.size();
    // [b28d-solo-CCreatureMgr P0/P183] 同 RecvMail：characNo 无具名槽。
    if (!sender.SendMail(item, m_user->getCurCharacNo(), slot, reason, mapSize))
    {
        // [b28d-solo P221] 全表达式临时链（ORIG trace 槽 -0x1c）
        cMyTrace(__PRETTY_FUNCTION__, 0xbed, 5)(
            "cannot write db uid(%d)\n", m_user->get_unique_id() & 0xffff);
        return false;
    }
    sender.Send();
    // [b28d-solo-CCreatureMgr P194] ORIG 0x833ab56 `xor $1;test;je <mov$0>;mov$1`
    // ⇒ 原码是 `if (!DeleteCreatureItem(...)) return false; return true;`
    // （直返透传形无此三段）。
    if (!DeleteCreatureItem(item->m_addInfo2, reason))
        return false;
    return true;
}
