// ORIG 0x084b7c50：m_reservedEffects 中 m_type==type 者移到 m_activeEffects 并擦除。
void CGlobalEffectManager::beginReservedEffect(unsigned int type)
{
    if (m_reservedEffects.empty())
        return;

    for (std::vector<CGlobalEffect>::iterator it = m_reservedEffects.begin();
         it != m_reservedEffects.end(); )
    {
        if (it->getType() == type)
        {
            m_activeEffects.push_back(*it);
            // b28d-50：ORIG 0x084b7cd4 erase 的 sret 落 -0xc 槽后**不回收**（直接
            // jmp 到循环条件），即原码丢弃返回值；写 `it = erase(it)` 会多一条
            // `mov -0x1c,%eax; mov %eax,-0x10` 回写并使帧 0x28→0x38。vector::erase
            // 返回的迭代器与入参 it 同址（元素左移），丢弃语义等价（P171 照抄）。
            m_reservedEffects.erase(it);
        }
        else
        {
            ++it;
        }
    }
    notifyGlobalEffectInfo();
}