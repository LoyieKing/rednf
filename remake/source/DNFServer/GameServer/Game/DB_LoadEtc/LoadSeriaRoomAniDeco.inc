// ============================================================================
// DB_LoadEtc::LoadSeriaRoomAniDeco @0x0844a20a —— seria 房间动画装饰双 short
// 至 +0x14b5c/+0x14b5e（combo 区尾部残段，超 SIG_COMBO_SKILL 0xb2 的部分）。
// ============================================================================
bool DB_LoadEtc::LoadSeriaRoomAniDeco(SIG_LOAD_ETC* sig)
{
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x69)->IsEventing((CUser*)0))
    {
        return false;
    }
    // ORIG：ok=-0xd（深）先声明、db=-0xc（浅）后声明（4.4 后声明→浅）。
    bool ok;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect receive_flow, deco_flow from event_1208_seriaroom_anideco"
                  " where m_id = %d and server_id = %d and charac_no = %d",
                  sig->m_accId, sig->m_serverGroup, sig->m_characNo);
    // ORIG 0844a2b0：exec 结果存 -0xd（深于 db=-0xc）→ ok 先声明。
    ok = db->exec(true);
    if (!ok)
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0xba8f, "LoadSeriaRoomAniDeco Query Error : m_id(%d)",
                              sig->m_accId);
        return false;
    }
    // ORIG 0844a308-30f：test/setne 物化正条件（nRows != 0 才进 fetch 段），
    // 尾统一 return true。
    if (db->get_n_rows() != 0)
    {
        if (!db->fetch())
        {
            return false;
        }
        if (!db->get_short(0,
                           *reinterpret_cast<short*>(sig->m_seriaRoomAniDeco)))  // +0x14b5c
        {
            return false;
        }
        if (!db->get_short(1,
                           *reinterpret_cast<short*>(sig->m_seriaRoomAniDeco + 2)))  // +0x14b5e
        {
            return false;
        }
    }
    return true;
}