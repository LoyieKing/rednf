void cUserHistoryLog::EnchantItem(int a, int b, int c, bool up,
                                  const char* name1, const char* name2)
{
    if (up)
        m_user->LogHistory("Enchant+,%d,%d,%d,\"%s\",\"%s\"", a, b, c, name1, name2);
    else
        m_user->LogHistory("Enchant-,%d,%d,%d,\"%s\",\"%s\"", a, b, c, name1, name2);
