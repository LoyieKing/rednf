void stLimitItemUsageInfo::Add(char kind, int a, int b)
{
    LimitInfo entry;
    entry.m_kind = kind;
    entry.m_a = a;
    entry.m_b = b;
    m_list.push_back(entry);
