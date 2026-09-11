// ===========================================================================
// 26. _debugCommandPowerWarEnd —— 0x0859044e
// ===========================================================================
bool DisPatcher_DebugCommand::_debugCommandPowerWarEnd(CUser* user)
{
    GlobalData::s_guild_proxy_mgr->GetServerProxy(
        static_cast<ENUM_SERVER_GROUP>(0))
        ->SendPowerWarEnd(user->get_acc_id());
    return true;
}