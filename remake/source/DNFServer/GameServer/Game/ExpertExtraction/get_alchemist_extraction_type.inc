// ============================================================================
// CAlchemistExtraction::get_alchemist_extraction_type —— ORIG T 0x084a355c
//   m_itemType(+0x234) 跳表：10→2；0xc..0x10→0；0x11..0x15→1；其它→3
// ============================================================================
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
}