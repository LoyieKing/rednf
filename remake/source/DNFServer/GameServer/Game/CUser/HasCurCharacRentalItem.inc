bool CUser::HasCurCharacRentalItem(int type) const
{
    // b28d-solo-CUser 试形（P198 指针侧孪生判据）：ORIG 0x086772d2 段为
    //   `mov %eax,-0x10(%ebp); cmpl $0x0,-0x10(%ebp); jne …`（结果先落槽再判空）
    //   ⇒ 原码「声明与赋值分两句」（初始化式形 4.4 折叠为寄存器直比）。
    // 2026-09-10：ORIG 0x08677413..0x0867741b 对 cur->m_characNo **落槽 -0x20 再判空**
    //   （P0 槽往返 ⇒ 原码具名 characNo 局部）；旧形内联 `*(const unsigned int*)cur`
    //   无槽且需把 cur 缓存进 %ebx（ORIG 无 push ebx）。
    // 2026-09-10 复测：拆两条独立守卫 + 具名 characNo 实测 -27/+26 → -38/+39（反劣，
    //   块布局被拆散）⇒ 维持合并短路形；残余为 N126 首槽基址常数（-0xc vs -0x10）。
    const _Charac_info* cur;
    cur = getCurCharacR();
    if (cur == 0 || *(const unsigned int*)cur == 0)
        return false;
    std::map<unsigned int, std::map<unsigned int,
        std::vector<InstanceRentalSystem::RentalInfo> > >::const_iterator outer =
        m_rentalInfo.find((unsigned int)type);
    if (outer == m_rentalInfo.end())
        return false;
    std::map<unsigned int,
        std::vector<InstanceRentalSystem::RentalInfo> >::const_iterator inner =
        outer->second.find(*(const unsigned int*)cur);
    // p167（2026-09-10）：EXTRA `_ZNKSt23_Rb_tree_const_iteratorI...EdeEv` ×1 而 ORIG 0
    //   ⇒ 原码写 `outer->` 非 `(*outer).`（P167 的 it-> 与 (*it) 判据）。
    if (inner == outer->second.end())
        return false;
    return !inner->second.empty();
