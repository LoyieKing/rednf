int STExpertJobScript::GetUpgradeCost(unsigned int index)
{
    // P0：ORIG `call size; cmp index,%eax; setbe` 无槽、帧 0x18；具名 n 局部致帧 0x28。
    if (index >= m_upgradeCost.size())
        return 0;
    return m_upgradeCost.at(index);
