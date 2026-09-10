bool CCreatureMgr::RegisterCreatureItem(CCreatureItem* item, int uid)
{
    // ORIG 0x833a798：`test;setne %al;test;je` 查重物化（技巧 22）；true 块
    // uid→%esi（0x833a7a1 `mov 0x10(%ebp),%esi`）、get_unique_id 结果
    // `movzwl %ax,%ebx`（0x833a7b2）跨 ctor call 存活，trace 变参寄存器直传
    // PF 串 @0x8c2f140。
    // 【N96/P0 违例清零】历史 register asm("esi"/"ebx"/"al") 绑定形已删；
    // 自然局部形下 4.4 的 uid 槽往返/movzwl 二段/spill 中转均不可钉寄存器，
    // 残余为 N18a/N76 跨版本装载序。
    // [b28d-solo-CCreatureMgr P227/P194] ORIG 0x833a7a1 `mov 0x10(%ebp),%esi`
    // （uid 跨 ctor 保活）+ 0x833a7b2 `movzwl %ax,%ebx`：全表达式临时链令
    // 4.4 在 ctor 前求实参并以 callee-saved 保活（具名 uniqBx 反产槽预清+往返）。
    if (FindCreatureItem(uid) != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xbb4, 5)(
            "RegisterCreatureItem failed. uid(%d), key(%d)\n",
            (unsigned short)m_user->get_unique_id(), uid);
        return false;
    }
    m_creatureItems.insert(std::make_pair(uid, item));
    return true;
