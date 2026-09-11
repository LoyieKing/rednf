void GameResultType::Clear()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
    m_field9 = 0;
    m_fielda = 0;
    m_fieldc = 0;
    m_field10 = 0;
    m_field34 = 0;
    m_field36 = 0;
    m_field38 = 0;
    m_field3a = 0;
    m_field3c = 0;
    m_field3e = 0;
    m_field40 = 0;
    m_field42 = 0;
    m_field48 = 0;
    m_field44 = 0;
    m_field4a = 0;
    m_field4c = 0;
    memset(m_shot, 0, sizeof(m_shot));
}