Inven_Item* CSecu_ProtectionField::GetInvenItemPtr(CUser* user, char space,
                                                   short slot)
{
    // ORIG T 0x082886ae（报告 GetInvenItemPtr.md）：switch 树状展开。
    switch (space)
    {
    case 0:
    case 3:
    case 7:
        return (Inven_Item*)user->getCurCharacInvenR()->GetInvenRef(
            GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(int)space), (int)slot);
    case 2:
        return user->getCurCharacCargoR()->GetCargoRef((int)slot);
    }
    return 0;
