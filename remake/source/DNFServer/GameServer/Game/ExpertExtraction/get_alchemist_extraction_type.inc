int CAlchemistExtraction::get_alchemist_extraction_type(CEquipItem const* equip)
{
    switch (equip->m_itemType)
    {
    case 10:
        return 2;
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
        return 0;
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
        return 1;
    default:
        return 3;
    }
