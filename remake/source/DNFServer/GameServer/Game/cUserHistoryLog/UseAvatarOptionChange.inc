void cUserHistoryLog::UseAvatarOptionChange(int a, int b, int c, int d)
{
    if (m_user != 0)
        m_user->LogHistory("UseAvatarOptionChange,%d,%d,%d,%d", a, b, c, d);
    return;
