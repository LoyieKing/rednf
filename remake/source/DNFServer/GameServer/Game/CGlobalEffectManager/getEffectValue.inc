// ORIG 0x084b7daa：遍历 m_activeEffects，累加所有 m_type==type 的 m_value。
float CGlobalEffectManager::getEffectValue(unsigned int type) const
{
    float sum = 0.0f;
    // [b28c-16] P13：end 缓存具名局部（ORIG sum=-0xc、it=-0x10、e=-0x14）。
    std::vector<CGlobalEffect>::const_iterator it = m_activeEffects.begin();
    std::vector<CGlobalEffect>::const_iterator e = m_activeEffects.end();
    for (; it != e; ++it)
    {
        if (it->getType() == type)
            sum += it->getValue();
    }
    return sum;
}