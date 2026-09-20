int CUserCharacInfo::getRequestRenameState() const
{
    return m_selected ? m_selected->m_viewState : 0;
}