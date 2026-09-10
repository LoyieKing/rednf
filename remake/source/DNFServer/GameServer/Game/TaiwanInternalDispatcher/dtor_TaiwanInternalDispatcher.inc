TaiwanInternalDispatcher::~TaiwanInternalDispatcher()
{
    std::for_each(m_dispatchers.begin(), m_dispatchers.end(), FnDelete());  // ORIG @0x08160212
