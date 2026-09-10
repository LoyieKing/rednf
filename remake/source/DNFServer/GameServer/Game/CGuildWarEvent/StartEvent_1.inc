void CGuildWarEvent::StartEvent(Word_Param param)
{
    if (!IsEventing(0))
    {
        SetEventFlag(true);
        ((GWView*)this)->m_wp = param;
        if (G_GameWorld()->GetChannelType() == 6)
        {
            G_TimerQueue()->InsertTimer(
                TimerEntry::OBJ_TYPE_2, 0, TIMER_MESSAGE_0x62,
                (int)m_duration * 0xe10 - 600, 10, 0);
            GlobalData::s_guild_proxy_mgr
                ->GetServerProxy((ENUM_SERVER_GROUP)0)
                ->SendGuildWarStart(
                    (unsigned char)((CEnvView*)G_CEnvironment())->m_gcNo);
            cMyTrace(__PRETTY_FUNCTION__, 0x39, 0)(
                "Guild War Event Start! time(%d), server(%d)",
                (int)param.m_duration,
                ((CEnvView*)G_CEnvironment())->m_gcNo);
        }
    }
