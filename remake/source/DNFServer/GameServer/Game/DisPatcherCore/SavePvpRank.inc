bool DisPatcher_ResPvpRank::SavePvpRank(CUser* user, MSG_RES_PVP_RANK& msg)
{
    // ORIG 0x081f05ea T（报告 DisPatcher_ResPvpRank/SavePvpRank.md 全文转写）：
    // 分数(0x89=视图+0x7c)/时间(0x8d=视图+0x80) 反作弊 → job 3/4-grow2 折算 → update_pvp_rank
    // → PVPRoom SetPVPResult/recv 链。
    // ORIG 0x081f05f2/0x081f05f8 双 `movl $0x0` 预清（P198③ 两句式声明）
    unsigned int score = 0;
    int playSec = 0;
    score = (unsigned int)msg.m_result.m_field7c;   // msg+0x89（视图 +0x7c）
    playSec = (int)msg.m_result.m_field80 / 1000;   // msg+0x8d（视图 +0x80），有符号除
    if (10000 < score)
    {
        unsigned int over = score / 10000;
        score = score % 10000;
        user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x76, 1, (int)over, (int)playSec);
    }
    // ORIG _DAT_08bd6320=0.5（job3）/ _DAT_08bd6328=0.8（其余）双精度折算（(int) 截断语义）
    bool reduced = (user->get_charac_job() == 3 ||
                    (user->get_charac_job() == 4 && user->getCurCharFirstGrowType() == 2));
    if (reduced)
    {
        score = (unsigned int)(0.5 * (double)score);
    }
    else
    {
        score = (unsigned int)(0.8 * (double)score);
    }
    // ORIG 折算后直接写回 score（-0x1c），无 adjusted 具名局部（P0）
    if (playSec < score)
    {
        user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x73, 1, (int)score, (int)playSec);
    }
    if (!user->update_pvp_rank(msg.m_result))
    {
        return false;
    }
    PvP_Room* room = static_cast<PvP_Room*>(user->GetPVPRoom());
    if (room == 0)
    {
        return false;
    }
    int seat = room->get_user_seat(user);
    room->SetPVPResult(seat, msg.m_result);
    if (!room->set_recv_pvp_rank_flag(user))
    {
        return false;
    }
    if (!room->recv_pvp_rank_all())
    {
        if (room->get_recv_pvp_rank_count() == 1)
        {
            room->insert_timer_recv_pvp_rank();
        }
    }
    else
    {
        room->send_pvp_end();
    }
    return true;
