void CUser::SetTradePunishOverThirtyDays(bool flag)
{
    printf(" CUser::SetTradePunishOverThirtydays(%c) \n",
           static_cast<unsigned int>(flag));
    m_postChecksumData.m_tradePunishOverThirtyDays = flag;
