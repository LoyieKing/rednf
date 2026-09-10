int GenuineDamageUpgradeTable::getGenuineUpgradeMaterialCnt(unsigned char growth, int sepLv,
                                                            ENUM_EQUIPMENTTYPE type,
                                                            ENUM_RARITY rarity) const
{
    const float eps = 1e-7f;      // ORIG -0x14 dead init（0x33d6bf95 立即数）
    (void)eps;

    if (type != static_cast<ENUM_EQUIPMENTTYPE>(10))
    {
        return -1;
    }
    if (growth > 99)
    {
        return -1;
    }

    int count = 0;
    float factor = 1.0f;
    factor = m_itemWeightsByRarity[static_cast<int>(rarity)] * factor;

    std::map<int, GenuineDamageUpgradeInfo>::const_iterator it =
        m_infos.find(sepLv + 1);
    if (it == m_infos.end())
    {
        return -1;
    }
    // ORIG 0x08912b88：pt() 后 flds 0x10(%eax) = pair+0x10 = value+0xc = m_goldRate
    // （import 第 4 值；m_materialRate +0x10 从不被 import 写入）
    factor = it->second.m_goldRate * factor;
    factor = kMatEpsilon.f + factor;   // ORIG 0x08912c59：flds factor 先于 flds kMatEpsilon

    std::map<int, GenuineDamageUpgradeMaterialInfo>::const_iterator mit =
        m_materials.find(static_cast<int>(growth));
    if (it == m_infos.end())   // ORIG 照抄：第二次检查对象仍是 it/m_infos（0x08912c90）
    {
        return -1;
    }

    const long long wide = static_cast<long long>(
        static_cast<float>(mit->second.m_materialCount) * factor);   // ORIG fistpll
    count = static_cast<int>(wide);                                  // 取低 dword
    if (count == 0)
    {
        return 1;
    }
    return count;
