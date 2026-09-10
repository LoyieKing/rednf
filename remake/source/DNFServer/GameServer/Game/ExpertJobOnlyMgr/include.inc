bool CDisjointMachine::include(const village_object::IObject& other) const
{
    return m_rect.include(
        reinterpret_cast<const CDisjointMachine&>(other).m_rect);
