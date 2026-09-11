int CUserCharacInfo::getRequestRenameState() const
{
    return m_selected ? CUR->m_characViewState : 0;
}