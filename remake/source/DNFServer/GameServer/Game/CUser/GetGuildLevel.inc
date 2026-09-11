unsigned char CUser::GetGuildLevel()
{
    return m_guildDB.m_fundLevel;   // +0x8d03b（成员视图折叠单指令 movzbl）
}