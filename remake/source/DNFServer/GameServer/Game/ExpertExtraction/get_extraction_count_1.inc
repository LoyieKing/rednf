int CEnchanterExtraction::get_extraction_count(Inven_Item item)
{
    CItem* c = G_CDataManager()->find_item(item.m_addInfo);
    int g = c->get_grade() / 10;
    if (g == 0)
    {
        g = 1;
    }
    return g;
