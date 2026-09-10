bool CCreatureMgr::ChangeOwner(Inven_Item* item, int slotFrom, int mapSizeHint,
                               int param_4, int param_5)
{
    if (!IsCreatureEquipmentScope(slotFrom))
        return true;
    if (item->m_field1 != 5)  // ITEM_TYPE_CREATURE_EQUIP
        return true;

    // b28d-solo-CreatureOnlyMgrRest [P212/P309]：ORIG reason 占最深槽 -0x3c
    // （sender -0x34 / mapSize -0x1c）⇒ 全头部声明、先声明深。
    eItemAddReason reason;
    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, m_user);
    int mapSize = (int)m_creatureItems.size();
    // ORIG 实参序：(item, characNo, slotFrom, mapSizeHint, mapSize, p4, p5)；
    // sender 侧 +0x48=characNo、+0x2c=slotFrom、+0x51 低字节=mapSizeHint、
    // +0x68=mapSize、+0x6c=p4、+0x70=p5（汇编 ebp+0x10..0x24 对应）。
    // b28d-solo-CreatureOnlyMgrRest [P0]：ORIG 0x0833ada9 getCurCharacNo 结果
    // 无槽（直接 0x8(%esp)）⇒ 内联实参。
    if (!sender.ChangeOwner(item, m_user->getCurCharacNo(), slotFrom, mapSizeHint,
                            mapSize, param_4, param_5))
    {
        // b28d-solo-CreatureOnlyMgrRest [P227/P139]：ORIG uid 在 ctor call 前
        // movzwl→%ebx 保活 ⇒ 全表达式临时链；函数名串用 __PRETTY_FUNCTION__
        // 复现 ORIG `_ZZ...E19__PRETTY_FUNCTION__` 局部静态符号。
        cMyTrace(__PRETTY_FUNCTION__, 0xc26, 5)(
            "cannot write db uid(%d)\n", m_user->get_unique_id() & 0xffff);
        return false;
    }
    sender.Send();

    // b28d-solo-CreatureOnlyMgrRest [真语义 bug 修正 + P52]：ORIG 0x0833ae4e
    // 的 WriteCreatureLog 末实参读 0x14(%ebp)=mapSizeHint（旧码误传 param_5）；
    // 该值落具名槽 -0x3c，field1/addInfo2/addInfo 先物化 edi/esi/ebx 再 call
    // getCurCharacInvenR ⇒ receiver 单语句链式。
    reason = (eItemAddReason)mapSizeHint;
    m_user->getCurCharacInvenR()->WriteCreatureLog(
        (INVEN_TYPE)3, item->m_addInfo, item->m_addInfo2, 1,
        (int)item->m_field1, reason);
    return true;
