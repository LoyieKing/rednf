bool CUser::IsRestrictedGoods(int itemIdx)
{
    if (itemIdx == 0)
        return false;
    // b28d-solo-CUser N封顶(N86/N89): ORIG 每轮重算 this+0x711ec 且 begin/end 各占独立
    //   sret 临时槽 -0xc/-0x10；4.4 缓存基址复用 -0x14 槽（非 const iterator 形实测
    //   ndiff 不变 20，回退）。
    // b28d-solo-CUser p167：ORIG 0x0864d878 用非 const `_List_iterator<int>`（我方
    // const_iterator 多转换构造 + 混合 operator!=）⇒ 改非 const iterator（P96/P50）。
    // b28d-solo-CUser 重写（2026-09-09，P13）：ORIG 0x0864d878 在循环头**缓存 end**
    //   （begin sret -0xc / end sret -0x10，条件只调 ne）；旧形每轮重调 end 且槽反。
    for (std::list<int>::iterator it = m_list711ec.begin(),
                                  e = m_list711ec.end();
         it != e; ++it)
    {
        if (*it == itemIdx)
            return true;
    }
    return false;
