bool CBattle_Field::ClearCondition(ENUM_DUNGEON_CLEAR_CONDITION_TYPE type, int idx)
{
    if (!m_clearCondition.Empty())
    {
        if (m_clearCondition.ClearCondition(type, idx))
        {
            return true;
        }
    }
    return false;
