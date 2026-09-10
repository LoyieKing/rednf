int CCompound_ExtreamDun_Item::_convertOutputItem(                  // ORIG 0x832c5ac
    ENUM_UNUSABLE_EQUIPMENT_TYPE eqType) const
{
    // ORIG 0x832c5ac：switch 降形态（pivot=2 树，case9 独立体在右叶之后）。
    switch ((int)eqType)
    {
    case 9:
        return 0;
    case 1:
        return 1;
    case 2:
        return 2;
    case 3:
        return 3;
    default:
        return 4;
    }
