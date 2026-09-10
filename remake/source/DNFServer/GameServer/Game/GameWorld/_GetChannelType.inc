int GameWorld::_GetChannelType()
{
    if (G_CEnvironment()->m_serverEnvir.m_channelType <= 0x12)
    {
        return G_CEnvironment()->m_serverEnvir.m_channelType;
    }
    channel_script_t* channelScript =
        (channel_script_t*)G_CDataManager()->GetChannelScript();
    // ORIG 0x086cXXXX `mov 0x1b0(%eax),%eax; mov %eax,%ebx`：channelNo 直落
    // callee-saved %ebx（技巧2 register 同型成员单 mov），免 -0x10 槽、帧对齐
    register unsigned int channelNo =
        G_CEnvironment()->m_serverEnvir.m_channelNo;
    channel_info_t* info = channelScript->getChannelInfo(
        (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup,
        channelNo);
    if (info == NULL)
    {
        return 0x13;
    }
    return info->m_channelType;
