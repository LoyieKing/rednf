void CHades::UseFatigue(unsigned int count)
{
    (void)count;
    needLogin();
    ++m_useFatigueCnt;
    if (m_useFatigueCnt > 1)
    {
        StdCateUpdate(1, 0);
        m_useFatigueCnt = 0;
    }
    else
    {
        return;
    }
