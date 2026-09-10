void CMailBox::CMail::ResetPackage()
{
    m_senderName[0] = '\0';
    m_package.m_item.reset();
    m_package.m_packageMoney = 0;
    m_unlimit = 0;
    m_packageType = 0;
