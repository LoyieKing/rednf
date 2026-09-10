int CUser::_isMatchedItem(int itemIdx,
                          const std::vector<std::pair<int, int> >& list)
{
    // ORIG 0x867cf0c：prologue 单槽 it=-0xc（begin 缓存），循环 cond 无 end 重构
    //（技巧 25 具名缓存；声明序 begin 后占近槽）。
    std::vector<std::pair<int, int> >::const_iterator it = list.begin();
    std::vector<std::pair<int, int> >::const_iterator itend = list.end();
    for (; it != itend; ++it)
    {
        if (it->first == itemIdx)
            return it->second;
    }
    return -1;
