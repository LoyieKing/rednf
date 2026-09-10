int CDataManager::find_lottery_use_cost(unsigned int money) const   // ORIG 0x835fb5a：bool→int（%eax 透传）
{
    return m_lotteryNeedMoney.isNeedMoney(money);
