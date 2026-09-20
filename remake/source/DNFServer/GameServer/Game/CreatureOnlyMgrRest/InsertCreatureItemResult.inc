// ORIG _ZN13user_creature12CCreatureMgr24InsertCreatureItemResultEP17SIG_...
// T 0x0833b38c（InsertCreatureItemResult.md）：load 后 field64==6 广播列表；
// set/order 记录 ceraOrderNo(+0x60) 前后置/清零；WriteCreatureLog 五值直取 SIG
// （idx +0x8 / addInfo2 +0x30 / 类型 byte +0x5 / reason +0x51）。
// [b28d-solo-CreatureOnlyMgrRest] verdict=NEAR：删多余 memset（ORIG 仅
// Inven_Item::C1）+ P235 具名成员访问（m_reason/m_addInfo2/m_itemView.m_itemIdx/
// m_item[1] 复现 disp 折叠）+ P52 单语句链式 receiver。残差 = N82附（4.4 把
// Inven_Item 聚合放低槽 -0x59、enum reason 放浅槽 -0x1c；ORIG reason 最深
// -0x5c、localItem -0x55）+ N115 帧差；reason 提前/后置、ReasonBox 聚合化、
// reason[1] 数组化、char[3] 撑位四形实测不可翻（聚合/数组形多 2 条拷贝劣化）。
bool CCreatureMgr::InsertCreatureItemResult(SIG_CREATURE_ITEM* sig)
{
    Inven_Item localItem;
    // b28d-solo-CreatureOnlyMgrRest [ORIG 0x0833b395/N107]：ORIG 仅
    // `lea -0x55; call Inven_Item::C1`（清零由 ctor reset 完成，无独立
    // memset）；`lea 0x4(%eax),%edx` = 显式指针算术 `(char*)sig + 4`。
    sig->m_itemView.makeInvenItem(localItem);

    if (!LoadCreatureItem(&localItem,
                          reinterpret_cast<SIG_CREATURE_ITEM_DETAIL_INFO*>(&sig->m_slot)))
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
    // [L2Free4to15.S3] 试验记录：声明序交换（reason 前置）与块级声明两形
    // 均实测无效（ndiff 恒 10）⇒ 4.4「聚合对象优先低槽、标量分区排布」
    // （N82附）不可翻，保持原声明序。
    eItemAddReason reason = (eItemAddReason)(signed char)sig->m_reason;
    m_user->getCurCharacInvenR()->WriteCreatureLog(
        (INVEN_TYPE)3,
        sig->m_itemView.m_itemIdx,                            // +0x08
        sig->m_addInfo2,                                      // +0x30
        1,
        (int)sig->m_item[1],                                  // +0x05
        reason);
    m_user->set_cera_item_order_no(0);
    return true;
}