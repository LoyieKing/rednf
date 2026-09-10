bool CCreatureMgr::AuctionRegist(Inven_Item* item, int param_2, int param_3)
{
    if (!IsCreatureEquipmentScope(param_2))
        return true;
    if (item->m_field1 != 5)
        return true;

    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, m_user);
    int mapSize = (int)m_creatureItems.size();
    // b28d-solo-CreatureOnlyMgrRest [P0/P227]：ORIG 0x0833b110 的
    // getCurCharacNo 结果无槽（直接 0x8(%esp)）⇒ 内联实参；失败 trace 的
    // uid 在 ctor call 前求值并入 %ebx（P227 全表达式临时链）。
    if (!sender.AuctionRegist(item, m_user->getCurCharacNo(), param_2, param_3,
                              mapSize))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xc5c, 5)(
            "cannot write db uid(%d)\n", m_user->get_unique_id() & 0xffff);
        return false;
    }
    sender.Send();
    // b28d-solo-CreatureOnlyMgrRest [真语义 bug 修正]：ORIG 0x0833b1a4 第二实参
    // 读 0x14(%ebp)=param_3（旧码误传两次 item->m_addInfo2）；返回归一化形
    // `xor $1; test; je <mov $1>` = `if(!call()) return false; return true;`。
    if (!DeleteCreatureItem(static_cast<int>(item->m_addInfo2), param_3))
        return false;
    return true;
