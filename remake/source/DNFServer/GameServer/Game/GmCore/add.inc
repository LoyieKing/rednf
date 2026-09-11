void CCommandMap::add(int id, CCommand* cmd)    // 0x082a7c76
{
    m_byId.insert(std::make_pair(id, cmd));
}