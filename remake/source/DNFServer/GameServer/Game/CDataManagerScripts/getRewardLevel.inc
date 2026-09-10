int QuestParameterScript::getRewardLevel(int level)
{
    int result = 0;
    std::map<int, int>::iterator it = m_map3c.begin();
    // ORIG 0x8a6be2a 调 ppEi（后缀 it++，0x8(%esp)=$0，返回值落废弃临时
    // -0x10），非前缀 ++it。
    for (; it != m_map3c.end(); it++)
    {
        if (it->second <= level)
        {
            result = it->first;
        }
    }
    return result;
