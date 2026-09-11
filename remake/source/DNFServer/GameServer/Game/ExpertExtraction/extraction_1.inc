// ============================================================================
// CEnchanterExtraction::extraction —— ORIG T 0x084a37c6
//   无 result_type 分支（Enchanter 单结果表）。
// ============================================================================
// [b28d-solo-ExpertExtraction] AE 修复：同 AlchemistExtraction::extraction 三旋钮
//   （pick 两句 + cMyTrace 全表达式链 + 内联 count；单结果表无 result_type 分支）。
//   p167 25/25 CLEAR。
bool CEnchanterExtraction::extraction(CUser* user, short param,
                                      IntPairVector& needItems)
{
    const CInventory* invenR = user->getCurCharacInvenR();
    Inven_Item item = invenR->GetInvenSlot(1, (int)param);
    CEquipItem* equip = (CEquipItem*)G_CDataManager()->find_item(item.m_addInfo);
    STExpertJobScript* script = (STExpertJobScript*)G_CDataManager()->GetExpertJobScript(1);
    STExpertJobScript::stEnchanterExtractionResultInfo* info =
        script->GetEnchanterExtractionResultInfo();
    int sum = info->m_count;
    unsigned long range = (unsigned long)(sum - 1);
    int pick = m_rand->randInt(range);
    ++pick;
    std::vector<STExpertJobScript::stMonsterCardBindResult>& vec = info->m_vec;
    if (vec.size() == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x169, 5)(
            "CEnchanterExtraction::extraction ERROR : size(%d)", (int)vec.size());
        return false;
    }
    if (vec.size() != 0 && sum == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x16e, 5)(
            "CEnchanterExtraction::extraction ERROR : size(%d), sum(%d)",
            (int)vec.size(), sum);
        return false;
    }
    int acc = 0;
    unsigned int i = 0;
    while (i < vec.size())
    {
        if (acc < pick &&
            acc + (int)(unsigned short)vec[i].m_field4 >= pick)
        {
            needItems.push_back(std::make_pair(vec[i].m_field0,
                                               (int)get_extraction_count(item)));
            break;
        }
        acc += (int)(unsigned short)vec[i].m_field4;
        ++i;
    }
    if (needItems.empty())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x17f, 5)(
            "CEnchanterExtraction::extraction ERROR : result is empty");
        return false;
    }
    return true;
}