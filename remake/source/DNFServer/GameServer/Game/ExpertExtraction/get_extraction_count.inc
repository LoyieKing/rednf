// ============================================================================
// CAlchemistExtraction::get_extraction_count —— ORIG T 0x084a3596
//   grade / 5，0 → 1。
// ============================================================================
int CAlchemistExtraction::get_extraction_count(Inven_Item item)
{
    CItem* c = G_CDataManager()->find_item(item.m_addInfo);
    int g = c->get_grade() / 5;
    if (g == 0)
    {
        g = 1;
    }
    return g;
}