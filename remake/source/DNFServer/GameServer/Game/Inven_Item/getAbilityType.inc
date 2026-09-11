unsigned char stAmplifyOption_t::getAbilityType() const
{
    if (!hasAbility())
    {
        return 0;
    }
    if (!isIdentified())
    {
        return 0x80;
    }
    return m_abilityType;
}