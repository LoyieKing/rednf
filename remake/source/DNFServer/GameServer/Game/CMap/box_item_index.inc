int CMap::box_item_index(std::list<std::pair<int, int> > itemList) const
{
    int threshold = get_rand_int(100) + 1;
    int accumulated = 0;
    // ORIG 0x0825?? P167：非 const iterator（P96）+ 循环外默认构造 pair（C1Ev）
    // + 体内临时 pair 右值赋值（deEv + aSEOS0_，P135）+ 后置 it++（ppEi，P28）。
    std::pair<int, int> item;
    for (std::list<std::pair<int, int> >::iterator it = itemList.begin();
         it != itemList.end(); it++)
    {
        item = std::pair<int, int>(*it);
        accumulated += item.second;
        if (threshold < accumulated)
            return item.first;
    }
    return -2;
