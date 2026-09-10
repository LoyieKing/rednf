bool CUser::SetCharacViewStateByCharacNo(int characNo, int state)
{
    // ORIG 0x0868??：prologue begin→-0xc、end→-0x10 双缓存（技巧 25/T-CU3）。
    std::vector<_Charac_info>::iterator it = m_characList.begin();
    std::vector<_Charac_info>::iterator itend = m_characList.end();
    for (; it != itend; ++it)
    {
        if (it->m_characNo == characNo)
        {
            m_characterView.enableSaveCharacView();
            it->m_viewState = state;
            return true;
        }
    }
    return false;
