void Secu_GoldControl::SetMailCharName(const char* name)
{
    if (name)
    {
        strncpy(m_mailCharName, name, 0x1e);
    }
