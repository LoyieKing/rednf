int CMap::box_item_index(std::list<std::pair<int, int> > itemList) const
{
    int threshold = get_rand_int(100) + 1;
    int accumulated = 0;
    // ORIG 0x0825?? P167：非 const iterator（P96）+ 循环外默认构造 pair（C1Ev）
    // + 体内临时 pair 右值赋值（deEv + aSEOS0_，P135）+ 后置 it++（ppEi，P28）。
    // [L6S6b 2026-09-11] ORIG 0x0834ebf9 begin() 在前、0x0834ec0e pair C1Ev 在后，
    //   且 it 槽 -0x24 **浅于** pair 槽 -0x2c（我方旧形 pair=-0x28/it=-0x2c 恰反向）
    //   ⇒ 声明序必须是 iterator 先、pair 后（P28/P212），for 只留条件与步进。
    std::list<std::pair<int, int> >::iterator it = itemList.begin();
    std::pair<int, int> item;
    for (; it != itemList.end(); it++)
    {
        item = std::pair<int, int>(*it);
        accumulated += item.second;
        if (threshold < accumulated)
            return item.first;
    }
    return -2;
}