// ============================================================================
// 6. Dispatcher_EnchantByBead::check_error
//    报告 docs/class_func_reports/Dispatcher_EnchantByBead/check_error.md
//    [b28d-solo-DispLinkA2 判词] verdict=IDENTICAL_AE（compare_tu clean：107/107；唯一残差
//    `mov &s_pSecuProtectionField,%eax` vs .o 未解析 R_386_32 显示 0x0，P264 归一口径）。
//    **真语义 bug（实参取错形参）**：原码 `reinterpret_cast<ParamSlotView_A2*>(&param)`
//    读 0x14(%ebp)=param；ORIG 0x081d238d 读 **0x10(%ebp)=msg**。铁证：ORIG process
//    0x081d2534 亦从 msg(0x10(%ebp)) 读 +0xd/0xe/0x10/0x12 四字段；本 TU 其它 check_error
//    注释同判（「ORIG mov 0x10(%ebp) 即 msg」）⇒ 已改 `&msg`。
//    其余修法：②删 static inline A2_paramShort helper（P49：-O0 static inline 产真实 call
//    `_ZL13A2_paramShortR9ParamBasei`）+ 具名 short 局部 → 槽位以 `(short)` 截断作普通实参
//    直传（P200：4.4 产 ORIG 的 `movzwl 0xe/0x12(%eax),%eax; movswl %ax,%ebx` 两段）；
//    ③删缓存 inven/dm，改链式单语句（P52：m_addInfo 先物化 %ebx 跨 G_CDataManager() 保活；
//    getCurCharacInvenW 每处重调，ORIG 两次独立 call）；④set 访问改 const
//    （p167：ORIG-only `std::set<int>::find(int const&) const`，我方原为非 const 版）
//    + 具名 const_iterator 令 find 先于 end 求值（ORIG sret -0x28/-0x20 序）。
// ============================================================================
unsigned int Dispatcher_EnchantByBead::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    if (user->get_state() <= 2)
        return 0x7fffffff;

    // CSecu_ProtectionField::Check(CUser*, SECURITY_PROTCTION)（ORIG 0x08288a02）
    int err = GlobalData::s_pSecuProtectionField->Check(
        user, static_cast<SECURITY_PROTCTION>(0x2b));
    if (err != 0)
        return err;

    // [b28d-solo-DispLinkA2] 真语义 bug（实参取错形参）：ORIG 0x081d238d 读的是
    // **0x10(%ebp)=msg**（同 TU 其它 check_error 注释「ORIG mov 0x10(%ebp) 即 msg」
    // 同判；铁证 = ORIG process 0x081d2534 亦从 msg 读 +0xd/0xe/0x10/0x12）。
    // msg 指针落 -0x18 具名槽，两处槽位以 (short) 截断后作普通实参直传 ⇒ 4.4 复现
    // ORIG 的 movzwl 0xe/0x12(%eax),%eax; movswl %ax,%ebx（P200 拓宽直传 + P52 保活）。
    ParamSlotView_A2* view = reinterpret_cast<ParamSlotView_A2*>(&msg);

    Inven_Item* item1 = user->getCurCharacInvenW()->GetInvenRef(   // 0x080da28e / 0x084fc2ae
        1, static_cast<short>(view->m_slot1));
    if (item1 == 0)
        return 0x11;

    Inven_Item* item2 = user->getCurCharacInvenW()->GetInvenRef(
        1, static_cast<short>(view->m_slot2));
    if (item2 == 0)
        return 0x11;

    // ORIG 0x081d240b：m_addInfo 先物化 %ebx、跨 G_CDataManager() 保活（P52 单语句链式）。
    CItem* pItem = G_CDataManager()->find_item(item1->m_addInfo);   // Inven_Item+0x02
    if (pItem == 0)
        return 0x11;

    // CItem 派生视图 +0x3d8 的可装配珠子类型集合（CStackableItem::m_set3d8）；
    // ORIG 每处重新取址（无引用局部槽），且 find 走 **const 版**
    // （p167：ORIG-only `set::find(int const&) const`）。
    // 具名 const_iterator 令 find 先求值（ORIG 先 find 后 end，sret -0x28/-0x20）。
    if (!static_cast<const CStackableItem*>(pItem)->m_set3d8.empty())
    {
        std::set<int>::const_iterator it =
            static_cast<const CStackableItem*>(pItem)->m_set3d8.find(
                item2->m_addInfo);
        if (it == static_cast<const CStackableItem*>(pItem)->m_set3d8.end())
            return 0x13;
    }
    return 0;
}