// ============================================================================
// CAlchemistExtraction::_check_error —— ORIG T 0x084a35f2
//   材料1（槽 a）的 +0x334 等级字段 → 脚本允许等级；材料2（槽 b）的
//   usableLevel（0 时 grade）；超出 → 0xd。
// ============================================================================
// [b28d-solo-ExpertExtraction] AE 修复：mat1->m_int334 提升为具名 level 局部
//   （P0：ORIG `mov 0x334(%eax),%eax; mov %eax,-0x14(%ebp)` 槽往返后回读传参）
//   + 比较改 `if (need > limit)` 复现 ORIG `mov need,%eax; cmp limit,%eax; jle`
//   （原 `limit < need` 产 jge 反向，P196/P47）。三派生类同族（Enchanter 0x084a3b92 /
//   DollController 0x084a4286 同形）。p167 13/13 CLEAR。
int CAlchemistExtraction::_check_error(CUser* user, short a, short b)
{
    Inven_Item item1 = user->getCurCharacInvenR()->GetInvenSlot(1, (int)a);
    CStackableItem* mat1 = (CStackableItem*)G_CDataManager()->find_item(item1.m_addInfo);
    Inven_Item item2 = user->getCurCharacInvenR()->GetInvenSlot(1, (int)b);
    CItem* mat2 = G_CDataManager()->find_item(item2.m_addInfo);
    int level = mat1->m_int334;
    int limit = ((STExpertJobScript*)G_CDataManager()->GetExpertJobScript(2))
                    ->GetAlchemistExtractionPossibleLevel(level);
    int need = mat2->getUsableLevel();
    if (need == 0)
    {
        need = mat2->get_grade();
    }
    if (need > limit)
    {
        return 0xd;
    }
    return 0;
}