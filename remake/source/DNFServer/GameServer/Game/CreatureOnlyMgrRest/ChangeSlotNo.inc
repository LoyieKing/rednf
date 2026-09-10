bool CCreatureMgr::ChangeSlotNo(Inven_Item* item, int newSlot, int param_3)
{
    if (!IsCreatureEquipmentScope(newSlot))
        return true;
    if (item->m_field1 != 5)
        return true;

    CCreatureItem* creatureItem =
        FindCreatureItem(static_cast<int>(item->m_addInfo2));
    if (creatureItem == 0)
        return false;

    creatureItem->SetSlotNo(newSlot);   // ORIG T 0x08337050（写 +0x10）

    // b28d-solo-CreatureOnlyMgrRest [P212/P309]：ORIG reason 占最深槽 -0x3c
    // （sender -0x34 / mapSize -0x1c）⇒ 全头部声明、先声明深。
    eItemAddReason reason;
    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, m_user);
    // b28d-solo-CreatureOnlyMgrRest [P0/P227]：ORIG 0x0833af74 getCurCharacNo
    // 结果无槽（直接 0x8(%esp)）⇒ 内联；失败 trace uid 在 ctor 前
    // movzwl→%ebx 保活 ⇒ 全表达式临时链。
    if (!sender.ChangeSlotNo(item, m_user->getCurCharacNo(), newSlot, param_3))
    {
        // b28d-solo-CreatureOnlyMgrRest [P139]：函数名串用 __PRETTY_FUNCTION__
        // 复现 ORIG `_ZZ...E19__PRETTY_FUNCTION__` 局部静态符号。
        cMyTrace(__PRETTY_FUNCTION__, 0xc44, 5)(
            "cannot write db uid(%d)\n", m_user->get_unique_id() & 0xffff);
        return false;
    }
    sender.Send();

    // b28d-solo-CreatureOnlyMgrRest [P52]：ORIG 0x0833b00d reason 落槽 -0x3c、
    // field1/addInfo2/addInfo 先物化 edi/esi/ebx 再 call getCurCharacInvenR。
    reason = (eItemAddReason)param_3;
    m_user->getCurCharacInvenR()->WriteCreatureLog(
        (INVEN_TYPE)3, item->m_addInfo, item->m_addInfo2, 1,
        (int)item->m_field1, reason);
    return true;
