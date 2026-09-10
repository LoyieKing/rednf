void CPrePayInfoMgr::InsertPrePayInfo(int characNo, PrePayInfo info)
{
    m_map.insert(std::make_pair(characNo, info));
