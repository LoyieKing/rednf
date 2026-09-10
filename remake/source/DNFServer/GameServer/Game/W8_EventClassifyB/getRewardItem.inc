int CExchangeRandomItemReward::getRewardItem(
    std::vector<std::pair<int, int> >& out)
{
    // [b28d-L14 实测] 提升 actionMng 到循环前（ORIG 声明序 it→actionMng→end→result）。
    int result = 0;
    CEventActionMng actionMng;
    for (std::vector<int>::iterator it =
             static_cast<ExchangeRandomItemRewardScript*>(m_script1c)
                 ->m_ints18.begin();
         it != static_cast<ExchangeRandomItemRewardScript*>(m_script1c)
                   ->m_ints18.end();
         ++it)
    {
        result = actionMng.getResultItemFromLottery((unsigned int)*it,
                                                    out);
        if (result != 0)
            return result;
    }
    return 0;
