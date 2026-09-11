// ORIG 0x89babc5：src 相邻元素配对填充 m_items，m_counts 每对补 1；奇数时 break
// 【N90】差值主体=libstdc++4.1 iterator/find 非 inline sret 外联（ORIG 8 insns/步 vs 4.4 inline 4）；豁免不逐形追。
// [b28d-N90] fresh 复验 97/85：维持 N90（同上，迭代器外联 + 帧差）。
void createLotteryInfo(LotteryInfo* info, std::vector<int>* source)
{
    info->clear();
    std::vector<int>::iterator it = source->begin();
    info->m_defaultIdx = *it;
    it++;
    info->m_defaultCount = 1;
    while (it != source->end())
    {
        std::pair<int, int> p;
        p.first = *it;
        it++;
        if (it == source->end())
            break;
        p.second = *it;
        it++;
        info->m_items.push_back(p);
        info->m_counts.push_back(1);
    }
}