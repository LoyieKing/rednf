int CExchangeRandomItemReward::sendRandomItemReward(
    CUser* user, std::vector<std::pair<int, int> >& rewards,
    std::vector<int>& recordVec, unsigned char& flag)
{
    // [b28d-L14 实测] 提升 actionMng 到循环前（ORIG 声明序 it→actionMng→end→result）。
    int result = 0;
    CEventActionMng actionMng;
    for (std::vector<std::pair<int, int> >::iterator it = rewards.begin();
         it != rewards.end(); ++it)
    {
        static_cast<ExchangeRandomItemRewardScript*>(m_script1c)
            ->m_mail.set_item(it->first, it->second);
        result = actionMng.process_action_send_mail(
            user, static_cast<ExchangeRandomItemRewardScript*>(m_script1c)
                      ->m_mail, true);
        if (result != 0)
            return result;
        ++flag;
    }
    recordVec = static_cast<ExchangeRandomItemRewardScript*>(m_script1c)
                    ->m_mail.m_vec38;
    static_cast<ExchangeRandomItemRewardScript*>(m_script1c)
        ->m_mail.m_vec38.clear();
    return 0;
