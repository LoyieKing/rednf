// ORIG setLevel 0x084b395c：>70 截 70；<=0 归 1；其余原值。
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
}