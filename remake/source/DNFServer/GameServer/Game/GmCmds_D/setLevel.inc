void CLevelUp::setLevel(int level)
{
    if (!(level < 71))                                       // cmpl $0x46; jle
    {
        m_level = 70;
    }
    else if (level < 1)
    {
        m_level = 1;
    }
    else
    {
        m_level = level;
    }
