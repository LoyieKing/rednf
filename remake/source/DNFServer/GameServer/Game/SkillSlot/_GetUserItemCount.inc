void CSkillChanger::_GetUserItemCount(const CUser* user)
{
    for (int i = 3; i < 0x69; i++)
    {
        Inven_Item item;
        item = ((CInventory*)user->getCurCharacInvenR())->GetInvenSlot(1, i);
        _CountSkillBook(item);
    }
    for (int i = 0; i < 0x98; i++)
    {
        Inven_Item item;
        item = ((const CCargo*)user->getCurCharacCargoR())->get_cargo_slot(i);
        _CountSkillBook(item);
    }
