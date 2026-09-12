// 0x0865cfd8
// ORIG：PvP 积分增减。负值：事件(0x36)进行且 grade∈[10,19] 半额（0.5f）→ 0.9f →
// GetPvPPenaltyRevision 三级衰减；整合 PvP 基础渠道 ×70%；0xf 频道超档按
// GetPvPChannelGradePenalty 衰减；任务内限制升降档；重算 grade 并在变更时
// §6.2 巨件核销：P167 102/102 [0缺0多]；gap 641→645（our+4）；残差归因 N82
// 槽号错位族（x87 ROUND 岛跨版本形态差），语义完整仅剩 codegen 差，N 封顶
// （详见 docs/asm_translation_patterns.md 负例表）。
// 重发任务信息包并重建 0x1e 任务列表。
// b28d-lane5 N封顶: P168a/P248 setcc 物化方向; N82/P112 槽号分配 | ORIG 0x0865cfd8 | _ZN5CUser16update_pvp_pointEi
bool CUser::update_pvp_point(int v)   // b28c-L5 SIGFIX（P174）
{
    // 2026-09-10 gA（A 组）负结论 N（ORIG 641 vs 我方 651）：ORIG 0x0865cfd8 逐块比对
    //   无字段/调用缺口；差集特征为 (a) cMyTrace 临时槽 -0x74(ORIG) vs -0x7c(我方)
    //   ——同帧 0xbc 下的槽分配策略差（N82）；(b) 我方函数体 +0x44 字节（跳转目标
    //   0x17421 vs 0x865d97b，全为等长块平移）；字符串/浮点常量差异系重定位伪差
    //   （`flds 0x8cf396c` vs `flds 0x165c`）⇒ 维持 P167/P183 已收敛源形，判 N。
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
    if (!has_within_Mission())
    {
        // [R20a11] §26.2/P360：ORIG 0x0865d5a4 段为**逐条件 setcc 物化**
        //  （`mov $0xf,%eax` 比较后 `sete %al; test %al,%al; je`，续以
        //    0x0865d5c6 `test %al,%al; je`、0x0865d5ca `cmpl $0x0,0xc(%ebp); jle`
        //    直落 `movl $0x0,0xc(%ebp)`）；`&&` 链在 4.4 恒产「链首直跳 + 链尾
        //    mov $1/jmp/mov $0/test/je 融合岛」（我方 0x0848435d..0x08484372 实测）
        //    ⇒ 拆嵌套 if 复现逐条件物化（P360「嵌套 if 内层同形」）。
        if (G_GameWorld()->GetChannelType() == 0xf)
        {
            if (acceptable_within_mission())
            {
                if (v > 0)
                    v = 0;
            }
        }
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
}