bool DB_LoadEtc::LoadAccountFirstLoginDaily(SIG_LOAD_ETC* sig, int eventCode)
{
    // b28d-31: ORIG 0x08405e0a —— 首段链式（无 evMng 具名槽）；第二段具名 -0x20。
    // get_convert_to_user_confirmflag 在 3 处 set/reset 各重发（P167 差集 ×2）。
    if (!reinterpret_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng())->is_eventing(eventCode))
    {
        return false;
    }
    EventClassify::CEventScriptMng* evMng =
        reinterpret_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng());
    EventClassify::CEventEntity* entity = evMng->get_event_entity(eventCode);
    if (entity == 0)
    {
        return false;
    }

    // ORIG 0x08405ebc：today 走重新链式（无 evMng 槽复用）。
    int today = reinterpret_cast<EventClassify::CEventScriptMng*>(
                    G_CDataManager()->get_event_script_mng())->get_today_time();
    if (today < entity->get_event_start_time()
        || today > entity->get_event_end_time())
    {
        return false;
    }

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                  (ENUM_SERVER_GROUP)0);
    time_t nowT = time(0);
    struct tm curTm;
    localtime_r(&nowT, &curTm);
    struct tm resetTm;
    // ORIG 0x08405f2c：11 个字段逐对 `mov src,%eax; mov %eax,dst`（P54 逐字段赋值形）。
    resetTm.tm_sec = curTm.tm_sec;
    resetTm.tm_min = curTm.tm_min;
    resetTm.tm_hour = curTm.tm_hour;
    resetTm.tm_mday = curTm.tm_mday;
    resetTm.tm_mon = curTm.tm_mon;
    resetTm.tm_year = curTm.tm_year;
    resetTm.tm_wday = curTm.tm_wday;
    resetTm.tm_yday = curTm.tm_yday;
    resetTm.tm_isdst = curTm.tm_isdst;
    resetTm.tm_gmtoff = curTm.tm_gmtoff;
    resetTm.tm_zone = curTm.tm_zone;
    resetTm.tm_hour =
        G_CEnvironment()->m_serverEnvir.m_fatigueTime;       // ORIG tm_hour=+0x37c
    resetTm.tm_min = 0;
    resetTm.tm_sec = 0;
    long baseSec = mktime(&resetTm);
    if (curTm.tm_hour < resetTm.tm_hour)
    {
        baseSec -= 0x15180;
    }

    db->set_query("seLect m_id from %s where m_id=%u and occ_date >"
                  " FROM_UNIXTIME(%d)", toTString(entity->get_db_table_name()),
                  sig->m_accId, baseSec);

    // ORIG 0x08405fd5：exec 结果直测（无 ok 具名槽）⇒ -0xd 归 baseSec，全槽组回正 4B。
    if (!db->exec(true))
    {
        sig->m_bitset.set((unsigned int)
            evMng->get_convert_to_user_confirmflag(eventCode), true);
        return false;
    }
    int nRows = db->get_n_rows();                            // ORIG -0xc 具名槽
    // ORIG 0x08406025：cmpl $0,-0xc; je <reset 块> ⇒ set 块直落（正极性 !=0，P220）。
    if (nRows != 0)
    {
        sig->m_bitset.set((unsigned int)
            evMng->get_convert_to_user_confirmflag(eventCode), true);
    }
    else
    {
        sig->m_bitset.reset((unsigned int)
            evMng->get_convert_to_user_confirmflag(eventCode));
    }
    return true;
