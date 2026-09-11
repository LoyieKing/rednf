// ---- SEventPeriodData -------------------------------------------------------
// 【objdump】C1：m_eventCode/m_beginTime/m_endTime 三字段直存（无其它副作用）。
SEventPeriodData::SEventPeriodData(int eventCode, unsigned int beginTime,
                                   unsigned int endTime)
{
    // 【二次核验 CCMC8】ORIG 0x088b4368 首存基址先载；4.4 对 int→unsigned 首成员
    // 恒值先物化——this->+(unsigned) 赋值形与成员初始化列表形两种 probe 实测均不改，
    // 维持直存（同 GetInt 族 N18a/cprop 归因）。
    m_eventCode = eventCode;
    m_beginTime = beginTime;
    m_endTime   = endTime;
}