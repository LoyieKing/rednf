int AdvanceAltarShopParameter::getMaxStarResetGoldFee()
{
    // b28d-solo-advancealtar_misc：ORIG @0x088a2bc0 用**非 const** `_Rb_tree_iterator`
    //   （Ghidra C 实证 begin/end/ne/operator-> 均为非 K 版本）⇒ 原 const_iterator
    //   触发 iterator→const_iterator 转换构造/析构，帧 0x38、end 临时 -0x1c（P96/P270）。
    //   改 iterator 后帧/槽与 ORIG 一致（maxKey -0x10、it -0x14、end 临时 -0xc）。
    //   增量段 ORIG 为 `ppEv`（**前置** ++，无 sret 临时）⇒ 源保持 `++it`。
    int maxKey = 0;
    for (std::map<int, _StarRestFeeByGold>::iterator it = m_starRestMap.begin();
         it != m_starRestMap.end(); ++it)
    {
        if (maxKey < it->first)
        {
            maxKey = it->first;
        }
    }
    return m_starRestMap[maxKey].m_field0;
