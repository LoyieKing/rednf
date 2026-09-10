bool CUser::update_pvp_point(int v)   // b28c-L5 SIGFIX（P174）
{
    if (v == 0)
        return 1;
    if (v < 0)
    {
        if (v < -100000 || v > 100000)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x35d6, 5)
                ("pvp_exp_error5 %d, charac_no(%d), grade(%d), exp(%d)",
                  v, get_charac_no(-1), get_pvp_grade(),
                  getPVPResultRefR()->m_expPoint);
        }
        if (getPVPResultRefR()->m_pvpGrade < 10)
            return 1;
        // ORIG 0x0865d0bb：GetRepeatEvent(0x36) 后直接解引用 IsEventing，无判空。
        if (GlobalData::s_event_manager->GetRepeatEvent(0x36)
                ->IsEventing((CUser*)0))
        {
            // P183：ORIG 两次独立 getPVPResultRefR() 重物化（asm +0x106/+0x119），
            // 禁 int grade 缓存变元。
            if (getPVPResultRefR()->m_pvpGrade >= 10 &&
                getPVPResultRefR()->m_pvpGrade <= 0x13)
                v = (int)(0.5f * (float)v);
        }
        v = (int)(0.9f * (float)v);
        float revision =
            G_CDataManager()->m_serverParameter.GetPvPPenaltyRevision(
                getPVPResultRefR()->m_pvpGrade);
        v = (int)(revision * (float)v);
        if (v < -100000 || v > 100000)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x35ed, 5)
                ("pvp_exp_error6 %d, charac_no(%d), grade(%d), exp(%d)",
                  v, get_charac_no(-1), get_pvp_grade(),
                  getPVPResultRefR()->m_expPoint);
        }
    }
    // 整合 PvP 基础渠道且渠道类型 0 → 打七折。
    if (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
        G_CDataManager()->GetPvPChannelType() == 0)
    {
        v = (v * 70) / 100;
    }
    // 0xf 频道正向积分超过当前渠道最高档 → 按档位差衰减。
    if (G_GameWorld()->GetChannelType() == 0xf && v > 0)
    {
        int diff = getPVPResultRefR()->m_pvpGrade -
                   G_CDataManager()->GetMaxGradePvPChannel();
        if (diff > 0)
        {
            int penalty =
                G_CDataManager()->m_serverParameter.GetPvPChannelGradePenalty(
                    diff);
            v = (v * penalty) / 100;
        }
    }
    if (v < -100000 || v > 100000)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x3607, 5)
            ("pvp_exp_error7 %d, charac_no(%d), grade(%d), exp(%d)",
              v, get_charac_no(-1), get_pvp_grade(),
              getPVPResultRefR()->m_expPoint);
    }
    PvP_Room* room = (PvP_Room*)GetPVPRoom();
    if (room == 0)
        return 1;
    int seat = room->get_user_seat(this);
    if (room->IsPvpObserver(seat))
        return 1;
    bool inMission = has_within_Mission();
    if (!inMission)
    {
        if (G_GameWorld()->GetChannelType() == 0xf &&
            acceptable_within_mission() && v > 0)
            v = 0;
    }
    else
    {
        PacketGuard packet;
        if (v < 1)
        {
            if (v < 0)
            {
                Remove_Mission((short)0x1b);
                Remove_Mission((short)7);
                ((InterfacePacketBuf*)&packet)->put_header(0, 0x158);
                ((InterfacePacketBuf*)&packet)->put_byte(1);
                ((InterfacePacketBuf*)&packet)->finalize(true);
                Send(packet);
            }
        }
        else if (G_GameWorld()->GetChannelType() == 0xf)
        {
            v = 0;
        }
        else
        {
            Remove_Mission((short)0x1b);
            Remove_Mission((short)7);
            ((InterfacePacketBuf*)&packet)->put_header(0, 0x158);
            ((InterfacePacketBuf*)&packet)->put_byte(0);
            ((InterfacePacketBuf*)&packet)->finalize(true);
            Send(packet);
        }
    }
    if (v < -100000 || v > 100000)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x3653, 5)
            ("pvp_exp_error8 %d, charac_no(%d), grade(%d), exp(%d)",
              v, get_charac_no(-1), get_pvp_grade(),
              getPVPResultRefR()->m_expPoint);
    }
    getPVPResultRefW()->m_expPoint += v;
    if (getPVPResultRefR()->m_expPoint < 0)
    {
        LogManager::logFormat(1, "user.cpp", __PRETTY_FUNCTION__,
                              0x365c, "PVP correct now<%d> point<%d>",
                              getPVPResultRefR()->m_expPoint, v);
        getPVPResultRefW()->m_expPoint = 0;
    }
    if (getPVPResultRefR()->m_pvpGrade >= 0x1e)
        return 1;
    if (!has_within_Mission() && G_GameWorld()->GetChannelType() == 0xf)
        acceptable_within_mission();
    int newGrade =
        GlobalData::s_ref_pvp_grade->GetPvpGrade(
            getPVPResultRefR()->m_expPoint);
    if (newGrade < 1)
    {
        LogManager::logFormat(1, "user.cpp", __PRETTY_FUNCTION__,
                              0x36bc, "PVP Grade Incorrect %d",
                              getPVPResultRefR()->m_expPoint);
    }
    else
    {
        // ORIG 0x0865d9..：newGrade<1 时无早退，走 else 块（Ghidra C 结构）。
        if (newGrade < getPVPResultRefR()->m_pvpGrade)
        {
            // 降档保护：grade∈[10,0xf] 时按当前档下限回填积分。
            // P183：每处字段访问独立 getPVPResultRefR() 重物化（ORIG 3 处额外 R 调用），
            // 禁用缓存变元（int grade 缓存即缺 3 call 语义缺口）。
            if (getPVPResultRefR()->m_pvpGrade >= 10 &&
                getPVPResultRefR()->m_pvpGrade <= 0xf)
            {
                getPVPResultRefW()->m_expPoint =
                    GlobalData::s_ref_pvp_grade->GetPvpPoint(
                        getPVPResultRefR()->m_pvpGrade) + 1;
                newGrade = getPVPResultRefR()->m_pvpGrade;
            }
        }
        int oldGrade = getPVPResultRefR()->m_pvpGrade;
        getPVPResultRefW()->m_pvpGrade = newGrade;
        if (has_within_Mission() && getPVPResultRefR()->m_pvpGrade > oldGrade)
            getPVPResultRefW()->m_pvpGrade = oldGrade;   // 任务中禁止升档
        if (getPVPResultRefR()->m_pvpGrade != oldGrade)
        {
            PacketGuard packet;
            getCurCharacQuestR()->get_quest_info(
                reinterpret_cast<char*>(&packet));
            if (G_GameWorld()->GetChannelType() != 0xf)
            {
                Remove_Mission((short)0x1e);
                CMissionList_Charac* list = (CMissionList_Charac*)GetCharacExpandData(
                    (ENUM_CHARAC_EXPAND_TYPE)8);
                list->MakeMissionList_JustKind(*this, 0x1e);
            }
            Send(packet);
        }
    }
    return 1;
