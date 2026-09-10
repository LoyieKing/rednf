void stAmplifyOption_t::_setAbility(unsigned char type, unsigned short value,
                                    bool identified)
{
    m_abilityType = type;
    m_abilityValue = value;
    if (!identified)
    {
        m_abilityType |= 0x80;
    }
