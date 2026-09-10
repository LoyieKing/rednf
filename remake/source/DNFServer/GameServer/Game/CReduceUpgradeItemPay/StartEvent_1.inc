void CReduceUpgradeItemPay::StartEvent(Word_Param param)
{
    m_addInfo.m_info0 = param.m_duration;
    m_addInfo.m_info1 = param.m_field2;
    StartEvent();
    cMyTrace(__PRETTY_FUNCTION__, 0x1c, 0)
        ("CReduceUpgradeItemPay(AddInfo:%d,%d) Event Start!",
         (short)m_addInfo.m_info0, (short)m_addInfo.m_info1);
