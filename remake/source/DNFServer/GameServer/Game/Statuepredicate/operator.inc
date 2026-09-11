bool Statuepredicate::operator()(pieceQuestRewardStatus const& status) const
{
    if (m_appendageType == status.m_appendageType &&
        m_valueType == status.m_valueType)
        return true;
    return false;
}