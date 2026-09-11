int RandomOptionScript::getOptionModificationCost(short level, ENUM_RARITY rarity) const
{
    for (std::vector<OPTION_MODIFICATION_COST>::const_iterator it =
             m_costVec10c.begin();
         it != m_costVec10c.end(); ++it)
    {
        if (level <= (*it).m_level)   // ORIG 用 operator*（de）
        {
            if (rarity == RARITY_RARE)
            {
                return (*it).m_cost2;
            }
            if (rarity == RARITY_UNIQUE)
            {
                return (*it).m_cost3;
            }
        }
    }
    return -1;
}