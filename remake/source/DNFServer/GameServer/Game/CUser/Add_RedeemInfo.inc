// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086472c0 | _ZN5CUser14Add_RedeemInfoERK10Inven_Itemib
int CUser::Add_RedeemInfo(const Inven_Item& item, int count, bool isRedeem)
{
    // ORIG 0x086472c0：GetCharacExpandData(6)->isAddableFilter(item)，可加则记录历史并 AddRedeemList
    Redeem_Item::CRedeemItem* cdata =
        (Redeem_Item::CRedeemItem*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)6);
    // b28d-solo-CUser P171：ORIG 0x086472c0 call 后 `mov %eax,-0xc(%ebp)` 直接落槽、
    //   **无判空分支**（我方原形多 `cmpl $0,-0x10; jne; mov $0,%eax; jmp` 段）⇒ 删防御照抄。
    // b28d-solo-CUser p167 修复：ORIG 0x086472c0 调用点 `call; test %al,%al; je`（无 xor、
    //   无 setne）⇒ 正条件包裹（P59/P214①）；EXTRA `Inven_Item::get_add_info()` ×1 ⇒
    //   ORIG 直接读 +0x7 字段（`mov 0x7(%eax),%edx`），改具名成员 m_addInfo2（P235）。
    if (cdata->isAddableFilter(item))
    {
        // b28d-solo-CUser P171/P0：ORIG 0x086472c0 无 pItem 判空、且 find_item 结果
        //   无槽往返（我方原具名 pItem 落 -0xc 多 2 条）⇒ 内联表达式直用。
        if (G_CDataManager()->find_item(item.m_addInfo)->is_stackable())
            m_historyLog.RedeemItemAdd(item.m_addInfo, item.m_addInfo2);
        else
            m_historyLog.RedeemItemAdd(item.m_addInfo, 1);

        return cdata->AddRedeemList(item, count, isRedeem);
    }
    return 0;
}