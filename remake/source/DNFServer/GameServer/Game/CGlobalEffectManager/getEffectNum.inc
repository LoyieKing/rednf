unsigned int CGlobalEffectManager::getEffectNum() const
{
    return static_cast<unsigned int>(
        m_activeEffects.size() + m_reservedEffects.size());
