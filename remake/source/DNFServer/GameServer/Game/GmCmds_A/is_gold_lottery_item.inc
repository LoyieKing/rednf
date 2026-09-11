// ORIG is_gold_lottery_item 0x084afccc：lotto 表项 id(+2)==0 且数量(+7)>0。
bool CBoosterTest::is_gold_lottery_item(int idx)
{
    Inven_Item lotto;
    G_CDataManager()->m_itemList->get_lotto_item(idx, reinterpret_cast<char*>(&lotto));
    return lotto.m_addInfo == 0 &&
           lotto.m_addInfo2 > 0;
}