void CUser::SetGuildDBInfo(STGuildDBInfoOnly& info)
{
    memcpy(&m_guildDB, &info, 0xbd);
