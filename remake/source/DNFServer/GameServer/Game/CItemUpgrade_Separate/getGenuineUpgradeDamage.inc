int GenuineDamageUpgradeTable::getGenuineUpgradeDamage(ENUM_RARITY rarity, int a, int b,
                                                       int key, int count, bool useAlt)
{
    static float pointFive = 0.5f;   // ORIG .data 093f4fb0（非 const ⇒ 4.4 不折 fld1）
    static float two = 2.0f;         // ORIG .data 093f4fb4
    float result = 0.0f;
    const float altBias = two * pointFive;

    if (key == 0)
    {
        return 0;
    }

    // P48 no-CSE：ORIG 两臂各 fresh m_infos[key]（operator[] 共 2 次），且各臂
    // 完整展开乘链（非三目共享）。
    if (useAlt)
    {
        result = static_cast<float>(m_infos[key].m_b) *
                 (static_cast<float>(count) / static_cast<float>(m_divisor) + altBias) *
                 m_weaponWeights[a][b] *
                 m_statWeightsByRarity[static_cast<int>(rarity)];
    }
    else
    {
        result = static_cast<float>(m_infos[key].m_a) *
                 (static_cast<float>(count) / static_cast<float>(m_divisor) + altBias) *
                 m_weaponWeights[a][b] *
                 m_statWeightsByRarity[static_cast<int>(rarity)];
    }

    result += kRoundHalf;
    return static_cast<int>(result);
