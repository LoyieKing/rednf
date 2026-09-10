bool DBSaveHeroMissionEventData::dispatch(int code, int uid, Stream* stream)
{
    try
    {
        Taiwan::SigSaveHeroMissionData* sig =
            stream->GetOutBuffer<Taiwan::SigSaveHeroMissionData>();
        if (sig == 0)
            throw "context_null";
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle_null";
        // ORIG 0x0815db98：set_query 返回值不检查（call 后直接 exec）；仅 exec(1)
        // 失败才 throw "exec_update"。b28d-lane6 试形：三实参内联（ORIG 用
        // esi/ebx 寄存器流转、无 serverGroup/characNo/blob 具名槽，帧 0x30）。
        db->set_query(
            "upDate event_hero_mission_data set mission='%s' where server_group=%d and charac_no=%u",
            db->blob_to_str(0, reinterpret_cast<SigHeroMissionDataView*>(sig)->m_data, 0x100),
            G_CEnvironment()->get_server_group(),
            reinterpret_cast<SigHeroMissionDataView*>(sig)->m_field0);
        if (!db->exec(1))
            throw "exec_update";
        return 1;
    }
    catch (const char* e)
    {
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", __PRETTY_FUNCTION__,
            0x413,
            "[HeroMission] Error load hero mission. (msg:%s)", e);
        return 0;
    }
