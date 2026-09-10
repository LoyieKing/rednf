bool CUser::make_basic_info(char* buf, char type)   // b28c-L5 SIGFIX（P174 串 bool）
{
    // 2026-09-10（G3CUser-B 负结论·余量登记）：栈槽集对照（报告 ORIG 汇编 vs 我方
    //   重建汇编）：ORIG 50 槽 / 我方 59 槽、整体平移 +0x28，平移后**仍多余 9 个
    //   具名槽**（-0x13c/-0x12c/-0x120/-0x11c/-0xb1/-0x65/-0x49/-0x3c/-0x31 等）。
    //   即本件确有可削的具名局部，但函数 2149 insn、字段写入 500+ 站点，定位需
    //   逐块比对；本轮未落（避免无直证改动）。建议后续单件专批：按上述槽地址回查
    //   我方汇编 store 点 → 对应源码具名量，按 P0/P183 内联。
    // b28d-solo-CUser p167 复核（ORIG 407 / OUR 404，本轮 10→3 缺口）：
    //   已修 —— creatureName/guild->m_guildName/GetGuildName 长度具名复用（消 strlen×3）、
    //   teamName getter 三次独立调用（P183）、blood->GetEnterCount 具名复用（消 EXTRA×1）、
    //   两处 getCurCharacR 指针缓存改逐点重调（P183，消 MISSING×2）。
    //   余 3 缺口待后续：MISSING put_byte×2 + put_int×1（ORIG 有我们无的字段写入段，
    //   疑某条件分支 3 个 put；需逐块比对 0x0865ae63/0x0865b00d 两区）。
    InterfacePacketBuf& pkt = *(InterfacePacketBuf*)buf;
    PacketGuard& pg = *(PacketGuard*)buf;

    if (type == 1)
    {
        // ---- 分支 1：ENUM_USERINFO_ADDITION（ORIG +0xbb9）----
        if (getCurCharacR() == 0)
        {
            LogManager::logFormat(1, "user.cpp",
                                  __PRETTY_FUNCTION__,
                                  0x31c3,
                                  "Error ENUM_USERINFO_ADDITION, getCurCharacR() == 0");
            return 0;
        }
        lock();
        pkt.put_short((int)m_field704a8);
        int exp;
        if (G_GameWorld()->IsCharacterLevelRevisionChannel())
            exp = G_CDataManager()->get_level_exp(0x55);
        else
            exp = get_charac_exp();
        pkt.put_int(exp);
        make_charac_stat(pg);
        pkt.put_byte((int)GetCurExEquipSlotStat());
        ((CInventory*)getCurCharacInvenR())
            ->MakeEquipList(&pg, true, (ENUM_USERINFO)1, (ENUM_EQUIPSLOT)0);
        pkt.put_byte(GetCurCharacSkillTreeIndex());

        // 公会技能/成员双组块 ×2（保留索引-回填计数模式）
        const GuildDbInfoView* guild =
            (const GuildDbInfoView*)GetGuildDBInfo();
        unsigned char memberCount = guild->m_memberCount;
        {
            int countIdx = pkt.get_index();
            int count = 0;
            pkt.put_byte(0);
            for (int i = 0; i < 0xcc; ++i)
            {
                if (getCurCharacSkillR()[6 + (i + 0x20) * 2] != '\0')
                {
                    pkt.put_byte(
                        (int)(unsigned char)getCurCharacSkillR()[6 + (i + 0x20) * 2]);
                    pkt.put_byte(
                        (int)(char)getCurCharacSkillR()[6 + 1 + (i + 0x20) * 2]);
                    ++count;
                }
            }
            const GuildMemberEntryView* members =
                reinterpret_cast<const GuildMemberEntryView*>(guild->m_members);
            for (int j = 0; j < (int)memberCount; ++j)
            {
                if (getGuildMemberGrade() != 0 && getGuildMemberGrade() != 0
                    && members[j].m_grade > 0xc7)
                {
                    pkt.put_byte(members[j].m_grade);
                    pkt.put_byte((int)(char)members[j].m_position);
                    ++count;
                }
            }
            pkt.put_byte(countIdx, count);
        }
        {
            int countIdx = pkt.get_index();
            int count = 0;
            pkt.put_byte(0);
            for (int i = 0; i < 0xcc; ++i)
            {
                if (getCurCharacSkillR()[0xe + (i + 0xe8) * 2] != '\0')
                {
                    pkt.put_byte(
                        (int)(unsigned char)getCurCharacSkillR()[0xe + (i + 0xe8) * 2]);
                    pkt.put_byte(
                        (int)(char)getCurCharacSkillR()[0xe + 1 + (i + 0xe8) * 2]);
                    ++count;
                }
            }
            const GuildMemberEntryView* members =
                reinterpret_cast<const GuildMemberEntryView*>(guild->m_members);
            for (int j = 0; j < (int)memberCount; ++j)
            {
                if (getGuildMemberGrade() != 0 && getGuildMemberGrade() != 0
                    && members[j].m_grade > 0xc7)
                {
                    pkt.put_byte(members[j].m_grade);
                    pkt.put_byte((int)(char)members[j].m_position);
                    ++count;
                }
            }
            pkt.put_byte(countIdx, count);
        }

        const user_creature::CCreatureMgr* creatureMgr =
            ((const CInventory*)getCurCharacInvenR())->GetCreatureMgrR();
        pkt.put_byte(creatureMgr->GetEquipedCreatureLevel());
        pkt.put_byte(6);
        for (int i = 0; i < 6; ++i)
            pkt.put_byte((int)(char)getDemensionInoutValue((char)i));
        if (!GlobalData::s_revengeDungeonMgr->IsOpenRevengeDungeon())
            pkt.put_byte(0);
        else
            pkt.put_byte((int)GetCurRevengeDungeonCount());
        pkt.put_byte((int)(char)getUltimateInoutValue(2));
        const TOD_UserState* tod = (const TOD_UserState*)GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)0xd);
        pkt.put_byte(tod->getEnterCount());
        int limit = (int)(char)G_CDataManager()->get_limit_inout_count(
            ENUM_BLOOD_TYPE_1);
        const CCharacBloodDungeon* blood =
            (const CCharacBloodDungeon*)GetCharacExpandData(
                (ENUM_CHARAC_EXPAND_TYPE)1);
        // b28d-solo-CUser p167：ORIG 0x865b623 单次 GetEnterCount 落 -0x48 槽后
        // 两处复用（P0 槽往返）⇒ 具名局部。
        const int bloodEnterCount = blood->GetEnterCount();
        if (limit - bloodEnterCount < 0)
            pkt.put_byte(0);
        else
            pkt.put_byte(limit - bloodEnterCount);
        ((CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb))
            ->makePacket_BuyingStatusList(pg, false);
        pkt.put_byte(GetManageLevel());
        unlock();
    }
    else if (type < 2)
    {
        if (type == 0)
        {
            // ---- 分支 0：ENUM_USERINFO_MINIMUM（ORIG +0x52）----
            if (getCurCharacR() == 0)
            {
                LogManager::logFormat(
                    1, "user.cpp", __PRETTY_FUNCTION__,
                    0x308c,
                    "Error ENUM_USERINFO_MINIMUM, getCurCharacR() == 0");
                return 0;
            }
            lock();
            pkt.put_short((int)(get_unique_id() & 0xffff));
            // ORIG 0x0865a..：put_int(strlen)/put_str/put_str 的 strlen 各自重调 getCurCharacName()。
            pkt.put_int((int)strlen(getCurCharacName()));
            pkt.put_str(getCurCharacName(), (int)strlen(getCurCharacName()));
            pkt.put_byte(get_charac_job());
            pkt.put_byte((int)getCurCharacGrowType());
            if (G_GameWorld()->IsCharacterLevelRevisionChannel())
                pkt.put_byte(0x55);
            else
                pkt.put_byte(get_charac_level());
            pkt.put_byte(get_pvp_grade());
            const PvpResultBasicView* pvp =
                (const PvpResultBasicView*)getPVPResultR();
            pkt.put_byte(pvp->m_field5c);
            pkt.put_byte(GetUserState());
            ((CInventory*)getCurCharacInvenR())
                ->MakeEquipList(&pg, false, (ENUM_USERINFO)0,
                                (ENUM_EQUIPSLOT)0);
            get_acc_id();
            pkt.put_byte(0);
            if (G_GameWorld()->IsSchoolPvPChannel() && GetSchoolNo() != 0)
                pkt.put_int((int)GetSchoolNo());
            if (G_GameWorld()->GetChannelType() == 0xe)
                pkt.put_int(GetCurCharacOnlinePreliminaryTeamId());
            else
                pkt.put_int(get_charac_guildkey());
            pkt.put_byte((int)IsPermissionPrivateStore());

            // ORIG 0x0865b..：每处 creature 访问重取 getCurCharacInvenR()->GetCreatureMgrR()
            // （P36/P44），creatureName 仍缓存（ORIG 亦缓存 local_9c）。
            pkt.put_int(
                ((const CInventory*)getCurCharacInvenR())->GetCreatureMgrR()
                    ->GetCreatureItemId());
            const char* creatureName =
                ((const CInventory*)getCurCharacInvenR())->GetCreatureMgrR()
                    ->GetCreatureName();
            // b28d-solo-CUser p167：ORIG 0x865a7b5 单次 strlen 落 -0x94 槽后复用
            //（put_int(len)+put_str(name,len)）⇒ 具名 len 局部（P0 槽往返证据）。
            const int creatureNameLen = (int)strlen(creatureName);
            pkt.put_int(creatureNameLen);
            pkt.put_str(creatureName, creatureNameLen);
            pkt.put_byte(
                ((const CInventory*)getCurCharacInvenR())->GetCreatureMgrR()
                        ->IsDieCreature()
                    ? 0
                    : 1);

            pkt.put_byte(isAffectedPremium((ENUM_PREMIUM_TYPE)0xc) ? 1 : 0);
            pkt.put_int(GetPCRoomNo());
            pkt.put_int(getCurrCharacBlackCount());

            const GuildDbInfoView* guild =
                (const GuildDbInfoView*)GetGuildDBInfo();
            if (G_GameWorld()->IsSchoolPvPChannel() && GetSchoolNo() != 0)
            {
                // ORIG：CLeagueMatch::GetPlayCountPerOneTeam() 读全局
                // nPlayerCountPerOneTeam_（+0x00 字节）→ put_byte(& 0xff)。
                pkt.put_byte((int)CLeagueMatch::GetPlayCountPerOneTeam());
                pkt.put_int((int)m_str8e080.length());
                pkt.put_str(m_str8e080.c_str(), (int)m_str8e080.length());
            }
            else if (G_GameWorld()->GetChannelType() == 0xe)
            {
                // ORIG：online_preliminary::COnlinePreliminary::GetPlayCountPerOneTeam()
                // （内部取 G_CDataManager()->GetPvPTeamCount()）→ put_byte(& 0xff)。
                pkt.put_byte(
                    (int)(online_preliminary::COnlinePreliminary::GetPlayCountPerOneTeam()
                          & 0xff));
                // b28d-solo-CUser p167：ORIG 0x865a99a/0x865a9c1/0x865a9d6 三次
                // 独立调用 getter（P183 禁缓存）：#1→strlen→put_int；
                // #2→strlen→#3→put_str。
                pkt.put_int(
                    (int)strlen(GetCurCharacOnlinePreliminaryTeamName()));
                pkt.put_str(
                    GetCurCharacOnlinePreliminaryTeamName(),
                    (int)strlen(GetCurCharacOnlinePreliminaryTeamName()));
            }
            else
            {
                pkt.put_byte((int)guild->m_field1b);
                // b28d-solo-CUser p167：ORIG 0x865aa1b 单次 strlen 复用。
                const int guildNameLen = (int)strlen(guild->m_guildName);
                pkt.put_int(guildNameLen);
                pkt.put_str(guild->m_guildName, guildNameLen);
            }

            pkt.put_int(GetCurCharacChaosPoint());
            pkt.put_byte(0xe < GetAge() ? 1 : 0);
            pkt.put_byte((int)(char)getPowerSide());
            pkt.put_byte((int)(char)isJoinPowerWar());
            pkt.put_byte(GetCurCharacExpertJobType());
            pkt.put_int(GetCurCharacExpertJobExp());
            pkt.put_byte((int)getDisguiseKind());
            pkt.put_short((int)getDisguiseIndex());
            pkt.put_byte((int)(char)get_charac_visible_values());

            if (G_GameWorld()->IsForbiddenChat(getCurCharacName()))
            {
                unsigned int left =
                    G_GameWorld()->GetLeftTimeOfForbiddenChat(getCurCharacName());
                if (left == 0)
                    G_GameWorld()->EnableChat(getCurCharacName());
                {
                    cMyTrace(__PRETTY_FUNCTION__, 0x3154, 0)
                        ("%s FCT1: %ld\n", getCurCharacName(), left);
                }
                left = left / 60000u;
                {
                    cMyTrace(__PRETTY_FUNCTION__, 0x3156, 0)
                        ("%s FCT2: %ld\n", getCurCharacName(), left);
                }
                pkt.put_int((int)left);
            }
            else
            {
                pkt.put_int(0);
            }
            if (G_GameWorld()->IsForbiddenMove(getCurCharacName()))
            {
                unsigned int left =
                    G_GameWorld()->GetLeftTimeOfForbiddenMove(getCurCharacName());
                if (left == 0)
                    G_GameWorld()->EnableMove(getCurCharacName());
                {
                    cMyTrace(__PRETTY_FUNCTION__, 0x3163, 0)
                        ("%s FMT1: %ld\n", getCurCharacName(), left);
                }
                left = left / 60000u;
                {
                    cMyTrace(__PRETTY_FUNCTION__, 0x3165, 0)
                        ("%s FMT2: %ld\n", getCurCharacName(), left);
                }
                pkt.put_int((int)left);
            }
            else
            {
                pkt.put_int(0);
            }

            pkt.put_int((int)getCurCharacHelpAbuseComputedRatio());
            // b28d-solo-CUser p167：ORIG 0x865ae5d-0x865ae7c 每使用点重调
            // getCurCharacR()（P183 禁指针缓存）⇒ 内联视图 cast。
            pkt.put_byte((int)((const CurCharacByteView*)getCurCharacR())
                             ->m_field75);
            pkt.put_short((int)getCurCharacUsedFatigue());
            pkt.put_byte((int)GetServerGroup());
            pkt.put_short((int)(short)GetUserPowerWarPoint());
            pkt.put_byte(GlobalData::s_power_manager->GetRankingByCharacNo(
                (unsigned int)getCurCharacNo()));
            pkt.put_short((int)(short)GetCurCharacEmotion());
            pkt.put_byte((int)(char)GetCurCharacSkillTreeIndex());
            // b28d-CUser p167 真缺口：ORIG 0x0865af90/0x0865afa5 为 if/else **两个**
            //   put_byte 调用点（`test %al; je <put_byte(0)>`），三目只发一个。
            if (IsReturnUser() != 0)
                pkt.put_byte(1);
            else
                pkt.put_byte(0);
            char linkType = getCharacLinkType();
            pkt.put_byte((int)getLinkCharacSlotIndex());
            pkt.put_byte((int)linkType);
            unlock();
        }
    }
    else if (type == 2)
    {
        // ---- 分支 2：账号角色列表（ORIG +0x19e3；无当前角色守卫）----
        lock();
        pkt.put_byte((int)getCharacSlotLimit());
        pkt.put_byte((int)getSlotEffectCount());
        pkt.put_byte(GetManageLevel());
        pkt.put_int(GetManagePoint());
        int written = 0;
        int countIdx = pkt.get_index();
        pkt.put_short(written);
        const int nowSec = GlobalData::s_systemTime_.getCurSec();
        for (int i = 0; i < (int)m_characList.size(); ++i)
        {
            if (G_GameWorld()->GetChannelType() == 0xe
                && (unsigned int)reinterpret_cast<CharacListElemView&>(m_characList[i]).m_characNo != getOnlinePreliminaryCharacNo())
                continue;
            pkt.put_short(written);
            ++written;
            pkt.put_int((int)strlen(reinterpret_cast<CharacListElemView&>(m_characList[i]).m_name));
            pkt.put_str(reinterpret_cast<CharacListElemView&>(m_characList[i]).m_name, (int)strlen(reinterpret_cast<CharacListElemView&>(m_characList[i]).m_name));
            bool special = checkSpecialChar(reinterpret_cast<CharacListElemView&>(m_characList[i]).m_name) != false
                           || checkSpecialCharName(reinterpret_cast<CharacListElemView&>(m_characList[i]).m_name) != false;
            pkt.put_byte(special ? 1 : 0);
            pkt.put_byte((int)(char)reinterpret_cast<CharacListElemView&>(m_characList[i]).m_job);
            pkt.put_byte((int)(char)reinterpret_cast<CharacListElemView&>(m_characList[i]).m_field29);
            pkt.put_byte((int)reinterpret_cast<CharacListElemView&>(m_characList[i]).m_level);
            pkt.put_byte(0);
            if (!(nowSec < reinterpret_cast<CharacListElemView&>(m_characList[i]).m_expireSec))
                m_characList[i].m_fieldE66 = 0;   // +0xe66 ≡ ElemView::m_reenterable（Main 视图 cast 反模式修法：具名直访问）
            else
                m_characList[i].m_fieldE66 = 1;
            if (reinterpret_cast<CharacListElemView&>(m_characList[i]).m_viewState == 0xff)
                pkt.put_byte(8);
            else
                pkt.put_byte(reinterpret_cast<CharacListElemView&>(m_characList[i]).m_reenterable != 0 ? 4 : 0);
            ((CInventory*)reinterpret_cast<CharacListElemView&>(m_characList[i]).m_inventory)
                ->MakeEquipList(&pg, false, (ENUM_USERINFO)2,
                                (ENUM_EQUIPSLOT)0);
            get_acc_id();
            pkt.put_byte(0);
            pkt.put_int(get_charac_guildkey());
            pkt.put_byte((int)IsPermissionPrivateStore());
            pkt.put_byte(isAffectedPremium((ENUM_PREMIUM_TYPE)0xc) ? 1 : 0);
            pkt.put_int(GetPCRoomNo());
            pkt.put_byte((int)(char)reinterpret_cast<CharacListElemView&>(m_characList[i]).m_fielde40);
            pkt.put_byte((int)reinterpret_cast<CharacListElemView&>(m_characList[i]).m_field1240);
            pkt.put_byte((int)(char)reinterpret_cast<CharacListElemView&>(m_characList[i]).m_field1246);
            if (reinterpret_cast<CharacListElemView&>(m_characList[i]).m_forbidPlayFlag == '\0')
            {
                pkt.put_int(0);
            }
            else if (reinterpret_cast<CharacListElemView&>(m_characList[i]).m_forbidPlayLeftSec < 1)
            {
                pkt.put_int(0);
                m_characList[i].m_pad83 = 0;      // +0x83 ≡ ElemView::m_forbidPlayFlag
                m_characList[i].m_field84 = 0;    // +0x84 ≡ ElemView::m_forbidPlayLeftSec
                // P259：ORIG 无 Stream* 具名槽（Acquire 结果 %eax 直传 C1）。
                CStreamGuard guard(
                    GlobalData::s_stream_pool->Acquire("user.cpp", 0x344c), true);
                // ORIG 0x0865c4f9/0x0865c524/0x0865c53e：`*guard << v`（operator*
                // 链产生 deEv）+ `guard->GetInBuffer`（ptEv 链），P103/P132。
                (*guard)->operator<<(0x93);
                (*guard)->operator<<((int)GetUID());
                // ORIG 0x0865b40b：GetInBuffer<SIG_FORBID_USER_TO_PLAY>()（真 SIG 类型
                // 保证 mangled `GetInBufferI23SIG_FORBID_USER_TO_PLAYEEPT_v` 匹配）；
                // 前 9 字节 payload 借 SigForbidUserToPlayBody 视图写。
                SIG_FORBID_USER_TO_PLAY* sig =
                    guard.operator->()->GetInBuffer<SIG_FORBID_USER_TO_PLAY>();
                SigForbidUserToPlayBody* body =
                    (SigForbidUserToPlayBody*)sig;
                memset(body, 0, 9);
                body->m_characNo = (unsigned int)reinterpret_cast<CharacListElemView&>(m_characList[i]).m_characNo;
                body->m_field04 = 0;
                body->m_field05 = 0;
                GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
            }
            else
            {
                int days = reinterpret_cast<CharacListElemView&>(m_characList[i]).m_forbidPlayLeftSec / 0x15180;
                // b28d-CUser p167 真缺口：ORIG 为 if/else 两个 put_int 调用点
                //   （Ghidra G101/G102；三目只发一个）。
                if (days < 1)
                    pkt.put_int(1);
                else
                    pkt.put_int(days);
            }
        }
        pkt.put_short(countIdx, written);
        unlock();
    }
    else if (type == 3)
    {
        // ---- 分支 3：ENUM_USERINFO_OTHER_CHARACTER（ORIG +0x127f）----
        if (getCurCharacR() == 0)
        {
            LogManager::logFormat(
                1, "user.cpp", __PRETTY_FUNCTION__,
                0x328c,
                "Error ENUM_USERINFO_OTHER_CHARACTER, getCurCharacR() == 0");
            return 0;
        }
        lock();
        pkt.put_short((int)m_field704a8);
        int exp;
        if (G_GameWorld()->IsCharacterLevelRevisionChannel())
            exp = G_CDataManager()->get_level_exp(0x55);
        else
            exp = get_charac_exp();
        pkt.put_int(exp);
        make_charac_stat(pg);
        pkt.put_byte((int)GetCurExEquipSlotStat());
        ((CInventory*)getCurCharacInvenR())
            ->MakeEquipList(&pg, true, (ENUM_USERINFO)3, (ENUM_EQUIPSLOT)0);
        pkt.put_byte(GetCurCharacSkillTreeIndex());

        const GuildDbInfoView* guild = (const GuildDbInfoView*)GetGuildDBInfo();
        unsigned char memberCount = guild->m_memberCount;
        {
            int countIdx = pkt.get_index();
            int count = 0;
            pkt.put_byte(0);
            for (int i = 0; i < 0xcc; ++i)
            {
                if (getCurCharacSkillR()[6 + (i + 0x20) * 2] != '\0')
                {
                    pkt.put_byte(
                        (int)(unsigned char)getCurCharacSkillR()[6 + (i + 0x20) * 2]);
                    pkt.put_byte(
                        (int)(char)getCurCharacSkillR()[6 + 1 + (i + 0x20) * 2]);
                    ++count;
                }
            }
            const GuildMemberEntryView* members =
                reinterpret_cast<const GuildMemberEntryView*>(guild->m_members);
            for (int j = 0; j < (int)memberCount; ++j)
            {
                if (getGuildMemberGrade() != 0 && getGuildMemberGrade() != 0
                    && members[j].m_grade > 0xc7)
                {
                    pkt.put_byte(members[j].m_grade);
                    pkt.put_byte((int)(char)members[j].m_position);
                    ++count;
                }
            }
            pkt.put_byte(countIdx, count);
        }
        {
            int countIdx = pkt.get_index();
            int count = 0;
            pkt.put_byte(0);
            for (int i = 0; i < 0xcc; ++i)
            {
                if (getCurCharacSkillR()[0xe + (i + 0xe8) * 2] != '\0')
                {
                    pkt.put_byte(
                        (int)(unsigned char)getCurCharacSkillR()[0xe + (i + 0xe8) * 2]);
                    pkt.put_byte(
                        (int)(char)getCurCharacSkillR()[0xe + 1 + (i + 0xe8) * 2]);
                    ++count;
                }
            }
            const GuildMemberEntryView* members =
                reinterpret_cast<const GuildMemberEntryView*>(guild->m_members);
            for (int j = 0; j < (int)memberCount; ++j)
            {
                if (getGuildMemberGrade() != 0 && getGuildMemberGrade() != 0
                    && members[j].m_grade > 0xc7)
                {
                    pkt.put_byte(members[j].m_grade);
                    pkt.put_byte((int)(char)members[j].m_position);
                    ++count;
                }
            }
            pkt.put_byte(countIdx, count);
        }

        const user_creature::CCreatureMgr* creatureMgr =
            ((const CInventory*)getCurCharacInvenR())->GetCreatureMgrR();
        pkt.put_byte(creatureMgr->GetEquipedCreatureLevel());
        // ORIG 0x0865c..：每字段重调 getPVPResultR()（P44），共 4 次。
        pkt.put_int(((const PvpResultBasicView*)getPVPResultR())->m_totalWinCount);
        pkt.put_int(((const PvpResultBasicView*)getPVPResultR())->m_field04);
        pkt.put_int(((const PvpResultBasicView*)getPVPResultR())->m_pvpGrade);
        pkt.put_byte(((const PvpResultBasicView*)getPVPResultR())->m_field5c);
        // b28d-solo-CUser p167：ORIG 0x865bc41/0x865bc65/0x865bc89 每字段重调
        // getCurCharacR()（P183 禁指针缓存）。
        pkt.put_byte((int)(char)((const CurCharacByteView*)getCurCharacR())
                         ->m_field39);
        pkt.put_byte((int)(char)((const CurCharacByteView*)getCurCharacR())
                         ->m_field3a);
        pkt.put_byte((int)(char)((const CurCharacByteView*)getCurCharacR())
                         ->m_field3b);
        pkt.put_int((int)getCurCharacHelpAbuseComputedRatio());
        pkt.put_short((int)(short)GetUserPowerWarPoint());
        pkt.put_int(GetGuildPowerWarPoint());
        // b28d-solo-CUser p167：ORIG 0x865bd0b/0x865bd36 GetGuildName 两次 + 单次
        // strlen 落 -0x38 槽 ⇒ 长度具名、getter 不缓存（P183）。
        const int guildNameLen = (int)strlen(GetGuildName());
        pkt.put_int(guildNameLen);
        pkt.put_str(GetGuildName(), guildNameLen);
        pkt.put_byte((int)GetGuildLevel());
        pkt.put_byte((int)(char)getPowerSide());
        // b28d-CUser p167 真缺口：ORIG 0x0865bdb4/0x0865bdc9 为 if/else **两个**
        //   put_byte 调用点（`test %al; je <put_byte(0)>`），三目只发一个。
        if (IsExistGuildAgit() != 0)
            pkt.put_byte(1);
        else
            pkt.put_byte(0);
        ((CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb))
            ->makePacket_BuyingStatusList(pg, false);
        pkt.put_byte(GetManageLevel());
        unlock();
    }
    return 1;
