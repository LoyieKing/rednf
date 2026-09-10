int CLottery_NeedMoney::isNeedMoney(unsigned int itemIdx) const
{
    static std::map<unsigned int, unsigned int>::const_iterator _itr;  // ORIG _ZZNK…isNeedMoneyEjE4_itr
    _itr = m_needMoneyMap.find(itemIdx);
    // ORIG 0x83651d2/0x83651e8：end() 临时(-0xc)为 ne 的 this、&静态 _itr 为
    // rhs ⇒ 源码方向 end() != _itr；命中 0x83651f1 ptEv 后 `mov 0x4(%eax),%eax`
    // 值直返（int 透传，无 bool 收窄），未命中 je→0x8365202 mov$0 尾岛。
    if (m_needMoneyMap.end() != _itr)
        return _itr->second;
    return 0;
