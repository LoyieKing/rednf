void PvpResultRecvFlag::Clear()
{
    for (int i = 0; i <= 7; ++i)
    {
        m_flag[i] = 0;
    }
}