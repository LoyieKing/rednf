void* CDataManager::GetExpertJobEtcScript()
{
    // ORIG 0x0849f516 W：调 CExpertJobList::get_etc()（W 0x0849f50a，返回 this+0x18
    // = &m_etc）。CUser 侧将其作 std::map<uchar,short>（等级→所需角色等级）读取。
    return m_regeneration.m_expertJobList->get_etc();
