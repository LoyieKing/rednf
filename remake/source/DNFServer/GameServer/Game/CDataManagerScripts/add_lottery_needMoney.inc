void CLottery_NeedMoney::add_lottery_needMoney(unsigned int itemIdx, unsigned int cost)
{
    static std::map<unsigned int, unsigned int>::iterator _itr;  // ORIG _ZZN…add_lottery_needMoneyEjjE4_itr
    // ORIG 0x83650b2：end() 先于 find 调用（sret 槽 -0x28）；find 结果临时
    // -0x2c 落静态 _itr（0x83650d6-0xd9）；0x83650ec ne(this=_itr, rhs=&end)。
    // 命中 fall-through trace（cMyTraceNoop() 临时 -0x21 @0x83650f5-0x836510e，
    // 串 "already exist item index!!"@0x8c33253），je→0x8365115 insert。
    std::map<unsigned int, unsigned int>::iterator e = m_needMoneyMap.end();
    _itr = m_needMoneyMap.find(itemIdx);
    if (_itr != e)
    {
        cMyTraceNoop()("already exist item index!!");
    }
    else
        m_needMoneyMap.insert(std::make_pair(itemIdx, cost));
