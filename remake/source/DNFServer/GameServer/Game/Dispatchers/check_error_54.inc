int Dispatcher_ItemHyperLinkMessage::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081da514：两守卫**各自独立**（`cmpl $0,0xc; jne L1` / 第二段 bool 物化），
    // 合并 `||` 在 4.4 恒折成单一 je（P133）。
    if (user == 0)
    {
        return -1;
    }
    // ORIG 0x081da527：`call get_state; cmp $0x2,%eax; setle %al; test %al,%al; je`
    // —— 4.1 bool 物化后带分支消费者 ⇒ P225 `while (expr) return …` 形。
    while (user->get_state() <= 2)
    {
        return -1;
    }
    const MSG_ITEM_HYPER_LINK_MESSAGE& req =
        reinterpret_cast<const MSG_ITEM_HYPER_LINK_MESSAGE&>(msg);
    // ORIG 四判定各自重取 `mov -0xc(%ebp),%eax; movzbl 0xd(%eax),%eax; cmp $K,%al`
    // （P0：type 无槽往返）。
    return req.m_typeD == 0x24 || req.m_typeD == 0x25 ||
                   req.m_typeD == 0x21 || req.m_typeD == 7
               ? -1 : 0;
