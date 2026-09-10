bool CStackableItem::verifyUsableServerType()
{
    if (m_usableServers.empty())
    {
        return 1;
    }
    StackableAction::UsableServerType serverType =
        static_cast<StackableAction::UsableServerType>(0);
    if (((CEnvironmentView*)G_CEnvironment())->m_serverType == 0)
    {
        serverType = StackableAction::USABLE_SERVER_TYPE_GENERAL;
    }
    if (((CEnvironmentView*)G_CEnvironment())->m_serverType == 2)
    {
        serverType = StackableAction::USABLE_SERVER_TYPE_STARTER;
    }
    if (((CEnvironmentView*)G_CEnvironment())->m_serverType == 1)
    {
        serverType = StackableAction::USABLE_SERVER_TYPE_EVENT;
    }
    if (std::find(m_usableServers.begin(), m_usableServers.end(), serverType) ==
        m_usableServers.end())
    {
        return 0;
    }
    return 1;
