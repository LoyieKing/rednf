Arad_DatabaseDispatcher::~Arad_DatabaseDispatcher()
{
    std::for_each(m_dispatchers.begin(), m_dispatchers.end(), DeleteDispatcher());
