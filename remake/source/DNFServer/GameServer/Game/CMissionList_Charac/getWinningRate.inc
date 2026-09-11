int CMissionList_Charac::getWinningRate() const
{
    unsigned short rate = (unsigned short)m_field14c;
    int count = 0;
    for (int i = 0; i < 10; ++i)
    {
        if (rate & 1)
        {
            ++count;
        }
        rate = (unsigned short)((int)(unsigned int)rate >> 1);
    }
    return count * 10;
}