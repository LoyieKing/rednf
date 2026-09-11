// ORIG 0x83520c8：src 按相邻元素配对填充 std::pair<int,int>{first,second}；
// 奇数元素时 cMyTrace 记录 "[%s][%d]_it == data.rewardSelectItems_.end()"
// （0x1031 行，PRETTY __PRETTY_FUNCTION__）。
// 与 RewardItemInfo 版同 TU 相邻（0x83520c8/0x8352203），ORIG T 强符号，无调用点（导出）。
// b28d-20 AE：同上形（it++ + 全表达式临时链），83=83 槽位对齐。
void makeRewardItemInfo(const INT_VECTOR& src,
                        IntPairVector& out)
{
    std::pair<int, int> p;
    std::vector<int>::const_iterator it = src.begin();
    while (it != src.end())
    {
        p.first = *it;
        it++;
        if (it == src.end())
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1031, 5)(
                "[%s][%d]_it == data.rewardSelectItems_.end()",
                __PRETTY_FUNCTION__, 0x1031);
            return;
        }
        p.second = *it;
        it++;
        out.push_back(p);
    }
}