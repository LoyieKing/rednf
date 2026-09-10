int CUser::GetSpecialLevelCharacCnt(int type)
{
    // ORIG 0x08656636 逐段：cnt=0(-0xc) → begin 临时(-0x14) → const 转换(-0x1c)
    //   → end 临时(-0x10) → const 转换(-0x20) → 循环只比一次缓存 end（P13）；
    //   m_characList 非 const ⇒ 两处 iterator→const_iterator 转换构造（P326 三级槽阶梯）。
    int cnt = 0;
    std::vector<_Charac_info>::const_iterator it = m_characList.begin();
    std::vector<_Charac_info>::const_iterator itEnd = m_characList.end();
    for (; it != itEnd; ++it)
    {
        if (it->m_level == type)
            ++cnt;
    }
    return cnt;
