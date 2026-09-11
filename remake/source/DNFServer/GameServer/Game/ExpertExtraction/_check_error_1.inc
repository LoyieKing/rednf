// ============================================================================
// CEnchanterExtraction::_check_error —— ORIG T 0x084a3b92（+0x338 等级字段）
// ============================================================================
// [b28d-solo-ExpertExtraction] AE 修复：同 CAlchemistExtraction::_check_error
//   （具名 level 局部 + `need > limit` 方向；字段 m_int338 / 脚本类型 1）。p167 13/13 CLEAR。
int CEnchanterExtraction::_check_error(CUser* user, short a, short b)
{
    Inven_Item item1 = user->getCurCharacInvenR()->GetInvenSlot(1, (int)a);
    CStackableItem* mat1 = (CStackableItem*)G_CDataManager()->find_item(item1.m_addInfo);
    Inven_Item item2 = user->getCurCharacInvenR()->GetInvenSlot(1, (int)b);
    CItem* mat2 = G_CDataManager()->find_item(item2.m_addInfo);
    int level = mat1->m_int338;
    int limit = ((STExpertJobScript*)G_CDataManager()->GetExpertJobScript(1))
                    ->GetEnchanterExtractionPossibleLevel(level);
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