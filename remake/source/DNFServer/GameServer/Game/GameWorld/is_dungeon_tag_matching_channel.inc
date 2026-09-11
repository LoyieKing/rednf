bool GameWorld::is_dungeon_tag_matching_channel(char* channel)
{
    channel_script_t* channelScript =
        (channel_script_t*)G_CDataManager()->GetChannelScript();
    // ORIG 0x86cbc1a `mov %eax,%ebx`：channelNo 直落 callee-saved %ebx（技巧2）
    register unsigned int channelNo = G_CEnvironment()->get_channel_no();
    channel_info_t* info = channelScript->getChannelInfo(
        (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup,
        channelNo);
    if (info == NULL)
    {
        return false;
    }
    // b28c-lane3：历史 register bool asm("al") 绑定按 P0/N96 清除（自然形）。
    // ORIG 0x86cbc6a-0x86cbc71 `test %eax; sete %al; test; je`。
    if (strcmp(channel, info->m_dungeonName.c_str()) == 0)
    {
        return true;
    }
    return false;
}