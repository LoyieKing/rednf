unsigned short CUserCharacInfo::GetCurCharacEventItemCount()
{
    return m_selected ? m_selected->m_eventItemCount : 0;
}