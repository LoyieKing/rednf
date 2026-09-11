// ORIG 0x084b7d2c：遍历 m_activeEffects，任一 m_type==type 返回 true。
bool CGlobalEffectManager::isAffectedEffect(unsigned int type) const
{
    // [b28c-16] P13：ORIG begin/end 头部各求值一次（it=-0xc、e=-0x10），
    // 4.4 对 cond 内 end() 每次迭代重调 ⇒ 具名缓存 end。
    std::vector<CGlobalEffect>::const_iterator it = m_activeEffects.begin();
    std::vector<CGlobalEffect>::const_iterator e = m_activeEffects.end();
    for (; it != e; ++it)
    {
        if (it->getType() == type)
            return true;
    }
    return false;
}