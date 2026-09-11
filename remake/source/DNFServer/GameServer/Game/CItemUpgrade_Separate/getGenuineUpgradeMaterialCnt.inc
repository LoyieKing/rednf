// ORIG T(_ZNK…) 0x08912b88。
// [b28d-solo-CItemUpgrade_Separate] 重写：eps 死局部（-0x14，P232 扩展 float 二段
// 常量初始化）、count 具名预置 0（-0x10）、factor 1.0f 两句式（1.0f 落槽不被折）、
// 乘加拆两句（ORIG 不融合 fmuls/fadds）、第二次 end 检查照抄 ORIG 的 it==m_infos.end()
// 重复形（ORIG 0x08912c90 lea 0x158 实证；mit 无 end 检查，缺键时 ORIG/我方同为
// 越界解引用，行为一致）。p167 8=8 CLEAR。判词 N（N95/N82 + N92/N18a 族）：残余 =
// ①4.4 浅槽基恒比 ORIG 深 8（eps/count/factor -0x1c/-0x18/-0x14 vs -0x14/-0x10/-0xc；
// 帧 0x58 vs 0x68，加 char[0x10]/double pad 只会整体平移 0x10，N95 不可上移）；
// ②ORIG `mov -0x50,%eax; mov -0x4c,%edx` 的 64 位宽读 vs 4.4 只读低 dword（N92）。
// [2026-09-09 SweepCItemUpgrade] 复核（未改）：gap=1（orig 217 / our 216）即 ② 那 1 条
// 高 32 位读（0x08912cf8，fistpll 结果丢弃的高半字）；槽基常数差 8 为 N95。判词维持 N。
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
    factor = 1e-7f + factor;   // ORIG 0x08912c59：flds factor 先于 flds <1e-7f 匿名常量>

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
}