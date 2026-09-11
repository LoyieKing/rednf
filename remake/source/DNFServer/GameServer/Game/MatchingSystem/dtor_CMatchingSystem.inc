// [b28d-lane6] NEAR·N7：虚调用目标 %ecx(ORIG 0x085dfb1c)↔%edx(4.4.7)。探针证 4.4.6 后端可归零，但本 TU 换 446 后 ae 13→11（净负）故不改路由。
CMatchingSystem::~CMatchingSystem()
{
    // ORIG 0x085dfaf4：i=0..5，对非空槽位经虚析构 delete。
    for (int i = 0; i < 6; ++i)
        if (m_systems[i] != 0)
            delete m_systems[i];
}