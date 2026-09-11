// ORIG 0x8504d0d：比较两个槽位排序键；键 = 槽空(addInfo==0)?0xb:m_field1(ITEM_TYPE)。
int _CompareSlot(const void* va, const void* vb)
{
    const Inven_Item* a = (const Inven_Item*)va;
    const Inven_Item* b = (const Inven_Item*)vb;
    int sa = a->m_addInfo != 0 ? a->m_field1 : 0xb;
    int sb = b->m_addInfo != 0 ? b->m_field1 : 0xb;
    if (sa < sb) return -1;
    if (sa > sb) return 1;
    return 0;
}