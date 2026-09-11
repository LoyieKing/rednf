bool PvP_Room::CheckPassword(const char* password, int len)
{
    if (m_passwordLen != len)
    {
        return 0;
    }
    return memcmp(m_password, password, m_passwordLen) == 0;
}