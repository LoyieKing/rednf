bool cSecuUserAction::IsSecurityStateOff()
{
    if (m_state == 1)
        return true;
    return false;
