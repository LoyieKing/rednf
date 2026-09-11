// ORIG size 0x856270c（const）：统计占用槽位数。
int TeamInfoThird::size() const
{
    int count = 0;
    for (int i = 0; i < 4; ++i)
    {
        if (m_players[i].m_flag != 0)
            ++count;
    }
    return count;
}