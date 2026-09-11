// process 依赖（清单外）：state!=3→7；count(msg+0x10)!=1→0x16；
// GetInvenSlot(EQUIP,slot) 后 find_item 缺失→0x11、持有数不足→0x16；
// 非 0x12ca 物品→0x17；0x12ca 时摆摊忙→0x3c、时光门开→0x16，否则 0。
// （声明需入 Dispatchers.h，见 NEEDS_DECL）
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081e329e orig/ours=78/84 ndiff-blocks=10
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x10(%ebp),%eax | mov    %eax,-0x10(%ebp) | movzwl 0x10(%eax),%eax
// [b28d-solo-Dispatchers] P211 callee 侧：返回类型 int→unsigned int（头声明同步，
//   mangled 不变），使 process 调用点 store 翻为 ORIG 的 RHS-first+edx 中转。
unsigned int Dispatcher_CollectItems::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081e329e：msg 槽只有一个（-0x10），每个字段用点都是
    // `mov -0x10(%ebp),%eax; movzwl 0xe|0x10(%eax),%eax`（**带 disp 的现址读**）。
    // 具名引用别名会让 4.4 在函数头为 `&req.m_itemIdx`/`&req.m_count` 各占一个
    // 指针槽（-0x14/-0x10）并退化成 `movzwl (%eax)`（帧 +8、全槽平移）⇒ 删别名。
    // 字段名保留注释：m_itemIdx=+0xe word / m_count=+0x10 word。
    //
    // ORIG 序言先 `call get_state` 再 `mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)`
    // ⇒ msg 引用声明必须落在 get_state 判定**之后**（技巧13：声明位=落栈时机）。
    if (user->get_state() != 3)
    {
        return 7;
    }
    MSG_COLLECT_ITEMS& req = reinterpret_cast<MSG_COLLECT_ITEMS&>(msg);
    if ((unsigned short)req.m_count != 1)
    {
        return 0x16;
    }
    Inven_Item slotItem =
        user->getCurCharacInvenR()->GetInvenSlot(1, (int)req.m_itemIdx);
    CItem* item = G_CDataManager()->find_item(slotItem.m_addInfo);
    if (item == 0)
    {
        return 0x11;
    }
    if (slotItem.m_addInfo2 < (int)(unsigned int)req.m_count)
    {
        return 0x16;
    }
    if (slotItem.m_addInfo != 0x12ca)
    {
        return 0x17;
    }
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
    {
        return 0x3c;
    }
    if (GlobalData::s_timeGate_->isOpen())
    {
        return 0x16;
    }
    return 0;
}