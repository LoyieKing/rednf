// ============================================================================
// CDollControllerExtraction::get_doll_controller_extraction_type —— ORIG T 0x084a3d66
//   usableLevel（0 时 grade）区间：1..0x1e→0；0x1f..0x32→1；0x33..0x50→2；其它→3
// ============================================================================
int CDollControllerExtraction::get_doll_controller_extraction_type(CEquipItem const* equip)
{
    int lv = equip->getUsableLevel();
    if (lv == 0)
    {
        lv = equip->get_grade();
    }
    if (lv >= 1 && lv <= 0x1e)
    {
        return 0;
    }
    if (lv >= 0x1f && lv <= 0x32)
    {
        return 1;
    }
    if (lv >= 0x33 && lv <= 0x50)
    {
        return 2;
    }
    return 3;
}