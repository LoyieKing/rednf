CMatchingSystem::~CMatchingSystem()
{
    // ORIG 0x085dfaf4：i=0..5，对非空槽位经虚析构 delete。
    for (int i = 0; i < 6; ++i)
        if (m_systems[i] != 0)
            delete m_systems[i];
