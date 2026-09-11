// ----------------------------------------------------------------------------
// ORIG 0x0866c196 CUser::resetGuildDBInfo() void
void CUser::resetGuildDBInfo()
{
    resetCharGuildInfo();                          // CUserCharacInfo 同基址转发
    memset(&m_guildDB, 0, 0xbd);                   // STGuildDBInfoOnly 0xbd 清零
}