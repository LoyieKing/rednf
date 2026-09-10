void CUser::DisableCharacInfo(CACHE_SECTION section)
{
    m_pad8d0fd[static_cast<int>(section)] = 0;
