void TeamInfoSecond::reset()
{
    /* ORIG 帧 0x28(@0855fdc3) / 循环变量槽 -0xc(@0855fdcf)：两个死槽在 i
     * 之前占用 -0x4/-0x8。4.4 死 pad 不占槽，改 double framePad 撑帧
     * （P190/技巧31 系，/tmp/p26_slot.cpp probeC 实证同形 0x28/i=-0xc）。 */
    double framePad;
    m_teamId = 0;
    for (int i = 0; i < 4; ++i)
        m_seats[i].reset();
    (void)&framePad;
