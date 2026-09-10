bool DB_LoadEtc::LoadPcRoomPlayTimeInfo(SIG_LOAD_ETC* sig)
{
    // ORIG 0x084066b4：GetRepeatEvent 结果直留 %eax 调虚槽（无具名局部，P0 槽律）。
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x5c)->IsEventing((CUser*)0))
    {
        return true;
    }

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                  (ENUM_SERVER_GROUP)0);
    // ORIG 0x0840670e：NumberToString 结果内联直进 set_query 实参（无 accStr 槽）。
    db->set_query("seLect play_time, reward_cnt from event_1204_pcroom_play_time"
                  " where m_id = %s and occ_date > curdate()",
                  NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        return false;
    }
    // ORIG 0x0840675e：两局部在 exec 检查之后、nRows 检查之前初始化。
    // N92 突破口（P197① union 双视角）：get_uint 需 unsigned int 左值，而字段
    //   m_totalPcRoomPlayTime 是 int——异型槽值 store 恒多 `mov %eax,%edx` 中转；
    //   union 让出参走 u、store 走同型 i，复现 ORIG 单段 `mov -0x10(%ebp),%edx`。
    union PlayTimeSlot
    {
        unsigned int u;
        int i;
    };
    PlayTimeSlot playTime;
    playTime.u = 0;
    unsigned char rewardCnt = 0;
    if (db->get_n_rows() == 0)
    {
        return true;
    }
    if (!db->fetch())
    {
        return false;
    }
    if (!db->get_uint(0, playTime.u))
    {
        return false;
    }
    if (!db->get_ubyte(1, rewardCnt))
    {
        return false;
    }
    sig->m_totalPcRoomPlayTime = playTime.i;                 // +0xc9b3
    sig->m_pcRoomPlayTimeRewardCnt = rewardCnt;              // +0xc9b7
    return true;
