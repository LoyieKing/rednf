void DisPatcher_SortItem::send(CUser* user, ParamBase& param)

{
    stOnSortItemParam& p = static_cast<stOnSortItemParam&>(param);
    PacketGuard guard;
    guard.put_header(1, 0x14);
    if (p.m_result == 0)
    {
        guard.put_byte(1);
        guard.put_byte((char)p.m_invenType);
    }
    else
    {
        guard.put_byte(0);
        guard.put_byte((char)p.m_result);
        guard.put_byte((char)p.m_invenType);
    }
    guard.finalize(true);
    user->Send(guard);
    if (p.m_result == 0)
    {
        user->send_itemspace((int)(char)p.m_invenType);
    }
    if (p.m_result == 0)
    {
        // [b28d-29] ORIG 0x081c9ec6 段为稀疏 switch 决策树（cmp$1/je case1 →
        // cmp$1/jg L2 → test/je case0 → jmp default；L2: cmp$2/je → cmp$7/je），
        // 非 if-else 链（P17/P251/P261：无状态槽 cmp 链 ⇒ 源形是 switch）。
        switch ((int)(char)p.m_invenType)
        {
        case 0:
            user->getCurCharacInvenR()->SendItemLockListInven();
            break;
        case 1:
            user->getCurCharacInvenR()->SendItemLockListAvatar();
            break;
        case 2:
            user->getCurCharacCargoR()->SendItemLockList();
            break;
        case 7:
            user->getCurCharacInvenR()->SendItemLockListCreature();
            break;
        default:
            break;
        }
    }
}
