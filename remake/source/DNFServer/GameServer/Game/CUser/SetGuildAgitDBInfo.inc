void CUser::SetGuildAgitDBInfo(STGuildAgitDBInfo& info)
{
    memcpy(&m_guildAgitDB, &info, 1);
}