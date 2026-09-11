// ============================================================================
// CAlchemistExtraction::extraction —— ORIG T 0x084a3182（报告 extraction.md）
//   按结果表随机选一条目（CMTRand），命中则 get_extraction_count 作数量，
//   needItems 输出 {itemId, count}。
// ============================================================================
// [b28d-solo-ExpertExtraction] AE 修复（三旋钮）：①`pick = randInt(range); ++pick;`
//   两句复现 ORIG `mov %eax,-0x18; addl $1,-0x18`（内存自增）；②三处 cMyTrace 改
//   全表达式临时链 `cMyTrace(...)(fmt, vec.size())`（P227：ORIG 在 ctor 前求值 size()
//   并入 %ebx 跨 ctor 保活；P221 槽方向随之翻正）；③删具名 count，把
//   `(int)get_extraction_count(item)` 内联进 make_pair 第二实参——ORIG 的 -0x44 是
//   `(int)` 右值临时（mangled 实证 make_pair<int&,int>(int&,int&&) @0x080dd799），
//   具名局部会令 4.4 再复制一份到临时并使全帧深移 4（-0x34 vs ORIG -0x30）。
//   p167 27/27 CLEAR。
bool CAlchemistExtraction::extraction(CUser* user, short param,
                                      IntPairVector& needItems)
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
}