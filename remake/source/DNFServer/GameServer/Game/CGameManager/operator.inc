bool CGameManager::FindUserByUnique::operator()(
    std::pair<const unsigned int, CUser*>& p)
{
    if (p.second != 0 &&
        (unsigned short)p.second->get_unique_id() == (short)m_uniqueId)
        return true;
    return false;
}