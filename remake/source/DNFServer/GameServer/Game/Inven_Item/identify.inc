bool stAmplifyOption_t::identify()
{
    if (isIdentified())
    {
        return false;
    }
    m_abilityType &= 0x7f;
    return true;
