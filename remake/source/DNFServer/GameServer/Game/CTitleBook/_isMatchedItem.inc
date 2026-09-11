int CTitleBook::_isMatchedItem(
    int itemIdx, const std::vector<std::pair<int, int> >& list)
{
    // P13/P28：ORIG 先 call begin()（槽 -0xc）再 call end()（槽 -0x10），
    // 且循环头只比较缓存槽 ⇒ 同一 for-init 声明 it、e。
    for (std::vector<std::pair<int, int> >::const_iterator it = list.begin(),
             e = list.end();
         it != e; ++it)
    {
        if (it->first == itemIdx)
        {
            return it->second;
        }
    }
    return -1;
}