bool CGameManager::insert_game_world(CUser* user)   // P139：ORIG prett 串 = `bool CGameManager::insert_game_world(CUser*)`
{
    // b28d-lane12: N82 槽平移封顶（orig=890 our=905 gap=-15；ndiff 831——大帧
    // 局部槽组 + 迭代器/Guard 分区差）。ORIG 0x08296d30。
    // b28d-solo-CGameManager 判词：DIFF·N82（ours 911 / orig 890，gap -21；帧 0x12c
    // vs ORIG 0x10c——大帧槽组分区差）。本轮修法（P0+P214①）：
    //   ①getCurCharacR() 判空去具名局部 ⇒ 复现 ORIG `call; test %eax,%eax;
    //     sete %al; test %al,%al; je`（原 `mov %eax,-0x54; cmpl; jne` 槽化）；
    //   ②bGuildPointDel 三态 if/else-if/else 改短路汇聚形（P214① w2 形）⇒
    //     复现 ORIG `jg L_false; …; jge L_false; mov $1; jmp; L_false: mov $0;
    //     test %al; je`（原 movb 槽 + cmpb 直比）。
    // **P167 CLEAR 193/193**
    // （旧判词记的 MISSING G_TimerQueue×4/getCurCharacR/make_pair<int,int>/
    // GetChannelType/IsEquipSlotSwitchChannel/getPunishTradeAlert/getCurCharacName 与
    // EXTRA getCurCharacLastPlayTick/make_pair<int&,int&>/std::move 已被后续修复清零，
    // 本波复验 0 missing / 0 extra）⇒ 语义闭合、仅剩 byte 层槽形差。
    // [K9GameRestA.CGameMgr12Run 2026-09-11] 本波真语义 bug×2（ORIG 直证 + 消费方
    //   `CUser::_isMatchedItem` ORIG 0x0867cf0c「pair.first==itemIdx」定 pair=(addInfo,
    //   count)）：vecA 两处 make_pair 实参序颠倒 ⇒ 已改 `make_pair(0xc84, 0)`
    //   （ORIG 0x08296e1a..0x08296e3c：-0x64 存 __x=0xc84、装载 `mov %edx,0x4(%esp)`）
    //   与 `make_pair((ENUM_HARDCODE_ITEM)0x4b0, 0)`（ORIG 0x08296e6e..0x08296e90：
    //   call 0x082b2c12 = make_pair<ENUM,int>，-0x4c 存 __x=0x4b0）。旧形 (0, id) 恒
    //   匹配不到槽 ⇒ 重置/删书请求全部落空。
    //   codegen：proxy 链（0x08297957 段）与 GetPICSMap()->clear()（0x08297836 段）
    //   去具名局部 ⇒ gap 901→892（fresh .o）；curSec2 内联实测到 891 但 verdict
    //   DIFF→NEAR 使 TU near 1→2 越闸门 ⇒ 已回滚（见该件内注）。
    //   残余 = 4.1 的 %edi/%esi/%ebx 实参保活角色 + `get_rand_int+600` 落 -0xec 槽
    //   （ORIG 0x08297a8a）vs 4.4 寄存器形 = N7/N82；p167 CLEAR 193=193。
    if (user == 0)
    {
        LogManager::logFormat(1, "App.cpp",
                              __PRETTY_FUNCTION__, 0xf20,
                              "insert_waiting_room pUser parameter is null.");
        return 0;
    }
    // b28d-solo-CGameManager 修法（P0）：ORIG 0x08296dXX `call getCurCharacR;
    // test %eax,%eax; sete %al; test %al,%al; je` —— 调用结果寄存器直比、无具名
    // 局部槽往返 ⇒ 表达式内联判空（原 `const void* characR = ...` 多 -0x54 槽 +
    // cmpl/jne 两段）。
    if (user->getCurCharacR() == 0)
    {
        LogManager::logFormat(1, "App.cpp",
                              __PRETTY_FUNCTION__, 0xf27,
                              "Not selected character(a reason is fast returning to character screen on client)");
        return 0;
    }
    int curSec = GlobalData::s_systemTime_.getCurSec();
    std::vector<std::pair<int, int> > vecA;
    std::vector<std::pair<int, int> > vecB;
    // b28d-solo-CGameManager 修法（P214① 汇聚块 w2 形）：ORIG 0x08296dXX =
    // `call getLastDeleteTime; cmp curSec; jg L_false; call getCurCharacLastPlayTick;
    // mov %eax,%ebx; call getLastDeleteTime; cmp %eax,%ebx; jge L_false; mov $1;
    // jmp; L_false: mov $0; test %al,%al; je` —— 短路链 bool 汇聚（无槽物化）；
    // 原 if/else-if/else 三态赋值恒产 `movb` 槽 + `cmpb $0` 直比。
    // b28d-laneCGM（P0）：ORIG 0x08296dXX 该 bool 无槽往返（`mov $1; jmp; mov $0;
    //   test %al,%al; je` 全在寄存器）⇒ 原码把短路链直接写进 if 条件，不是具名
    //   局部（具名形多 `mov %al,SLOT; cmpb $0x0,SLOT`）。
    if (curSec >= Inter_MonitorGuildPointDel::getLastDeleteTime() &&
        user->getCurCharacLastPlayTick() <
            Inter_MonitorGuildPointDel::getLastDeleteTime())
    {
        // P167：ORIG 调 make_pair<int,int>(int&&,int&&)（0x081b8d41）⇒ 实参为字面量
        // 右值；具名 a/b 左值会实例化 make_pair<int&,int&>（我们旧形 EXTRA）。
        // [K9GameRestA.CGameMgr12Run 2026-09-11 · 真语义 bug（P224 实参序）] ORIG
        //   0x08296e1a..0x08296e3c 直证：物化槽 -0x68=0（第二实参 __y）、-0x64=0xc84
        //   （第一实参 __x，装载 `lea -0x64; mov %edx,0x4(%esp)`）；ORIG 的
        //   `make_pair<int,int>` 体（0x081b8d41）以 0xc(%ebp)=__x 作 pair ctor 第一实参
        //   ⇒ 原码 = make_pair(**0xc84, 0**)。语义判据：消费方 `CUser::_isMatchedItem`
        //   （ORIG 0x0867cf0c「找 pair.first==itemIdx，返回 second」）⇒ pair = (addInfo,
        //   count)，旧形 (0, 0xc84) 恒匹配不到任何槽（重置失效）。
        vecA.push_back(std::make_pair(0xc84, 0));
    }
    if (user->getCurCharacGuildSecede() != 0)
    {
        // P167：ORIG 为 `make_pair<ENUM_HARDCODE_ITEM,int>` + 隐式转换构造
        // `pair<int,int>::pair<ENUM,int>(pair<ENUM,int>&&)` + push_back(pair&&)
        // ——匿名右值直传，无具名 p、无 std::move call（我们旧形 EXTRA 两项）。
        // [K9GameRestA.CGameMgr12Run 2026-09-11 · 真语义 bug（P224 实参序）] ORIG
        //   0x08296e6e..0x08296e90 直证：物化槽 -0x50=0（__y）、-0x4c=0x4b0（__x，
        //   装载 `lea -0x4c; mov %edx,0x4(%esp)`），且 `call 0x082b2c12` 符号 =
        //   `_ZSt9make_pairI18ENUM_HARDCODE_ITEMiE...`（T1=ENUM、T2=int）⇒ 原码 =
        //   `make_pair((ENUM_HARDCODE_ITEM)0x4b0, 0)`（0x4b0 = 公会经验书 addInfo，
        //   见下方 DB_GuildExpBookDelete::makeRequest）。旧形 (0, 0x4b0) 与
        //   _isMatchedItem 的 (first==addInfo) 语义相反 ⇒ 删除请求恒落空。
        vecA.push_back(std::make_pair((ENUM_HARDCODE_ITEM)0x4b0, 0));
        user->setCurCharacGuildSecede(0);
        DB_GuildExpBookDelete::makeRequest((unsigned int)user->get_charac_no(-1), 0, (ENUM_SERVER_GROUP)(size_t)user->GetServerGroup());
    }
    user->ResetItemByScript(vecA);
    // P167：ORIG 调 getCurCharacR()（0x08120432）后直读 +0x7b = m_lastPlayTick，
    // 不调 getCurCharacLastPlayTick（旧形 EXTRA +1）。
    // b28d-laneCGM（P0/P183）：lastPlay/env 均只用一次且 ORIG 无槽往返
    //   ⇒ 原码把 getter 表达式直接内联进实参位（具名形多 `mov REG,SLOT` 往返）。
    // [K9GameRestA.CGameMgr12Run 2026-09-11 · ORIG 直证 + gap 复核后回滚]
    //   ORIG 0x08296f15..0x08296f40：`movl $&s_systemTime_,(%esp); call getCurSec;
    //   mov %eax,%ebx`（值跨后续两次调用保活于 **%ebx**，无槽）。本波实测：内联该
    //   表达式后本件 891 insn ⇒ verdict DIFF→**NEAR**、TU near 1→2（越闸门）。
    //   ORIG 的 %ebx 保活是 4.1 分配器行为，4.4 恒给槽往返 ⇒ 维持具名 curSec2
    //   （N7/N82 封顶）。三条内联（pics/proxy×2）保留：它们把 gap 从 -11 收到 -2
    //   且不改 verdict。
    int curSec2 = GlobalData::s_systemTime_.getCurSec();
    if (CheckDailyScheduleTime(
            G_CEnvironment()->m_serverEnvir.m_fatigueTime,  // ORIG env+0x37c
            (long)user->getCurCharacR()->m_lastPlayTick, curSec2))  // _Charac_info +0x7b（CharLastPlayTick）
    {
        std::vector<std::pair<int, int> > vecC;
        user->deleteDailyStackableItem(vecC, 3, 8);
        user->deleteDailyStackableItem(vecC, 0x39, 0x68);
    }
    user->deleteSpecificItem(vecA, vecB);
    if (!user->send_itemspace(0xc))
    {
        LogManager::logFormat(1, "App.cpp",
                              __PRETTY_FUNCTION__, 0xf58,
                              "CGameManager::insert_game_world, pUser(%s)->send_itemspace( ENUM_ITEMSPACE_ACCOUNT_CARGO ) return FAILED",
                              NumberToString(user->get_acc_id(), 0));
        return 0;
    }
    if (!user->send_itemspace(0))
    {
        LogManager::logFormat(1, "App.cpp",
                              __PRETTY_FUNCTION__, 0xf5f,
                              "CGameManager::insert_game_world, pUser(%s)->send_itemspace( ENUM_ITEMSPACE_INVENTORY ) return FAILED",
                              NumberToString(user->get_acc_id(), 0));
        return 0;
    }
    if (!user->send_itemspace(1))
    {
        LogManager::logFormat(1, "App.cpp",
                              __PRETTY_FUNCTION__, 0xf64,
                              "CGameManager::insert_game_world, pUser(%s)->send_itemspace( ENUM_ITEMSPACE_AVATAR ) return FAILED",
                              NumberToString(user->get_acc_id(), 0));
        return 0;
    }
    if (!user->send_itemspace(2))
    {
        LogManager::logFormat(1, "App.cpp",
                              __PRETTY_FUNCTION__, 0xf69,
                              "CGameManager::insert_game_world, pUser(%s)->send_itemspace( ENUM_ITEMSPACE_CARGO ) return FAILED",
                              NumberToString(user->get_acc_id(), 0));
        return 0;
    }
    if (!user->send_itemspace(7))
    {
        LogManager::logFormat(1, "App.cpp",
                              __PRETTY_FUNCTION__, 0xf7a,
                              "CGameManager::insert_game_world, pUser(%s)->send_itemspace( ENUM_ITEMSPACE_CREATURE ) return FAILED",
                              NumberToString(user->get_acc_id(), 0));
        return 0;
    }
    if (G_GameWorld()->GetChannelType() == 0xe)
    {
        user->set_charac_guildkey(0);
    }
    if (G_GameWorld()->reach_game_world(user) != 1)
    {
        // b28d-laneCGM（P227）：ORIG 两处 cMyTrace 均为全表达式临时链（实参在 ctor 前
        //   求值），具名 tr/tr2 形使 op() 实参装载落在 ctor 之后（fn_diff 归一化后
        //   仍差 6 条块序）。
        cMyTrace(__PRETTY_FUNCTION__, 0xfa2, 0)(
            "CGameManager::insert_game_world , !G_GameWorld()->reach_game_world( pUser ) , USER : %d(%s)",
            user->get_unique_id() & 0xffff, NumberToString(user->get_acc_id(), 0));
        user->SendCmdErrorPacket((ENUM_CMDPACKET)1, 4);
        cMyTrace(__PRETTY_FUNCTION__, 0xfab, 0)(
            "from (%d) m_id[%s] uid[%d]", 1,
            NumberToString(user->get_acc_id(), 0), user->get_unique_id() & 0xffff);
        return 0;
    }
    // P321-T1 直证：ORIG 0x08297371 `movsbl %al,%eax`（signed 扩展）；头声明为
    // P321-T1 直证：ORIG 0x08297371 `movsbl %al,%eax`（signed 扩展）；getter 已按
    // ORIG 改为返回 char（CUserCharacInfo.h 广播），此处直用即产 movsbl。
    int eventGrow = user->getCurCharacEventCharacterGrowtype();
    if (eventGrow > 0)
    {
        CEventCharacterHandlerInstance()->_makeEventCharacter(user, 0x28);
    }
    CExpandEquipslot* expandSlot = user->GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE_9);
    // P167 真语义修正：ORIG 0x082973b7 外层条件是 `G_GameWorld()->IsEquipSlotSwitchChannel()`
    // （旧形误用 channelSwitch==1/==0 双分支，且完全不调 IsEquipSlotSwitchChannel）；
    // then 块判 `== 0`（0x082973cd sete 形）、else 块判 `== 1`（0x08297425 cmp$1;sete）。
    if (G_GameWorld()->IsEquipSlotSwitchChannel())
    {
        if (user->getCurChannelEquipslotSwitch() == 0)
        {
            // b28d-laneCGM（P0）：ORIG 该段两次 `movsbl REG,REG` 无槽往返 ⇒ 原码把
            //   getter 直接内联进实参位（具名 char c/d 多 `mov %al,SLOT` 往返）。
            expandSlot->EquipslotSwitch(
                user, 0, user->getCurChannelEquipslotSwitch(),
                user->getCurExpandEquipslotSwitch());
        }
    }
    else
    {
        if (user->getCurChannelEquipslotSwitch() == 1)
        {
            expandSlot->EquipslotSwitch(
                user, 0, user->getCurChannelEquipslotSwitch(),
                user->getCurExpandEquipslotSwitch());
        }
    }
    // P167：ORIG 0x08297472/0x08297484 两处独立 GetChannelType 调用（无具名 channelType）。
    if (G_GameWorld()->GetChannelType() == 0xf ||
        G_GameWorld()->GetChannelType() == 0x10)
    {
        void* mission = user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
        ((CMissionList_Charac*)mission)->Update_EnterWorld_event(*user);
    }
    PacketGuard guard;
    guard.put_header(0, 2);
    guard.put_byte(0);
    guard.put_short(1);
    user->make_basic_info((char*)&guard, 0);
    guard.finalize(true);
    G_GameWorld()->send_all(guard, user);
    user->send_skill_info();
    guard.clear();
    guard.put_header(0, 2);
    guard.put_byte(1);
    guard.put_short(1);
    user->make_basic_info((char*)&guard, 1);
    guard.finalize(true);
    user->Send(guard);
    // DIAG restored 2026-09-21: Expand_Equip id=339 live again (empty CHARLIST unrelated to this skip).
    expandSlot->Send_Expand_Equip_Info(user);
    user->processReturnUserQuestAutoClear();
    // Restored 2026-09-21: ORIG sends CLEAR_QUEST + QUEST_INFO after expand equip.
    user->send_clear_quest_list();
    guard.clear();
    user->getCurCharacQuestR()->get_quest_info((char*)&guard);
    user->Send(guard);
