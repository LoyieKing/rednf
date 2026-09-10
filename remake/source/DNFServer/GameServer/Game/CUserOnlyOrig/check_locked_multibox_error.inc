int CUser::check_locked_multibox_error(const CItem* opener, int openedCnt, bool doubleMode)
{
    Inven_Item lockHost;                                   // GetInvenSlot 隐藏回填
    lockHost = getCurCharacInvenR()->GetInvenSlot(1, openedCnt);
    CItem* locked = G_CDataManager()->find_item(lockHost.m_addInfo);
    if (opener == 0 || locked == 0)
        return 0x11;
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
        slots = (int)G_CDataManager()->m_boosterGage.m_cMultiboxCount * 2;  // +0x877e = m_boosterGage.m_cMultiboxCount(+0x0e)
    int err = check_multibox_error(slots);
    return err == 0 ? 0 : err;
