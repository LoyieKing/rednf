bool CUser::IsExistGuildAgit()
{
    return m_guildDB.m_agitExist != 0;   // +0x8d0be（成员视图折叠单指令 movzbl）
}