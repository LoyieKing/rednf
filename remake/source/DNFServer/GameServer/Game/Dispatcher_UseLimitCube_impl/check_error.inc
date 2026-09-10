int Dispatcher_UseLimitCube::check_error(int slotA, unsigned int itemIdx, int slotB,
                                         Inven_Item& invA, Inven_Item& invB,
                                         const CItem* cube)
{
    (void)slotA;
    (void)slotB;
    if (cube == 0)
    {
        return 0x11;
    }
    if (invA.m_addInfo == 0 || (unsigned int)invA.m_addInfo != itemIdx)
    {
        return 0x11;
    }
    if (invB.m_addInfo == 0)
    {
        return 0x11;
    }
    if (!cube->is_stackable())
    {
        return 0x11;
    }
    if (cube->GetItemType() != 0x1d)
    {
        return 0x11;
    }
    if (!((CStackableItem*)cube)->is_included_material_for_limit_cube(itemIdx))
    {
        return 0x11;
    }
    return 0;
