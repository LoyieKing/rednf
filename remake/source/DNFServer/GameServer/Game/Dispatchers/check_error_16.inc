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
