// ---------------------------------------------------------------------------
// 30) CUser::check_locked_multibox_error(CItem const*, int slotCntHint, bool doubleMode)
//     报告：docs/class_func_reports/CUser/check_locked_multibox_error.md。ORIG 0x08675fd2。
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x08675fd2：p167=CLEAR，gap=4；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08675fd2：N82 封顶——槽基差 8（ORIG -0x59 vs
// 4.4 -0x61）+ 帧差（p167 CLEAR）。
int CUser::check_locked_multibox_error(const CItem* opener, int openedCnt, bool doubleMode)
{
    Inven_Item lockHost;                                   // GetInvenSlot 隐藏回填
    lockHost = getCurCharacInvenR()->GetInvenSlot(1, openedCnt);
    CItem* locked = G_CDataManager()->find_item(lockHost.m_addInfo);
    if (opener == 0 || locked == 0)
        return 0x11;
    // [H6 2026-09-11 负结论·已回滚] 试把四个独立早返并成单一复合条件
    // `if (!A || B || !C || D) return 0x11;`（欲复现 ORIG 0x08676128 的共享物化岛
    // `mov $0x1,%eax; jmp` / `mov $0x0,%eax` / test / `mov $0x11,%eax`）：4.4 直接产 jcc
    // 短路（不物化 bool 岛）⇒ our 148→**144**（gap 0→+4，我们反而少 4 条）劣化 ⇒ 回滚。
    if (!opener->is_stackable())
        return 0x11;
    if (opener->GetItemType() /*virt+0x0c*/ != 0x1a)
        return 0x11;
    if (!locked->is_stackable())
        return 0x11;
    if (locked->GetItemType() != 1)
        return 0x11;

    // ORIG 读 CItem+0x1ec/+0x1f0（运行时 opener 为 CStackableItem；
    // 偏移 = m_multiBox(+0x1e4).m_field8 / m_fieldc）
    // [P0/P183 去缓存] ORIG 08676142/0867614e 每次重读 0xc(%ebp) 直取 +0x1ec/+0x1f0，
    // 0867615a get_index 结果直接在 %eax 比较（无 itemIdx 槽）⇒ 旧稿 stackOpener(-0x20)
    // 与 itemIdx(-0x14) 两个具名局部共多占 8B，致 Inven_Item 槽 -0x61 vs ORIG -0x59 全帧错位。
    int wantSlot = static_cast<const CStackableItem*>(opener)->m_multiBox.m_field8;
    int needAddInfo = static_cast<const CStackableItem*>(opener)->m_multiBox.m_fieldc;
    if (!(locked->get_index() == wantSlot &&
          needAddInfo <= lockHost.get_add_info()))
        return 0x11;

    int slots = 2;
    if (doubleMode)
        // [H6 2026-09-11 负结论·已回滚] 试 `* 2` → `* slots`（欲复现 ORIG 0x086761ad/b0
        // `mov -0x10(%ebp),%edx; imul %edx,%eax` 的具名乘数）：4.4 产 `imul` 后**多 1 条**
        // （our 148→149）⇒ 劣化回滚。语义两者等价（调用点 slots==2）。
        slots = (int)G_CDataManager()->m_boosterGage.m_cMultiboxCount * 2;  // +0x877e = m_cMultiboxCount(+0x0e)
    int err = check_multibox_error(slots);
    return err == 0 ? 0 : err;
}