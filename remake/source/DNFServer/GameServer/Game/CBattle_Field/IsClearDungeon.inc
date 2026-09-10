bool CBattle_Field::IsClearDungeon()
{
    if (!m_clearCondition.Empty())
    {
        if (m_clearCondition.IsClearCondition())
        {
            return true;
        }
        return false;
    }
    return check_end_point();
