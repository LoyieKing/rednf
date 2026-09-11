// ============================================================================
// CDollControllerExtraction::get_extraction_count —— ORIG T 0x084a41a8
//   按类型：0→grade/10；1→(grade-0x14)/10；2→(grade-0x28)/10；0 → 1。
// ============================================================================
// [b28d-solo-ExpertExtraction] AE 修复：switch case 改数值升序 0/1/2 ——
//   ORIG 叶块物理序 = case 书写序（P46）：case0 块直落决策树之后、case1/case2 各跳；
//   原书写 1/2/0 使 case0 块排到末尾（多 1 jmp + 反向 je）。p167 6/6 CLEAR。
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
}