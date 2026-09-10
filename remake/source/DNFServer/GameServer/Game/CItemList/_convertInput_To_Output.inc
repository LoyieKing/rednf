int CCompound_ExtreamDun_Item::_convertInput_To_Output(             // ORIG T 0x832c5f0
    ENUM_INPUTITEMS_RARITY inRarity) const
{
    // ORIG 0x832c5f0：switch 降形态；case0 体前置（test 判叶、jne 直落 default）。
    switch ((int)inRarity)
    {
    case 0: return 0;
    case 1: return 2;
    case 2: return 3;
    default: return 4;
    }
