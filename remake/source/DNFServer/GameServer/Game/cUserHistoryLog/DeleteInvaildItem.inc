void cUserHistoryLog::DeleteInvaildItem(const std::string& str, int a, int b)
{
    m_user->LogHistory("DeleteInvaildItem, %s, %d, %d", str.c_str(), a, b);
