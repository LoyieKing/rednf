bool DB_UpdateLogout::UpdateLogout(SIG_UPDATE_LOGOUT* sig)
{
    if (((DB_UpdateLogoutSigView*)sig)->m_accId == 0)
        return 1;
    // [b28d-solo-4] P316 volatile 旋钮：ORIG 4.1 先装载 serverId（-0x10→%edx）
    // 再算 m_gcNo（→%ecx）；4.4 右→左恒反序。volatile 声明强制该读先行。
    // [L4-r1 判据⑩] 槽序反转律：ORIG 声明序 = bool r → int serverId → ptr db
    //（槽 r@-0x11 / serverId@-0x10 / db@-0xc）⇒ r 必须声明在 serverId 之前。
    bool r;
    volatile int serverId = 0;
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        serverId = ((DB_UpdateLogoutSigView*)sig)->m_serverId;
    else
        serverId = G_CEnvironment()->m_serverEnvir.m_serverGroup;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    db->set_query("upDate login_account_%d set m_channel_no=%d,login_status=%d,last_login_date=now() where m_id=%s",
                  serverId, ((DB_UpdateLogoutSigView*)sig)->m_gcNo, 0,
                  NumberToString(((DB_UpdateLogoutSigView*)sig)->m_accId, 0));
    r = db->exec(true);
    if (!r)
        return 0;
    return 1;
