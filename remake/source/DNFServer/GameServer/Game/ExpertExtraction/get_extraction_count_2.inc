int CDollControllerExtraction::get_extraction_count(Inven_Item item)
{
    CItem* c = G_CDataManager()->find_item(item.m_addInfo);
    int type = get_doll_controller_extraction_type((CEquipItem*)c);
    int g = 0;
    switch (type)
    {
    case 0:
        g = c->get_grade() / 10;
        break;
    case 1:
        g = (c->get_grade() - 0x14) / 10;
        break;
    case 2:
        g = (c->get_grade() - 0x28) / 10;
        break;
    default:
        break;
    }
    if (g == 0)
    {
        g = 1;
    }
    return g;
