void CCeraShopBonusItemEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    if (param.m_duration == 0 || 1000 < param.m_duration) {
        // [b28c-30] P227 全表达式临时链：ORIG 0x0810a361 段 vararg
        // `(unsigned int)param.m_duration` 在 ctor 前求值并入 %ebx 跨 ctor 保活
        // （push %ebx/sub $0x34 三件套）；具名 tr 两语句使 4.4 ctor 后重取。
        cMyTrace(__FUNCTION__, 0x1a, 0)
            ("Can not start Cera shop bonus item event. Bonus rate is too big(%d)",
             (unsigned int)param.m_duration);
    } else {
        m_rate = (unsigned int)param.m_duration;
        cMyTrace(__FUNCTION__, 0x20, 0)
            ("Cera shop bonus item event Start");
    }
