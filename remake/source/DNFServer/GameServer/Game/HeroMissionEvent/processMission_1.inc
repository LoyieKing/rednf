void TotalMission::processMission(CUser* /*user*/, HeroMissionValue& value,
                                  unsigned int param)
{
    // [b28d-solo-HeroMissionEvent] ORIG 0x0816637e 先 call size() 再读 field4，
    // 比较方向为 `size() <= (unsigned)field4`（setbe+jne 跳尾 return），
    // 条件 2 同向（setbe + je 跳 else 累加块）；原正向 `<` 写法产 setb+je 反极性。
    if (m_condition.m_extra.size() <= static_cast<unsigned int>(value.m_field4))
        return;
    if (m_condition.m_extra.size() <=
        static_cast<unsigned int>(value.m_field4) + param)
    {
        value.m_field4 = static_cast<unsigned short>(m_condition.m_extra.size());
    }
    else
    {
        value.m_field4 = value.m_field4 + static_cast<unsigned short>(param);
    }
