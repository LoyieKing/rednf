int CCompound_ExtreamDun_Item::_convertInputItem(                   // ORIG 0x832c620
    ENUM_UNUSABLE_EQUIPMENT_TYPE eqType) const
{
    // ORIG 0x832c620：switch 降形态；case1/9 共享 body（fall-through 空 case）。
    switch ((int)eqType)
    {
    case 1:
    case 9:
        return 0;
    case 2:
        return 1;
    case 3:
        return 2;
    default:
        return 3;
    }
