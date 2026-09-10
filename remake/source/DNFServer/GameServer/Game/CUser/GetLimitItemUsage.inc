void CUser::GetLimitItemUsage(SIG_LOAD_ETC* sig) const
{
    if (sig == 0) return;
    sig->m_limitItemCount = 0;   // ORIG movl $0,0x2638
    // ORIG 0x08688dd5..0x08688e26：begin(-0x14)/end(-0x18) 先求值缓存（P13），
    //   随后**两条** `movl $0,-0xc`（P191 双初始化：声明 + for-init 各写一次）。
    std::vector<LimitItemUsage>::const_iterator it = m_limitItemUsage.begin();
    std::vector<LimitItemUsage>::const_iterator itEnd = m_limitItemUsage.end();
    int index = 0;
    // P157/P183：ORIG 每字段重算 (index+0x263)*0x10 并重调 operator->，禁 dst 缓存。
    // b28d-solo-CUser p167 修正（P28）：ORIG 增量段调 ppEi（后置）⇒ 写 it++。
    for (index = 0; it != itEnd; it++, ++index)
    {
        sig->m_limitItems[index].m_field8 = it->m_field15;   // ORIG mov 0x14(dst)
        sig->m_limitItems[index].m_fieldc = it->m_field19;   // ORIG mov 0x18(dst)
        sig->m_limitItems[index].m_itemIdx = it->m_fieldc;   // ORIG mov 0x10(dst)
        sig->m_limitItems[index].m_used = it->m_field10;     // ORIG movb 0xc(dst)
    }
    sig->m_limitItemCount = index;
