int StackableAction::Action::proc_addFatigue(input_param& p)
{
    // ORIG 0x0827fbd6：user/item 判空→0x13；item+0x3a0 的 vector<int> 取第 0 项
    // 与当前疲劳比较；疲劳不足返回 0x5f；否则扣减并 SendFatigue。
    if (p.m_user == 0)
    {
        return 0x13;
    }
    // [b28d-solo-GlobalData] P0：ORIG 0x0827fbf0 `mov 0x8(%eax),%eax;
    // mov %eax,-0x10(%ebp)` 在 m_item 判空前即写具名局部，后续三次调用
    // （getCurCharacFatigue×2/setCurCharacFatigue/SendFatigue）全部
    // `mov -0x10(%ebp),%eax` 复用该槽（无重读 p.m_user）。
    CUser* user = p.m_user;
    if (p.m_item == 0)
    {
        return 0x13;
    }
    // ORIG item+0x3a0 = CStackableItem::m_vec3a0（运行时 item 为 stackable）。
    // [b28d-solo-GlobalData] P183 去指针缓存：ORIG 0x0827fc0d/0x0827fc2e 每次
    // 重发 `mov 0xc(%ebp),%eax; mov 0xc(%eax),%eax; add $0x3a0,%eax`，具名
    // costs 局部会占 -0x10 槽并把后续 max 临时槽顶浅（-0x14 vs ORIG -0x1c）。
    if (static_cast<CStackableItem*>(p.m_item)->m_vec3a0.empty())
    {
        return 4;
    }
    int need = std::max(
        0, static_cast<CStackableItem*>(p.m_item)->m_vec3a0.at(0));
    // ORIG 0x0827fc6a/0x0827fc89：getCurCharacFatigue 两次独立调用
    // （O0 无 CSE，P48；原具名 cur 缓存少 1 次调用，p167 MISSING 实证）。
    if ((int)(user->getCurCharacFatigue() & 0xffff) < need)
    {
        return 0x5f;
    }
    // [b28d-solo-GlobalData] ORIG 0x0827fc97/0x0827fc9e：实参序
    // `lea -0x14(lower),%eax; mov %eax,(%esp)`（第一实参 lower）⇒
    // `std::max(lower, after)`。余 1 处槽互换（ORIG lower -0x14 / max 常量临时
    // -0x1c；4.4 按首次使用分配恒相反，P212 头部声明/前置声明三形实测均反劣）
    // ⇒ N98/N93 封顶。[b28d-PoolGlobalData P326 重试] 匿名 0 临时形实测：块1
    // 临时顶到 -0x18、块2 after 滑 -0x1c（4 条→8 条差）⇒ 反劣回退，N93 判不变。
    int after = (int)(user->getCurCharacFatigue() & 0xffff) - need;
    int lower = 0;
    user->setCurCharacFatigue(std::max(lower, after));
    user->SendFatigue();
    return 0;
