int CCompound_ExtreamDun_Item::_determine_rarity(                   // ORIG T 0x832c976
    unsigned int randVal, ENUM_INPUTITEMS_RARITY inRarity) const
{
    if ((int)inRarity < 0 || (int)inRarity > 2)
    {
        return 0;
    }
    // ORIG 0x832c9b7：i 为函数作用域，耗尽路径 `mov -0x4(%ebp),%eax` 返回 i(=4)。
    int i;
    for (i = 0; i <= 3; ++i)
    {
        if ((unsigned int)m_data.m_rarityDicision[(int)inRarity][i] >= randVal)
        {
            break;
        }
    }
    return i;
