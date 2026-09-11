// ORIG _ZN13user_creature12CCreatureMgr13AuctionRegistEP10Inven_Itemii
// T 0x0833b09c（AuctionRegist.md）：sender.AuctionRegist(item,characNo,p2,p3,size)
// 成功 → Send() + DeleteCreatureItem(addInfo2)；失败 trace 0xc5c。
// [b28d-solo-CreatureOnlyMgrRest] verdict=IDENTICAL/AE：**真语义 bug 修正**
// （DeleteCreatureItem 第 2 实参 ORIG 读 0x14(%ebp)=param_3，旧码误传两次
// item->m_addInfo2）+ P0/P227 内联 characNo 与临时链 trace + 返回归一化
// `if(!call()) return false; return true;`。
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
}