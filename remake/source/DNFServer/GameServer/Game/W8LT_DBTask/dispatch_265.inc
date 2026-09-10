bool DB_LoadBreakAwayRewardData::dispatch(int userIdx, int slot,
                                          Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    unsigned int m_id = 0;
    unsigned int characNo = 0;
    // ORIG 0x084392dc：`call G_CEnvironment;mov 0x378;→-0x20` 夹在第 2、3
    // 个清零槽之间（-0x4c 后、-0x50 前）⇒ 声明位=characNo 后。
    int serverId = G_CEnvironment()->m_serverEnvir.m_serverGroup;
    unsigned int lastRewardTime = 0;
    int questTime = 0;
    int luckPoint = 0;
    int rewardOrder = 0;
    unsigned int addInfo = 0;
    time_t curSec = (time_t)GlobalData::s_systemTime_.getCurSec();
    struct tm tmBuf;
    localtime_r(&curSec, &tmBuf);
    // ORIG 0x08439…：`movl $0,-0x6b; movw $0,-0x67; movb $0,-0x65` 逐段
    // 清零 = 声明初始化形（P4/29），非 memset 调用。
    char ym[7] = { 0 };
    sprintf(ym, "%d%02d", tmBuf.tm_year + 0x76c, tmBuf.tm_mon + 1);
    *stream >> m_id;
    *stream >> characNo;
    *stream >> lastRewardTime;
    *stream >> questTime;
    *stream >> luckPoint;
    *stream >> rewardOrder;
    *stream >> addInfo;
    if (rewardOrder == 0)
    {
        db->set_query(
            "upDate churn_member_info set last_reward_time =%u, first_reward_time = %u, quest_time =%u, luck_point=%d, add_info=%d where m_id=%s",
            (unsigned int)curSec, (unsigned int)curSec,
            (unsigned int)curSec + 0x15180, luckPoint, 3,
            NumberToString(m_id, 0));
    }
    else if (rewardOrder == 1)
    {
        db->set_query(
            "upDate churn_member_info set last_reward_time =%u, luck_point=%d where m_id=%u",
            (unsigned int)curSec, luckPoint, m_id);
    }
    else if (rewardOrder == 2)
    {
        db->set_query(
            "upDate churn_member_info set last_reward_time =%u, second_reward_time = %u, quest_time =%u, luck_point=%d, add_info=%d where m_id=%s",
            (unsigned int)curSec, (unsigned int)curSec,
            (unsigned int)curSec + 0x15180, luckPoint, 3,
            NumberToString(m_id, 0));
    }
    else if (rewardOrder == 3)
    {
        db->set_query(
            "upDate churn_member_info set last_reward_time =%u, luck_point=%d where m_id=%s",
            (unsigned int)curSec, luckPoint, NumberToString(m_id, 0));
    }
    else if (rewardOrder == 4)
    {
        db->set_query(
            "upDate churn_member_info set last_reward_time =%u, luck_point=%d, add_info=%d where m_id=%s",
            (unsigned int)curSec, luckPoint, -1, NumberToString(m_id, 0));
    }
    else
    {
        return 0;
    }
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x871e, 5)(
            "BREAK_AWAY : ABNORMAL DB_LoadBreakAwayRewardData REWARD!!");
        return 0;
    }
    db->set_query(
        "inSert into churn_reward_history_%s(m_id,occ_time,server_id,charac_no,item_id,add_info,luck_point,cera,reward_order) values(%s, from_unixtime(%u), %d, %u, %u, %d, %d, %u, %d)",
        ym, NumberToString(m_id, 0), (unsigned int)curSec, serverId,
        characNo, lastRewardTime, questTime, luckPoint, addInfo, rewardOrder);
    if (!db->exec(true))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x8740, 5)(
            "BREAK_AWAY : ABNORMAL DB_LoadBreakAwayRewardData LOG!!");
        return 0;
    }
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8745), true);
    guard.operator*()->operator<<(0x143);
    guard.operator*()->operator<<(slot);
    // [Lane31W8LTDBTaskView] 权威头具名直访问（ORIG RHS-first，避免 cast 形 base 先载）。
    SIG_BREAK_AWAY_PREVENT_MESSAGE* out =
        guard->GetInBuffer<SIG_BREAK_AWAY_PREVENT_MESSAGE>();
    if (rewardOrder == 0 || rewardOrder == 2)
        out->m_nextRewardTime = (unsigned int)curSec + 0x15180;
    out->m_rewardOrder = rewardOrder;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
