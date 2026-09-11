bool CUser::setCharacName(int characNo, const char* name)   // ORIG W 0x084ecc7c：命中 true/未中 false
{
    // ORIG W 0x84ecc7c：prologue begin→-0x10、end→-0x14 双缓存；latch ++idx 先于
    // ++it（技巧 25 具名缓存消 end 重构）。
    int idx = 0;
    std::vector<_Charac_info>::iterator it = m_characList.begin();
    std::vector<_Charac_info>::iterator itend = m_characList.end();
    for (; it != itend; ++idx, ++it)
    {
        if (it->m_characNo == characNo)
        {
            strncpy(it->m_name, name, 0x1d);
            m_characterView.enableSaveCharacView();
            return true;
        }
    }
    return false;
}