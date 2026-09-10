int QuestParameterScript::getRewardWeight(char level)
{
    // [b28c-L29] ORIG 0x08a6bd1c：find 结果直接落 it 槽（无 const_iterator
    // 转换构造调用），ne/ptEv 均为迭代器本型——用非 const iterator。
    std::map<char, int>::iterator it = m_map0.find(level);
    if (it != m_map0.end())
        return it->second;
    return 100;
}
