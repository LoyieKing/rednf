// ORIG 0x8352203：src 按相邻元素配对填充 RewardItemInfo{field0,field4}；
// 奇数元素时记录错误并返回
// b28d-20 AE：it++ 后置 + cMyTrace 全表达式临时链（83=83，槽位对齐；
// 残差仅 .o 重定位立即数）。
void makeRewardItemInfo(const INT_VECTOR& src,
                        RewardItemInfoVector& out)
{
    RewardItemInfo item;
    std::vector<int>::const_iterator it = src.begin();
    while (it != src.end())
    {
        item.m_itemIdx = *it;
        it++;
        if (it == src.end())
        {
            // ORIG 0x83522ce：cMyTrace 记录 "it == data.rewardSelectItems_.end()"
            cMyTrace(__PRETTY_FUNCTION__, 0x104d, 5)(
                "[%s][%d]_it == data.rewardSelectItems_.end()",
                __PRETTY_FUNCTION__, 0x104d);
            return;
        }
        item.m_count = *it;
        it++;
        out.push_back(item);
    }
}