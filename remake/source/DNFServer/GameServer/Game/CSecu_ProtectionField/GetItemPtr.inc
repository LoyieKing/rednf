CItem* CSecu_ProtectionField::GetItemPtr(CUser* user, char space, short slot)
{
    // ORIG T 0x082884d6（报告 GetItemPtr.md）：switch 树状展开，probe 整体拷贝。
    Inven_Item probe;
    switch (space)
    {
    case 0:
    case 3:
    case 7:
        probe = user->getCurCharacInvenR()->GetInvenSlot(
            GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(int)space), (int)slot);
        break;
    case 2:
        probe = user->getCurCharacCargoR()->get_cargo_slot((int)slot);
        break;
    }
    CItem* result = G_CDataManager()->find_item(probe.m_addInfo);
    return result;
