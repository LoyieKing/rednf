// ===========================================================================
// FrocedCloseAssaults —— ORIG T 0x082ed5f4 (0x7d)
// 遍历全部场次逐一 EndAssault（非空指针才调）。
// ===========================================================================
void CAssaultMgr::FrocedCloseAssaults()
{
    std::map<unsigned int, CAssaultPlace*>::iterator it = m_places.begin();
    while (it != m_places.end())
    {
        CAssaultPlace* place = it->second;
        if (place != 0)
        {
            place->EndAssault();
        }
        ++it;
    }
}