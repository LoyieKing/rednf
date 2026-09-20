long CUserCharacInfo::getCurCharacCreateTime() const
{
    return m_selected ? (long)*reinterpret_cast<const CommonTime*>(m_selected->m_delReqTime) : 0;
}
