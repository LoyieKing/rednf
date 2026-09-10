bool DBSelectMobileAuthRewardTw::dispatch(int code, int uid, Stream* stream)
{
    // [b28d-solo-TaiwanDBDispatcher] 语义闭合：msg-null/handle-null throw、set_query
    // (不查返)+exec→select_exec、get_n_rows()==0 → INSERT+ok=false+trace(0x2be)、
    // else ok=true+trace(0x2c3)、makeRequest(uid, m_id, ok)、return 1、catch
    // logFormat(0x2cd) 返 0——逐条对齐 ORIG。原「残余 N82/N86」已解：删 mId 具名缓存
    // 改逐处 `*reinterpret_cast<unsigned int*>(sig)` 直读（P183/P0）⇒ AE 171/171。
    try
    {
        bool ok = true;
        DBSelectMobileAuthRewardTw::MSG* sig = stream->GetOutBuffer<DBSelectMobileAuthRewardTw::MSG>();
        if (sig == 0)
            throw "msg is null";
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        // [b28d-solo-TaiwanDBDispatcher] P183/P0：ORIG 0x0815c6a8 每处直读
        // `mov -0x14(%ebp),%eax; mov (%eax),%eax`（无 mId 具名槽），删除缓存局部。
        // ORIG：set_query 返回值不检查（仅 exec 检查）
        db->set_query("seLect m_id from mobile_auth_reward_tw where m_id=%s",
                      NumberToString(*reinterpret_cast<unsigned int*>(sig), 0));
        if (!db->exec(1))
            throw "select_exec";
        if (db->get_n_rows() == 0)
        {
            db->set_query(
                "inSert into mobile_auth_reward_tw(m_id,occ_date) VALUES(%s,now())",
                NumberToString(*reinterpret_cast<unsigned int*>(sig), 0));
            if (!db->exec(1))
                throw "insert_exec";
            ok = false;
            cMyTrace(__PRETTY_FUNCTION__, 0x2be, 0)(
                "[Taiwan, MobileAuth] insert mobile_auth_reward_tw. m_id:%s",
                NumberToString(*reinterpret_cast<unsigned int*>(sig), 0));
        }
        else
        {
            ok = true;
            cMyTrace(__PRETTY_FUNCTION__, 0x2c3, 0)(
                "[Taiwan, MobileAuth] select mobile_auth_reward_tw. m_id:%s",
                NumberToString(*reinterpret_cast<unsigned int*>(sig), 0));
        }
        InterSelectMobileAuthReward::makeRequest(
            uid, *reinterpret_cast<unsigned int*>(sig), ok);
        return 1;
    }
    catch (const char* e)
    {
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", __PRETTY_FUNCTION__,
            0x2cd,
            "[Taiwan, MobileAuth] mobile_auth_reward_tw. (error:%s)", e);
        return 0;
    }
}
