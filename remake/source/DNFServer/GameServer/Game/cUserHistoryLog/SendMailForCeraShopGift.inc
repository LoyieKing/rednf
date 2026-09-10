void cUserHistoryLog::SendMailForCeraShopGift(unsigned int mailIdx, char flag,
                                              unsigned int a, unsigned int b)
{
    m_user->LogHistory("MailSG,%u,%d,%u,%u", mailIdx, (int)flag, a, b);
