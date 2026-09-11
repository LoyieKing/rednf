// ORIG 0x085503c4
MySQL* CLogGameChannel::GetLogTargetDB()
{
    return GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                             E_SERVER_GROUP_NONE);
}