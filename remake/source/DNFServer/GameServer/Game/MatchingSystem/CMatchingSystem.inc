CMatchingSystem::CMatchingSystem() throw()
{
    // ORIG 0x085dfac6：m_systems[0..5] 置 0。
    for (int i = 0; i < 6; ++i)
        m_systems[i] = 0;
}