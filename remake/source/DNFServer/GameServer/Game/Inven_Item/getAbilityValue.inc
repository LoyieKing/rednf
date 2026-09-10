unsigned short stAmplifyOption_t::getAbilityValue() const
{
    if (!hasAbility() || !isIdentified())
    {
        return 0;
    }
    return m_abilityValue;
