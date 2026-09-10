CCompoundAvatar::CCompoundAvatar()
{
    // b28d-45 复测：P212 全头部声明（seed/range/i 按 ORIG 槽深序）槽序不变且
    // 判级 NEAR→DIFF，确认 N82/N98 槽序封顶（ORIG 0x08331deb seed=-0x24 /
    // 0x08331e35 range=-0x20 不可翻）。
    m_rand = new CMTRand(static_cast<unsigned long>(time(0)));
    for (int i = 0; i < 100; ++i)
    {
        unsigned long range = 10000;
        m_rand->randInt(range);
    }
