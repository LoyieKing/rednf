bool ChattingEmoticon_byPremium::isUsable(const CUser& user) const
{
    for (std::set<int>::const_iterator it = m_set.begin();
         it != m_set.end(); ++it)
    {
        if (user.isAffectedPremium((ENUM_PREMIUM_TYPE)*it))
            return 1;
    }
    return 0;
