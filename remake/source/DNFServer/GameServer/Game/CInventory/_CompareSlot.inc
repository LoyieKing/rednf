int _CompareSlot(const void* va, const void* vb)
{
    const Inven_Item* a = (const Inven_Item*)va;
    const Inven_Item* b = (const Inven_Item*)vb;
    int sa = a->m_addInfo != 0 ? a->m_field1 : 0xb;
    int sb = b->m_addInfo != 0 ? b->m_field1 : 0xb;
    if (sa < sb) return -1;
    if (sa > sb) return 1;
    return 0;
