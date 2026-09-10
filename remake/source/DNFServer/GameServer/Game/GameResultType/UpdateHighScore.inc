int GameResultType::UpdateHighScore(const GameResultType& other)
{
    int flags = 0;
    if (other.m_field40 > m_field40)
    {
        m_field40 = other.m_field40;
        flags |= 1;
    }
    if (other.m_field42 > m_field42)
    {
        m_field42 = other.m_field42;
        flags |= 2;
    }
    if (other.m_field48 > m_field48)
    {
        m_field48 = other.m_field48;
        flags |= 8;
    }
    // b28d-54 判词：ORIG 0x084b7588 段 `mov 0x44(%eax),%edx; cmp %eax,%edx; jae`
    //   ⇒ 无符号比较 + 单段装载；字段改 unsigned int（与 RHS 同型，P211 档③）。
    if (other.m_field44 < m_field44)
    {
        m_field44 = other.m_field44;
        flags |= 4;
    }
    return flags;
