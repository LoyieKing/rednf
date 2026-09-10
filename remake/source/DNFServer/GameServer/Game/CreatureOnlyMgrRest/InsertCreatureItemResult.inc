bool CCreatureMgr::InsertCreatureItemResult(SIG_CREATURE_ITEM* sig)
{
    Inven_Item localItem;
    eItemAddReason reason;
    // b28d-solo-CreatureOnlyMgrRest [ORIG 0x0833b395/N107]：ORIG 仅
    // `lea -0x55; call Inven_Item::C1`（清零由 ctor reset 完成，无独立
    // memset）；`lea 0x4(%eax),%edx` = 显式指针算术 `(char*)sig + 4`。
    reinterpret_cast<Inven_Item_Compatible*>((char*)sig + 4)->makeInvenItem(localItem);

    if (!LoadCreatureItem(&localItem,
                          reinterpret_cast<SIG_CREATURE_ITEM_DETAIL_INFO*>(
                              (char*)sig + 0x2c)))
        return false;

    if (sig->m_field64 == 6)
        SendCreatureItemInfo(sig);

    m_user->set_cera_item_order_no((unsigned int)sig->m_ceraOrderNo);   // +0x60

    // b28d-solo-CreatureOnlyMgrRest [P52/P0]：ORIG 0x0833b417 先把 reason
    // 落具名槽 -0x5c、field1/addInfo2/addInfo 物化 edi/esi/ebx，之后才 call
    // getCurCharacInvenR ⇒ receiver 必须写成单语句链式，inv 不得具名。
    // b28d-solo-CreatureOnlyMgrRest [P235/P211]：ORIG 0x0833b417 全部为
    // disp 折叠成员访问（`movzbl 0x51(%eax)` / `mov 0x30(%eax),%esi` /
    // `mov 0x8(%eax),%eax` / `movzbl 0x5(%eax)`）⇒ 用具名字段而非指针算术。
    reason = (eItemAddReason)(signed char)sig->m_reason;
    m_user->getCurCharacInvenR()->WriteCreatureLog(
        (INVEN_TYPE)3,
        sig->m_itemView.m_itemIdx,                            // +0x08
        sig->m_addInfo2,                                      // +0x30
        1,
        (int)sig->m_item[1],                                  // +0x05
        reason);
    m_user->set_cera_item_order_no(0);
    return true;
