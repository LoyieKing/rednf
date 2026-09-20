void CUserCharacInfo::set_charac_createtime(long t)
{
    reinterpret_cast<CommonTime*>(m_selected->m_delReqTime)->SetCreateTime(t);
}
