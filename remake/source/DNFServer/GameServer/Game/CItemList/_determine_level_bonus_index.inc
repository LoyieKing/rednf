unsigned int CCompound_ExtreamDun_Item::_determine_level_bonus_index(  // ORIG T 0x832cc02
    unsigned int baseLevel, ENUM_INPUTITEMS_RARITY inRarity,
    eBindSphereType bind) const
{
    if ((int)inRarity < 0 || (int)inRarity > 2)
    {
        return 0;
    }
    // ORIG while 形：size()!=0 && v[idx]<baseLevel 短路链，两调用每次
    // 全地址重算（P48 无引用缓存）
    // [b28d-solo] P231 不适用：ORIG 0x832cc02 的 idx 占槽 -0xc（每轮
    //   `mov -0xc(%ebp),%ebx` 重载）——register 形实测免槽（`mov $0,%ebx`）反劣。
    // [b28d-solo-CItemList] P225/循环旋转：ORIG 4.1 把短路链**第二项**
    //   (rates[idx] >= baseLevel，setae 物化) 落在 body、第一项 (size()!=0)
    //   落在尾条件；4.4 对合并 && 恒反向展开（++idx 落 body）。改
    //   while(size())+if-break 复现 ORIG 布局。
    unsigned int idx = 0;
    while (m_lvBonus[(int)bind].m_rates[(int)inRarity].size() != 0)
    {
        if (m_lvBonus[(int)bind].m_rates[(int)inRarity][idx] >= baseLevel)
        {
            break;
        }
        ++idx;
    }
    return idx;
