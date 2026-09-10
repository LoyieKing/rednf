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
