void CUserCharacInfo::setCharacViewState(int state)
{
    if (m_selected)
    {
    m_selected->m_viewState = state;
    }
}