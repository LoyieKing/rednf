// [b28c-L26 SIGFIX·档①] 定义返回类型随头改 unsigned int（P159：返回类型不入
// mangled，ABI 同宽；lane35 转来的 spec，Main 终裁本 TU 归我收尾）。
unsigned int CUserCharacInfo::UseFinishPoint(int point)
{
    if (!m_selected)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x220, 5);
        trace("WARAREA CUserCharacInfo::UseFinishPoint() : m_selected is NULL");
        return 0;
    }
    ChangeFinishPoint(-point);
    ((CUser*)this)->m_historyLog.FPSub(
        point, m_selected->m_fieldE06, m_selected->m_fieldE0A, eFPSubReason_0);
    return m_selected->m_fieldE06;
}