// ORIG T 0x08912a54。浮点读取：statWeightsByRarity[rarity]（this+(rarity+8)*4）
// 与 weaponWeights[a][b]（this+((6a+b)+12)*4+8）；分母 *(int*)this；useAlt 选择
// info.m_b / info.m_a；尾 +0.5f（DAT_08e122e8）后截断。
// [b28d-solo-CItemUpgrade_Separate] 重写：ORIG 为 result 具名 float（P232 扩展二段
// 零初始化）、altBias 运行时 flds/fmulp/fstps（pointFive/two 必须非 const 静态量，
// 否则 4.4 折 fld1）、if/else 两臂各自完整计算（非三目共享 multiplier）、
// 乘法书写序 m_b*(count/divisor+altBias)*weapon*stat 使 4.4 浮点栈序与 ORIG 逐条一致。
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

    result += 0.5f;
    return static_cast<int>(result);
}