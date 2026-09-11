int StackableAction::Action::proc_lotteryCount(input_param& p)
{
    // ORIG 0x0827fb86：user/item 判空→0x13；否则 giveLotteryCountItem(
    // item->get_index(), false)，返回 0。
    // 两 guard 共享 0x13 尾块（|| 合并条件）。
    // 两个独立 if + 相同返回值 ⇒ gcc tail-merge 共享 0x13 尾块。
    // ORIG 0x0827fb86：正向嵌套（两 je 共享尾部 0x13 块），body 内 return 0
    // 走 jmp merge。
    if (p.m_user != 0)
    {
        if (p.m_item != 0)
        {
            p.m_user->giveLotteryCountItem(p.m_item->get_index(), false);
            return 0;
        }
    }
    return 0x13;
}