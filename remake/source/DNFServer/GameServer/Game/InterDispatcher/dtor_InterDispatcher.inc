// ORIG 0x84be89e：非空槽经虚表[2]（D0）删除后置 0。
InterDispatcher::~InterDispatcher()
{
    for (int i = 0; i <= 0x353; ++i)
    {
        if (m_dispatchers[i] != 0)
        {
            delete m_dispatchers[i];
        }
        m_dispatchers[i] = 0;
    }
}