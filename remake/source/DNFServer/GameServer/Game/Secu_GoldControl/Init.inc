void Secu_GoldControl::Init(CUser* user)
{
    m_pUser = user;
    m_dateFlag = GlobalData::g_GoldControlDateFlag;
    m_fieldb8 = 0;
    m_fieldbc = 0;
    m_mailGold = 0;
    m_fielde6 = 0;
    memset(m_mailCharName, 0, 0x1e);
}