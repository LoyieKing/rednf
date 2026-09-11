CUserGlobalInfoHandle::CUserGlobalInfoHandle()
{
    m_next = 1;
    for (int i = 0; i <= 0xfffe; ++i)
    {
        m_used[i] = 0;
    }
}