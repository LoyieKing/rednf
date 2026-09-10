void Avatar_Item::SetData(int expireDate, int registTime, const char* ipgAgency)
{
    m_expireDate = expireDate;
    m_registTime = registTime;
    strncpy(m_ipgAgencyNo, ipgAgency, 0x18);
