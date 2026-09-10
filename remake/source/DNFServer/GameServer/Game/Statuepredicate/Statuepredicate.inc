Statuepredicate::Statuepredicate(pieceQuestRewardStatus status)
    : std::unary_function<pieceQuestRewardStatus, bool>(),
      m_appendageType(status.m_appendageType),
      m_valueType(status.m_valueType),
      m_value(status.m_value)
{
