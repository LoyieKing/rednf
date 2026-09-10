bool CParty::SetRidableObjectState(int state, eRidableObjectState type)
{
    return reinterpret_cast<CBattle_Field*>(&m_battleField)
        ->SetRidableObjectState(state, type);
