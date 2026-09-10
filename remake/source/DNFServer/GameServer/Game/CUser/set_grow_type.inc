void CUser::set_grow_type(unsigned char firstGrow, unsigned char secondGrow,
                          char* name, eChangeGrowTypeReason reason)
{
    if (firstGrow >= 6 || secondGrow >= 3)
    {
        LogManager::logFormat(1, "user.cpp",
            __PRETTY_FUNCTION__,
            0x713b, "User %s - grow type < 1 || grow type >= GROW_TYPE_NUMBER",
            getCurCharacName());
        return;
    }

    if (secondGrow == 0)
    {
        // ORIG：主职业变更 → 使命列表 kind 0x1f 移除、0x20 重建。
        CMissionList_Charac* ml =
            (CMissionList_Charac*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
        ml->Remove_Mission(0x1f);
        ml->MakeMissionList_JustKind(*this, 0x20);
    }
    if (reason == (eChangeGrowTypeReason)1 || secondGrow == 0)
    {
        CQuestShop* questShop =
            (CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb);
        if (questShop != 0)
        {
            questShop->clearQP(this);
            questShop->sendCharacQp(this, 1);
        }
    }

    // ORIG:if/else 两分支各自调用 getCurCharacGrowType/getCurCharacVill/get_charac_job。
    if (secondGrow == 0)
    {
        // 2026-09-10（G3CUser-B，P0 槽-局部律）：ORIG 0x086788fc 段
        //   `call getCurCharacGrowType; movsbl %al,%edi`（oldGrow **无槽**、%edi 保活），
        //   紧随 `movzbl -0x10c(%ebp),%esi`（firstGrow 参数槽）与 getCurCharacVill
        //   的 `movsbl %al,%ebx` ⇒ 原码把 getCurCharacGrowType() 直接写在实参位，
        //   我方 `char oldGrow` 具名量 ORIG 无（且其 1 字节槽另占帧）。
        m_historyLog.ChangeGrowType(
            get_charac_job(), (int)(signed char)getCurCharacVill(),
            (int)firstGrow, (int)(signed char)getCurCharacGrowType(), 1, reason);
    }
    else
    {
        // 2026-09-10（G3CUser-B，P0）：同上一分支 —— ORIG 0x0867897e
        //   `call getCurCharacGrowType; movsbl %al,%edi`（无槽），0x08678998
        //   `movsbl %al,%ebx`（vill 无槽）⇒ 内联。
        m_historyLog.ChangeGrowType(
            get_charac_job(), (int)(signed char)getCurCharacVill(),
            (int)secondGrow, (int)(signed char)getCurCharacGrowType(), 2, reason);
    }
    InterfacePacketBuf* buf = (InterfacePacketBuf*)name;
    setCurCharGrowthType((char)firstGrow, (char)secondGrow);
    m_characterView.enableSaveCharacView();
    if (secondGrow == 0)
    {
        // ORIG：主职业切换重算属性；角色选择视口（state==3）回 ADDITION 包。
        adjust_charac_stat();
        if (m_chState == 3)
        {
            PacketGuard pg;
            InterfacePacketBuf* pkt = (InterfacePacketBuf*)&pg;
            pkt->clear();
            pkt->put_header(0, 2);
            pkt->put_byte(1);
            pkt->put_short(1);
            make_basic_info((char*)pkt, (char)1);
            pkt->finalize(true);
            Send(pg);
        }
    }

    std::vector<std::pair<int, int> > giveVec;
    // 配对（D06 定义侧 2026-09-10 已 bool 化；A17 spec + ORIG 0x08678c6c 消费形
    //   `call get_give_skill; mov %al,-0x3d(%ebp)` = 1 字节直存无归一）⇒ 消费端局部
    //   必须同为 bool（Main 净账必配对律），否则 4.4 恒插 test/setne 归一。
    bool giveOk = false;
    // ORIG:每 get_give_skill 站点独立调用 G_CDataManager() 并解引 +0x14。
    if (G_GameWorld()->IsPvPSkilTreeChannel())
    {
        giveOk = 1;
    }
    else if (reason == (eChangeGrowTypeReason)2 ||
             reason == (eChangeGrowTypeReason)0)
    {
        if (secondGrow == 0)
        {
            // ORIG：双树清空重建（addSkillOnCreateCharacter 树 A +0x46 / 树 B +0x1de）、
            // CSkillChanger 初始化 + send_skill_info 后按 get_give_skill(...,type 1)。
            // ORIG:clear/set_parent/addSkill 各自调用 getCurCharacSkillW()。
            ((SkillSlot*)getCurCharacSkillW())->clear_all_skills_both();
            ((SkillSlot*)getCurCharacSkillW())->set_parent(this);
            addSkillOnCreateCharacter(
                ((SkillSlot*)getCurCharacSkillW())->m_skillSlot, get_charac_job());
            addSkillOnCreateCharacter(
                ((SkillSlot*)getCurCharacSkillW())->m_skillSlot2, get_charac_job());
            {
                WongWork::CSkillChanger changer;
                changer.SkillInitialize(this, 0, false);
            }
            send_skill_info();
            giveOk = G_CDataManager()->m_characList->m_characs[get_charac_job()].get_give_skill(
                firstGrow, 0, giveVec, 1);
        }
        else
        {
            giveOk = G_CDataManager()->m_characList->m_characs[get_charac_job()].get_give_skill(
                firstGrow, secondGrow, giveVec, 2);
        }
    }
    else if (reason == (eChangeGrowTypeReason)1)
    {
        // ORIG:secondGrow==0 与否则两站点各自调用。
        if (secondGrow == 0)
        {
            giveOk = G_CDataManager()->m_characList->m_characs[get_charac_job()].get_give_skill(
                firstGrow, 0, giveVec, 1);
        }
        else
        {
            giveOk = G_CDataManager()->m_characList->m_characs[get_charac_job()].get_give_skill(
                firstGrow, secondGrow, giveVec, 0);
        }
    }
    if (giveOk == 1)
    {
        // ORIG:pair<int,int> 默认构造站点(local_5c)。
        std::pair<int, int> pairLocal5c;
        if (buf == 0)
        {
            // ORIG：直接应用 —— 每项双树 growtype_skill。
            for (size_t i = 0; i < giveVec.size(); ++i)
            {
                // ORIG:每字段读独立 operator[](second→first 序),ix ×4/迭代。
                int second0 = giveVec[i].second;
                int first0 = giveVec[i].first;
                ((SkillSlot*)getCurCharacSkillW())->growtype_skill(
                    get_charac_job(), first0, second0,
                    (ENUM_SKILL_TREE_KIND)0);
                int second1 = giveVec[i].second;
                int first1 = giveVec[i].first;
                ((SkillSlot*)getCurCharacSkillW())->growtype_skill(
                    get_charac_job(), first1, second1,
                    (ENUM_SKILL_TREE_KIND)1);
            }
        }
        else
        {
            // ORIG:树 0 / 树 1 两个显式循环(非 tree 变量循环)。
            {
                int countIdx = buf->get_index();
                int cnt = 0;
                buf->put_byte(0);
                for (size_t i = 0; i < giveVec.size(); ++i)
                {
                    const std::pair<int, int>& pr = giveVec[i];
                    int r = ((SkillSlot*)getCurCharacSkillW())->growtype_skill(
                        get_charac_job(), pr.first, pr.second,
                        (ENUM_SKILL_TREE_KIND)0);
                    if (r > 0)
                    {
                        buf->put_byte(r);
                        buf->put_byte(pr.first);
                        buf->put_byte(pr.second);
                        ++cnt;
                    }
                }
                buf->put_byte(countIdx, cnt);
            }
            {
                int countIdx = buf->get_index();
                int cnt = 0;
                buf->put_byte(0);
                for (size_t i = 0; i < giveVec.size(); ++i)
                {
                    const std::pair<int, int>& pr = giveVec[i];
                    int r = ((SkillSlot*)getCurCharacSkillW())->growtype_skill(
                        get_charac_job(), pr.first, pr.second,
                        (ENUM_SKILL_TREE_KIND)1);
                    if (r > 0)
                    {
                        buf->put_byte(r);
                        buf->put_byte(pr.first);
                        buf->put_byte(pr.second);
                        ++cnt;
                    }
                }
                buf->put_byte(countIdx, cnt);
            }
        }
        _UpdateUserInfo4Guild();
        // 2026-09-10（G3CUser-B）：ORIG 0x086790f4..0x0867917b（guild 段）——
        //   guildGrow 是唯一具名槽：0x0867910f `mov %eax,-0x118(%ebp)`（movsbl 后
        //   **4 字节** int 存 ⇒ 原码 `int guildGrow = getCurCharacGrowType();`）；
        //   level/characNo/accId **全无槽**（0x08679120 `movswl %ax,%edi`、
        //   0x08679136 `mov %eax,%esi`、0x08679143 `mov %eax,%ebx`，跨调用 callee-saved
        //   保活）；0x0867915d `call GetServerProxy` 结果**直接**作 this
        //   （0x08679178 `mov %eax,(%esp)`）——**无** `test %eax,%eax; je` 判空。
        //   ⇒ 删 guildLevel/guildCharacNo/guildAccId/guild 四具名量与 `if (guild != 0)`
        //   发明防御，实参内联，receiver 链式（ORIG 求值序：实参 → GetServerGroup →
        //   GetServerProxy → call）。
        int guildGrow = getCurCharacGrowType();
        GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup())
            ->SendCharLevelGrowType(get_acc_id(), get_charac_no(-1),
                                    (short)get_charac_level(), (short)guildGrow);
        // 2026-09-10（G3CUser-B）：monitor 段同构 —— monGrow 具名
        //   0x0867918e `movsbl %al,%eax; mov %eax,-0x114(%ebp)`（4 字节 int 槽）；
        //   0x0867919f `movswl %ax,%edi` / 0x086791b5 `mov %eax,%esi` /
        //   0x086791c2 `mov %eax,%ebx` 无槽；0x086791dc `call GetServerProxy` 结果
        //   0x086791f7 直传，无判空。
        int monGrow = getCurCharacGrowType();
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(GetServerGroup())
            ->SendCharLevelGrowType(get_acc_id(), get_charac_no(-1),
                                    (short)get_charac_level(), (short)monGrow);
        processGrowTypeEvent(firstGrow, secondGrow, reason);
    }
    else
    {
        LogManager::logFormat(1, "user.cpp",
            __PRETTY_FUNCTION__, 0x720c,
            "User %s - CUser::set_grow_type first_grow_type %d, second_grow_type %d",
            getCurCharacName(), (int)firstGrow, (int)secondGrow);
    }
