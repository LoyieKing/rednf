Arad_InternalDispatcher::~Arad_InternalDispatcher()
{
    std::for_each(m_dispatchers.begin(), m_dispatchers.end(), DeleteDispatcher());
