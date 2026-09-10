bool ConditionLevelChkDungeon::isEventDungeon(int dungeonIdx)
{
    if (m_unableDungeons.empty())
    {
        return true;
    }
    std::vector<int>::iterator it = m_unableDungeons.begin();
    std::vector<int>::iterator endIt = m_unableDungeons.end();
    while (it != endIt)
    {
        // [b28c-31] ORIG 0x08913a2c：`cmp 0xc(%ebp),%eax; sete %al; test %al,%al;
        // je <++it>`（正极性 sete 物化）⇒ 正向 if 返回形，勿写 !(a==b)（4.4 折 setne）。
        if (*it == dungeonIdx)
        {
            return false;
        }
        ++it;
    }
    return true;
