void UseCubeMission::processMission(CUser* /*user*/, HeroMissionValue& value,
                                    unsigned int amount)
{
    // [b28d-solo-HeroMissionEvent] ORIG 0x08165992：与 UseFatigueMission 同骨架
    // （limit <= field4 跳尾；limit <= field4+param 钳制）。
    if (m_condition.m_extra[0] <= value.m_field4)
        return;
    if (m_condition.m_extra[0] <=
        static_cast<unsigned int>(value.m_field4) + amount)
    {
        value.m_field4 = m_condition.m_extra[0];
    }
    else
    {
        value.m_field4 = value.m_field4 + static_cast<unsigned short>(amount);
    }
