bool CBattle_Field::setBloodState(ENUM_BLOOD_STATE state)
{
    if (state <= m_bloodState2)
        return false;
    m_bloodState2 = state;
    return true;
