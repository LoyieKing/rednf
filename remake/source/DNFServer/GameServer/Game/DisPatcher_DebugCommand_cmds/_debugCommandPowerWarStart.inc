// ===========================================================================
// 25. _debugCommandPowerWarStart —— 0x0859040e
// ===========================================================================
bool DisPatcher_DebugCommand::_debugCommandPowerWarStart(CUser* user)
{
    GlobalData::s_guild_proxy_mgr->GetServerProxy(
        static_cast<ENUM_SERVER_GROUP>(0))
        ->SendPowerWarStart(user->get_acc_id());
    return true;
}