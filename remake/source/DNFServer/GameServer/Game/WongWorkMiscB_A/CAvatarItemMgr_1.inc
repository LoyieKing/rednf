CAvatarItemMgr::CAvatarItemMgr(const CAvatarItemMgr& other)
    : m_registeredItems(other.m_registeredItems),
      m_waitAssignItems(other.m_waitAssignItems),
      m_tempKeyCounter(other.m_tempKeyCounter),
      m_emblemInfo(other.m_emblemInfo),
      m_emblemEnduranceMap(other.m_emblemEnduranceMap),
      m_enduranceUpdateMap(other.m_enduranceUpdateMap)
{
