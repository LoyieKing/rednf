void UseFatigueMission::processMission(CUser* /*user*/, HeroMissionValue& value,
                                       unsigned int amount)
{
    // [b28d-solo-HeroMissionEvent] ORIG 0x08165706：先 call m_extra[0]（operator[]），
    // 比较方向 `limit <= field4`（cmp %ax,%dx; setbe; jne 跳尾 return）；条件 2 同向
    // （limit <= field4+param → fall-through 钳制，je 跳累加块）。
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
