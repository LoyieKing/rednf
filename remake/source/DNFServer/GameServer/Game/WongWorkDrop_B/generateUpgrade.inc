// ---- generateUpgrade — ORIG T 0x08533676 ----------------------------------
int GenerateRandomUpgrade::generateUpgrade(ENUM_RARITY rarity, int levelDelta)
{
    // [M3FreeNext15B 复验·回滚] ItemFarmingData.h 拷贝 ctor 加 `throw()` 的 **A/B 实测**
    // （父泳道 P340 真基线反演：字面量反转重编取 base .o，base/after 在该符号区间
    // **逐指令完全相同** —— insns 233=233、`_Unwind_Resume` **1=1**）⇒ 本 LP **并非**
    // 源自拷贝 ctor 的具/不可见，而是**本函数内具名局部 `entry` 的 dtor cleanup**
    // （函数体内其余调用可抛）；ORIG 无 LP 属 4.1 侧判定差，4.4 -O0 无合法源码解。
    // ⇒ throw() 按闸门（无 gap/ndiff 改善）已回滚，.h 恢复原状；旧 EH 普查判读更正。
    // @ORIG 0x08533676
    // P183/P96：ORIG find(+0x00d) 与 end(+0x031) 各自重取 G_CDataManager()+0x528
    // （无 dm/farming 具名缓存）；迭代器走非 const 版 `_Rb_tree_iterator`（非 const_iterator）。
    FarmingMap::iterator it =
        G_CDataManager()->m_serverParameter.m_farming4c0.find(
            static_cast<int>(rarity));
    if (it == G_CDataManager()->m_serverParameter.m_farming4c0.end())
        return 0;
    if (it->second.m_active == 0)              // pair.second + ItemFarmingData+0xc
        return 0;

    // P167/§4.1：ORIG 直接 G_CDataManager()+0x540 取 rates，无 helper。
    // ORIG 081b48ac = vector::size()（+0x0af setne 正检、+0x0c2 `sub $1`），
    // operator[] 走非 const 版 ⇒ rates 去 const、`empty()`→`size()!=0`（P197④）。
    std::vector<std::pair<int, float> >* rates =
        &G_CDataManager()->m_serverParameter.m_vec4d8;
    float factor = 1.0f;
    if (rates->size() != 0)
    {
        // P167/§4.1：内联三元+循环查表（无 lookupGradeRatio helper）
        int last = static_cast<int>(rates->size()) - 1;
        if ((*rates)[0].first < levelDelta)
            factor = (*rates)[0].second;
        else if ((*rates)[last].first > levelDelta)
            factor = (*rates)[last].second;
        else
        {
            for (int i = 0; i <= last; ++i)
            {
                if ((*rates)[i].first == levelDelta)
                    factor = (*rates)[i].second;
            }
        }
    }

    ItemFarmingData entry = it->second;         // ORIG ItemFarmingData copy ctor

    int weightOld = entry.m_vecWeight[0].second;
    entry.m_vecWeight[0].second = static_cast<int>(static_cast<float>(weightOld) * factor);
    entry.m_active = entry.m_active - weightOld + entry.m_vecWeight[0].second;

    int roll = m_rand->randInt(static_cast<const unsigned long&>(entry.m_active));

    int acc = 0;
    for (size_t i = 0; i < entry.m_vecWeight.size(); ++i)
    {
        acc += entry.m_vecWeight[i].second;
        if (roll <= acc)
            return entry.m_vecWeight[i].first;
    }
    return 0;                                // ORIG 未命中返回 0
}