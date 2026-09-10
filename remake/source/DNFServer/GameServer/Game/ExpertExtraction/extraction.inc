bool CAlchemistExtraction::extraction(CUser* user, short param,
                                      std::vector<std::pair<int, int> >& needItems)
{
    const CInventory* invenR = user->getCurCharacInvenR();
    Inven_Item item = invenR->GetInvenSlot(1, (int)param);
    CEquipItem* equip = (CEquipItem*)G_CDataManager()->find_item(item.m_addInfo);
    STExpertJobScript* script = (STExpertJobScript*)G_CDataManager()->GetExpertJobScript(2);
    STExpertJobScript::stAlchemistExtractionResultInfo* info =
        script->GetAlchemistExtractionResultInfo();
    int type = get_alchemist_extraction_type(equip);
    if (type == 3)
    {
        LogManager::logFormat(1, "Extraction.cpp", __PRETTY_FUNCTION__, 0xe0,
                              "CAlchemistExtraction::extraction ERROR : result_type(%d)", type);
        return false;
    }
    int sum = info->m_count[type];
    unsigned long range = (unsigned long)(sum - 1);
    int pick = m_rand->randInt(range);
    ++pick;
    std::vector<STExpertJobScript::stMonsterCardBindResult>& vec = info->m_vec[type];
    if (vec.size() == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xec, 5)(
            "CAlchemistExtraction::extraction ERROR : size(%d)", (int)vec.size());
        return false;
    }
    if (vec.size() != 0 && sum == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xf1, 5)(
            "CAlchemistExtraction::extraction ERROR : size(%d), sum(%d)",
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
        cMyTrace(__PRETTY_FUNCTION__, 0x102, 5)(
            "CAlchemistExtraction::extraction ERROR : result is empty");
        return false;
    }
    return true;