#if 0 /* was DIAG skip post-339 town client extras; restored ORIG path */
#else
    ((CTitleBook*)user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)14))->sendList();
    ((CAchievement*)user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)15))->sendList();
    ((item_lock::CItemLock*)user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)2))->SendItemLockList(user);
    if (G_GameWorld()->GetChannelType() != 0xf)
    {
        SendPvpList(user);
    }
    SendWarRoomList(user);
    user->send_pvp_record();
    SendPartyList(user);
    // P167：ORIG 0x08297738/0x08297749/0x08297796 三处独立 getPunishTradeAlert
    // （条件两处 + put_int 实参一处），无具名 punish 局部。
    if (user->getPunishTradeAlert() == 0xe7 ||
        user->getPunishTradeAlert() == 0xe8)
    {
        guard.clear();
        guard.put_header(0, 0xff);
        guard.put_int((int)user->getPunishTradeAlert());
        guard.finalize(true);
        user->Send(guard);
    }
    guard.clear();
    guard.put_header(0, 0x7c);
    guard.finalize(true);
    user->Send(guard);
    // [K9GameRestA.CGameMgr12Run 2026-09-11 · P0/P183/P52] ORIG 0x08297836..0x0829783e 直证：
    //   `call GetPICSMap; mov %eax,(%esp); call map::clear` —— 返回直作 receiver，
    //   **无 pics 具名槽**（我方旧形多 `mov %eax,-0x20(%ebp)` + `mov -0x20(%ebp),%eax`）。
    ((std::map<std::pair<char, char>, int>*)user->GetPICSMap())->clear();
    user->ReCalcChattingEmoticon();
    user->SendChattingEmoticon();
    Packet_Monitor_Char_Info monitor;  // ORIG ctor 0x82a3ffa
    monitor.m_dbid = user->get_acc_id();
    monitor.m_channel = (unsigned char)((CEnvironment*)G_CEnvironment())->get_channel_no();
    monitor.m_channelType = (char)G_GameWorld()->GetChannelType();
    monitor.m_charNo = user->get_charac_no(-1);
    monitor.m_guildKey = user->get_charac_guildkey();
    monitor.m_job = user->get_charac_job();
    monitor.m_grade = user->getCurCharacGrowType();
    monitor.m_level = user->get_charac_level();
    monitor.m_memberKey = user->get_charac_memberkey();
    // P167：ORIG 0x08297913/0x08297928 两处独立 getCurCharacName（strlen 先求值入 %ebx，
    // 再第二次调用作 memcpy 第二实参），无具名 name/nameLen。
    memcpy(monitor.m_name, user->getCurCharacName(),
           (int)strlen(user->getCurCharacName()));
    monitor.m_returnUser = user->IsReturnUserFirstLogin();
    // [K9GameRestA.CGameMgr12Run 2026-09-11 · P183/P52/P329] ORIG 0x08297957..0x08297989
    // 直证：`lea -0xdf(%ebp),%ebx`（&monitor 先物化）→ `call GetServerGroup; mov %eax,0x4(%esp)`
    // → `mov &s_monitor_proxy_mgr,%edx; mov %edx,(%esp); call GetServerProxy` → 返回直作
    // SendTcpPacket 的 this —— **单条链式 receiver**，无 serverGroup/monitorProxy 具名槽。
    // 我方旧形多 4 条/处（`mov %eax,-0x2c; mov -0x2c,%edx` + `mov %eax,-0x28; mov -0x28,%eax`）。
    GlobalData::s_monitor_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)(size_t)user->GetServerGroup())
        ->SendTcpPacket((char*)&monitor, 0x3f);
    GlobalData::s_guild_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)(size_t)user->GetServerGroup())
        ->SendTcpPacket((char*)&monitor, 0x3f);
    g_schoolMgr.AddUser(user);
    if (user->get_charac_guildkey() != 0)
    {
        g_guildAgitMgr.AllocGuildAgitArea(user->get_charac_guildkey());
    }
    // P167 真参数序修正：ORIG 0x08297a0d/0x08297a2e 段实证实参 =
    //   (OBJ_TYPE_0, GetUID(), msg, sec, GetLoginTick(), getCurCharacNo())
    // （旧形第2参写 0、第5/6参颠倒 —— 定时器 idx/key 与 uid/characNo 错位）。
    // 实参右→左求值序：getCurCharacNo→%edi、GetLoginTick→%esi、GetUID→%ebx
    // （三者无栈槽往返 ⇒ 原码内联实参，P0/P24）；sec 为 0x3c/300/600 常量，
    // 第二段为 `get_rand_int(300) + 600`（ORIG 0x08297a85 add $0x258）。
    G_TimerQueue()->InsertTimer(
        TimerEntry::OBJ_TYPE_0, user->GetUID(), (TIMER_MESSAGE)2, 0x3c,
        user->GetLoginTick(), user->getCurCharacNo());
    G_TimerQueue()->InsertTimer(
        TimerEntry::OBJ_TYPE_0, user->GetUID(), (TIMER_MESSAGE)1,
        get_rand_int(300) + 600, user->GetLoginTick(), user->getCurCharacNo());
    G_TimerQueue()->InsertTimer(
        TimerEntry::OBJ_TYPE_0, user->GetUID(), (TIMER_MESSAGE)0x5f, 300,
        user->GetLoginTick(), user->getCurCharacNo());
    G_TimerQueue()->InsertTimer(
        TimerEntry::OBJ_TYPE_0, user->GetUID(), (TIMER_MESSAGE)0x60, 600,
        user->GetLoginTick(), user->getCurCharacNo());
#endif
        user->send_ontime_reward_start_notify();
    return 1;
}