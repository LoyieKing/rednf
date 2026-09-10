bool _checkItemMovable(const CUser* user, ENUM_ITEMSPACE fromSpace, int fromSlot,
                       unsigned long fromItemIdx, ENUM_ITEMSPACE toSpace, int toSlot,
                       unsigned long toItemIdx)
{
    if (fromSpace == (ENUM_ITEMSPACE)2 || toSpace == (ENUM_ITEMSPACE)2)
        return true;
    if (fromSpace == (ENUM_ITEMSPACE)0xc || toSpace == (ENUM_ITEMSPACE)0xc)
        return true;

    int fromType = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)fromSpace);
    int toType = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)toSpace);

    int foundFrom = 0;
    int foundTo = 0;
    if (fromType == 5)
    {
        const CExpandEquipslot* expand =
            user->GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)9);
        Inven_Item slot = expand->GetInvenSlot((INVEN_TYPE)fromType, fromSlot);
        foundFrom = slot.m_addInfo;                     // 读 sret 结果 +2
    }
    else if (fromType == 6)
    {
        fromItemIdx = 0;                                // ORIG 清参数槽 0x14(%ebp)
    }
    else
    {
        Inven_Item slot =
            user->getCurCharacInvenR()->GetInvenSlot(fromType, fromSlot);
        foundFrom = slot.m_addInfo;
    }

    if (toType == 5)
    {
        const CExpandEquipslot* expand =
            user->GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)9);
        Inven_Item slot = expand->GetInvenSlot((INVEN_TYPE)toType, toSlot);
        foundTo = slot.m_addInfo;
    }
    else if (toType == 6)
    {
        toItemIdx = 0;                                  // ORIG 清参数槽 0x20(%ebp)
    }
    else
    {
        Inven_Item slot =
            user->getCurCharacInvenR()->GetInvenSlot(toType, toSlot);
        foundTo = slot.m_addInfo;
    }

    return foundFrom == (int)fromItemIdx && foundTo == (int)toItemIdx;
