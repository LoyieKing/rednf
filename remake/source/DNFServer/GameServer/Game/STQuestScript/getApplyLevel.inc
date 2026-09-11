// [b28d-solo-STQuestScript · P212] ORIG 先构造 tmp（-0x38）再 call computeDayIndex
// ⇒ 头部声明序 tmp 在前（4.1/4.4 全头部声明同为先声明更深）；ORIG 0x08aaf8b6。
bool TrainingQuestScript::getApplyLevel(std::multiset<int>* out)
{
    if (m_selectProb.m_count <= 0 || m_selectProb.m_pairs.empty())
    {
        return false;
    }
    std::vector<int> tmp;
    int day = computeDayIndex();
    for (int i = 0; i < m_selectProb.m_count; ++i)
    {
        int val = get_random_fake_value(i, day);
        int acc = 0;
        for (std::vector<std::pair<int, int> >::iterator it =
                 m_selectProb.m_pairs.begin();
             it != m_selectProb.m_pairs.end(); ++it)
        {
            acc += it->second;
            if (val < acc)
            {
                tmp.push_back(it->first);
                break;
            }
        }
    }
    for (int level = 1; level <= 0x46; ++level)
    {
        for (std::vector<int>::iterator it = tmp.begin(); it != tmp.end(); ++it)
        {
            if (*it == -100)
            {
                out[level - 1].insert(*it);
            }
            // ORIG else 段 = `*it != -100 && *it + level >= 1` 短路链（cmp $-100;je
            // 跳过 + jle 跳过 + mov$1/mov$0/test 物化，P214①）。
            else if (*it != -100 && *it + level >= 1)
            {
                out[level - 1].insert(*it);
            }
        }
    }
    return true;
}