void cUserHistoryLog::UseDye(int a, int b, int c, const std::string& dyeName, int d)
{
    if (m_user != 0)
        m_user->LogHistory("UseDye ,%d,%d,%d,%s,%d", a, b, c, dyeName.c_str(), d);
    return;
