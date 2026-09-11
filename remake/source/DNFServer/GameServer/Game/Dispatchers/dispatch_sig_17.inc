int Dispatcher_GM_Command::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0)
    {
        // ORIG 0x81fa1e4：LineFunc(0x88fd, pretty, 0, 0) 断言后返回其值。
        return LineFunc(0x88fd, __PRETTY_FUNCTION__, 0, 0);
    }
    if (user->getCurCharacR() == 0)
    {
        // ORIG 0x81fa224：LineFunc(0x8902, pretty, 0, 0) 断言后返回其值。
        return LineFunc(0x8902, __PRETTY_FUNCTION__, 0, 0);
    }

    {
        // ORIG 0x81fa235：NumberToString 先求值并缓存（mov %eax,%ebx），随后才发
        // cMyTrace(0x8908) ctor——内联回实参位会把 ctor 提前到两个 call 之前（实测
        // 头部 +6/-6 错位）。按 ORIG 求值序固定为具名局部。
        const char* accIdStr = NumberToString(user->get_acc_id(), 0);
        cMyTrace trace(__PRETTY_FUNCTION__, 0x8908, 0);
        trace("GM Command : %s", accIdStr);
    }

    // ORIG 0x81fa277：cmd 无初始化直发 get_byte（失败路径 LineFunc 返回，不经读
    // cmd；成功路径必写），删多余的 movb $0 零化。cmd/len/extra/name/text 全部
    // 走 GmCmdArgs 视图，取址形态 = ORIG 的 lea 基址+add 偏移。
    GmCmdArgs args;
    if (!packet.get_byte(args.cmd))
    {
        // ORIG 0x81fa2cb：LineFunc(0x890e, pretty, 0, 0) 断言后返回其值。
        return LineFunc(0x890e, __PRETTY_FUNCTION__, 0, 0);
    }

    PacketGuard reject;   // ORIG local_1c4：目标缺失时的固定应答
    reject.put_header(1, 0x6e);
    reject.put_byte(1);
    reject.put_byte(99);
    reject.put_byte(1);
    reject.finalize(true);

    PacketGuard resp;     // ORIG local_1d0
    resp.put_header(1, 0x6e);
    resp.put_byte(1);
    resp.put_byte(args.cmd);

    // ORIG 读包文本布局见 GmCmdArgs 注释；查表副本仍是独立栈缓冲。
    char targetName[0x1e];   // case9 ORIG local_248：调级目标的零填充副本
    char lookupName[0x1e];   // ORIG case4/5/6/0xb/0xc/0xd 各自的 30B 零填充查表副本
    //（P0 槽律：ORIG 每分支 memset(+strncpy) 槽往返实证，非内联右值）
    CUser* target;
    switch (args.cmd)
    {
    case 0:  // 切换 GM 模式（无参数）
        if (GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) <= GM_LV0)
            break;
        {
            // ORIG 0x81fa420：if/else 两次字面 SetGameMasterMode（mode==1→false，否则 true）
            if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) != 1)
                user->SetGameMasterMode(true);
            else
                user->SetGameMasterMode(false);
            GlobalData::s_GM_Manager->TurnGmMode(user->get_acc_id());
        }
        break;
    case 1:  // 全服公告 GMC <str>
        // ORIG 守卫链：GmMode==1 与 GetGmLevel>0 各自 test/jne；get_int/get_str 返回值
        // 不测试（0x81fa4eb 只读 len 槽判 0）。
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) != 1)
            break;
        if (GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) <= GM_LV0)
            break;
        {
            packet.get_int(args.len);
            packet.get_str(args.u.text, 0x100, args.len);
            if (args.len != 0)
            {
            resp.put_int(args.len);
            resp.put_str(args.u.text, args.len);
            resp.finalize(true);
            G_GameWorld()->send_all(resp);
            // ORIG 0x8939：全服广播后记日志
            cMyTrace trace(__PRETTY_FUNCTION__, 0x8939, 0);
            trace("GMC %s \n", args.u.text);
            }
        }
        break;
    case 2:  // 本区域公告 GMZ <str>
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) != 1)
            break;
        if (GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) <= GM_LV0)
            break;
        {
            packet.get_int(args.len);
            packet.get_str(args.u.text, 0x100, args.len);
            if (args.len != 0)
            {
            resp.put_int(args.len);
            resp.put_str(args.u.text, args.len);
            resp.finalize(true);
            G_GameWorld()->send_to_area(
                user->getCurCharacVill(), user->get_area(false), resp);
            // ORIG 0x894a：区域广播后记日志
            cMyTrace trace(__PRETTY_FUNCTION__, 0x894a, 0);
            trace("GMZ %s \n", args.u.text);
            }
        }
        break;
    case 3:  // 监控服公告 GMS <str>（resp 仅 finalize 不发送，ORIG 同）
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) != 1)
            break;
        if (GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) <= GM_LV0)
            break;
        {
            packet.get_int(args.len);
            packet.get_str(args.u.text, 0x100, args.len);
            if (args.len != 0)
            {
            resp.put_int(args.len);
            resp.put_str(args.u.text, args.len);
            resp.finalize(true);
            Packet_Monitor_Notice_Message noti;   // ORIG ctor 0x0822bbd6：PacketHeader(0x9dc,0x10b)+memset(m_text)
            noti.m_len = (unsigned char)args.len;
            strncpy(noti.m_text, args.u.text, args.len);   // ORIG strncpy 到 +0xb（m_text）
            CMonitorServerProxy* proxy = GlobalData::s_monitor_proxy_mgr->GetServerProxy(
                user->GetServerGroup());
            proxy->SendTcpPacket((char*)&noti, 0x10b);
            // ORIG 0x8966：监控公告发送后记日志
            cMyTrace trace(__PRETTY_FUNCTION__, 0x8966, 0);
            trace("GMS %s \n", args.u.text);
            }
        }
        break;
    case 4:  // 踢人 expel <name>
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) != 1)
            break;
        if (GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) <= GM_LV0)
            break;
        {
            packet.get_int(args.len);
            packet.get_str(args.u.kick.name, 0x1e, args.len);
            memset(lookupName, 0, sizeof(lookupName));
            strncpy(lookupName, args.u.kick.name, (size_t)args.len);
            // ORIG 0x81fa9c9：查找键是 get_str 目标本身（+0x12），lookupName 的
            // memset/strncpy 是原码死写入（P171 照抄）。
            target = G_GameWorld()->find_user_from_world_byaccid(
                G_GameWorld()->GetIDUserInIDMap(args.u.kick.name));
            if (target == 0)
            {
                user->Send(reject);
            }
            else
            {
                resp.put_int(args.len);
                resp.put_str(args.u.kick.name, args.len);
                resp.finalize(true);
                user->Send(resp);
                target->Send(resp);
                target->DisConnSig((DISCONN_SIG)0x14, true, 0);
            }
            // ORIG 0x8993：踢人日志（目标在/不在都记）
            cMyTrace trace(__PRETTY_FUNCTION__, 0x8993, 0);
            trace("GM(%s) expel %s \n", user->getCurCharacName(), args.u.kick.name);
        }
        break;
    case 5:  // 禁言 forbid chat <mins> <nameLen> <name>
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) != 1)
            break;
        if (GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) <= GM_LV0)
            break;
        {
            packet.get_int(args.len);
            packet.get_int(args.u.chat.extra);
            packet.get_str(args.u.chat.name, 0x1e, args.u.chat.extra);
            // ORIG local_7c：分钟→秒钳制（<1→60s；>=0x5a0→0x15180=1 天）
            int sec = (args.len < 0x5a0) ? ((args.len < 1) ? 0x3c : args.len * 0x3c) : 0x15180;
            memset(lookupName, 0, sizeof(lookupName));
            strncpy(lookupName, args.u.chat.name, (size_t)args.u.chat.extra);
            target = G_GameWorld()->find_user_from_world_byaccid(
                G_GameWorld()->GetIDUserInIDMap(lookupName));
            if (target == 0)
            {
                user->Send(reject);
            }
            else
            {
                G_GameWorld()->DisableChat(args.u.chat.name, sec);
                // ORIG 0x89b8：定时器入队日志（%d=目标 characNo）
                {
                    cMyTrace trace(__PRETTY_FUNCTION__, 0x89b8, 0);
                    trace("Insert timer queue @ forbidchat: %s %d\n",
                          args.u.chat.name, target->getCurCharacNo());
                }
                G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0,
                                            (int)target->GetUID(),
                                            (TIMER_MESSAGE)0x5d, sec,
                                            (int)target->GetLoginTick(),
                                            (unsigned int)target->getCurCharacNo());
                resp.put_int(args.len);
                resp.put_int(args.u.chat.extra);
                resp.put_str(args.u.chat.name, args.u.chat.extra);
                resp.finalize(true);
                user->Send(resp);
                target->Send(resp);
                // ORIG Packet_Forbid_Chat_By_Monitor：ctor 0x0822bc70（PacketHeader(0x9dd,0x30)
                // + memset(m_name)），m_secs=秒、m_name=名（m_padE 无写入，头注释同证）
                Packet_Forbid_Chat_By_Monitor moni;
                moni.m_secs = sec;
                strncpy(moni.m_name, args.u.chat.name, (size_t)args.u.chat.extra);
                CMonitorServerProxy* proxy =
                    GlobalData::s_monitor_proxy_mgr->GetServerProxy(
                        target->GetServerGroup());
                proxy->SendTcpPacket((char*)&moni, 0x30);
                // ORIG 0x89ce：操作日志（%d=分钟数）
                cMyTrace trace(__PRETTY_FUNCTION__, 0x89ce, 0);
                trace("GM(%s) forbid chat %s for %d mins \n",
                      user->getCurCharacName(), args.u.chat.name, args.len);
            }
        }
        break;
    case 6:  // 禁移动 forbid move <mins> <nameLen> <name>（仅 state==3 生效）
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) != 1)
            break;
        if (GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) <= GM_LV0)
            break;
        {
            packet.get_int(args.len);
            packet.get_int(args.u.chat.extra);
            packet.get_str(args.u.chat.name, 0x1e, args.u.chat.extra);
            int sec = (args.len < 0x5a0) ? ((args.len < 1) ? 0x3c : args.len * 0x3c) : 0x15180;
            memset(lookupName, 0, sizeof(lookupName));
            strncpy(lookupName, args.u.chat.name, (size_t)args.u.chat.extra);
            target = G_GameWorld()->find_user_from_world_byaccid(
                G_GameWorld()->GetIDUserInIDMap(lookupName));
            if (target == 0)
            {
                user->Send(reject);
            }
            else if (target->get_state() == 3)
            {
                G_GameWorld()->DisableMove(args.u.chat.name, sec);
                // ORIG 0x89fe：定时器入队日志
                {
                    cMyTrace trace(__PRETTY_FUNCTION__, 0x89fe, 0);
                    trace("Insert timer queue @ forbidmove: %s %d\n",
                          args.u.chat.name, target->getCurCharacNo());
                }
                G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0,
                                            (int)target->GetUID(),
                                            (TIMER_MESSAGE)0x5e, sec,
                                            (int)target->GetLoginTick(),
                                            (unsigned int)target->getCurCharacNo());
                resp.put_int(args.len);
                resp.put_int(args.u.chat.extra);
                resp.put_str(args.u.chat.name, args.u.chat.extra);
                resp.finalize(true);
                user->Send(resp);
                target->Send(resp);
                // ORIG 0x8a09：操作日志
                cMyTrace trace(__PRETTY_FUNCTION__, 0x8a09, 0);
                trace("GM(%s) forbid move %s for %d mins \n",
                      user->getCurCharacName(), args.u.chat.name, args.len);
            }
            // state!=3：ORIG 无任何回包/动作
        }
        break;
    case 7:  // 隐身
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) != 1)
            break;
        if (GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) <= GM_LV0)
            break;
        {
            user->SetInvisible();
            if (user->get_state() == 3)
            {
                user->SendNotiPacket((CUser::eSendTarget)0, (ENUM_NOTIPACKET)2, 0);
            }
            // ORIG 0x8a2b
            cMyTrace trace(__PRETTY_FUNCTION__, 0x8a2b, 0);
            trace("GM(%s) is invisible \n", user->getCurCharacName());
        }
        break;
    case 8:  // 现身
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) != 1)
            break;
        if (GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) <= GM_LV0)
            break;
        {
            user->SetVisible();
            if (user->get_state() == 3)
            {
                user->SendNotiPacket((CUser::eSendTarget)0, (ENUM_NOTIPACKET)2, 0);
            }
            // ORIG 0x8a43
            cMyTrace trace(__PRETTY_FUNCTION__, 0x8a43, 0);
            trace("GM(%s) is visible \n", user->getCurCharacName());
        }
        break;
    case 9:  // 调级 setlevel <level> <nameLen> <name>
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) != 1)
            break;
        if (GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) <= GM_LV0)
            break;
        {
            packet.get_int(args.len);
            packet.get_int(args.u.chat.extra);
            packet.get_str(args.u.chat.name, 0x1e, args.u.chat.extra);
            // ORIG local_248：调级目标用零填充副本查表；回包/日志仍用读入文本 name。
            // （原实现误 memset(name) 致目标永远查不到。）
            memset(targetName, 0, sizeof(targetName));
            strncpy(targetName, args.u.chat.name, (size_t)args.u.chat.extra);
            target = G_GameWorld()->find_user_from_world_byaccid(
                G_GameWorld()->GetIDUserInIDMap(targetName));
            if (target == 0)
            {
                user->Send(reject);
            }
            else
            {
                int diff = args.len - target->get_charac_level();
                // ORIG -0x4d 为 **unsigned char** 槽（mov %al,-0x4d 低 8 位截断 +
                // cmpb $0x0 判 0）：最后一次 SetCharacLevel 的返回（!=0 才回包；
                // diff==0 静默）。叶块序 = ORIG 物理序：diff==0 → 升（>0）→ 降（<0）。
                unsigned char result = 0;
                if (diff == 0)
                {
                    result = 0;
                }
                else if (diff > 0)
                {
                    for (int i = 0; i < diff; ++i)
                    {
                        int nextLv = target->get_charac_level() + 1;
                        int lvCap = 200;
                        result = target->SetCharacLevel(
                            std::min(lvCap, nextLv));
                    }
                }
                else
                {
                    for (int i = 0; i > diff; --i)
                    {
                        int nextLv = target->get_charac_level() - 1;
                    int lvCap = 200;
                    result = target->SetCharacLevel(
                        std::min(lvCap, nextLv));
                    }
                }
                if (result != 0)
                {
                    int st = target->get_state();
                    if (st == 3)
                    {
                        target->SendNotiPacket((CUser::eSendTarget)1, (ENUM_NOTIPACKET)0x25, 0);
                        target->SendNotiPacket((CUser::eSendTarget)0, (ENUM_NOTIPACKET)2, 0);
                        target->SendNotiPacket((CUser::eSendTarget)1, (ENUM_NOTIPACKET)2, 1);
                    }
                    else if (st == 5)
                    {
                        target->SendNotiPacket((CUser::eSendTarget)1, (ENUM_NOTIPACKET)0x25, 0);
                        target->SendNotiPacket((CUser::eSendTarget)2, (ENUM_NOTIPACKET)2, 1);
                    }
                    resp.put_int(args.len);
                    resp.put_int(args.u.chat.extra);
                    resp.put_str(args.u.chat.name, args.u.chat.extra);
                    resp.finalize(true);
                    user->Send(resp);
                    target->Send(resp);
                    // ORIG 0x8a89
                    cMyTrace trace(__PRETTY_FUNCTION__, 0x8a89, 0);
                    trace("GM(%s) modify %s level to %d \n",
                          user->getCurCharacName(), args.u.chat.name, args.len);
                }
            }
        }
        break;
    case 10:  // 发物品 grant item <itemIdx> <count> <nameLen> <name>
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) != 1)
            break;
        if (GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) <= GM_LV0)
            break;
        {
            packet.get_int(args.len);
            packet.get_short(args.u.item.wLen);
            packet.get_int(args.u.item.extra);
            packet.get_str(args.u.item.name, 0x1e, args.u.item.extra);
            memset(lookupName, 0, sizeof(lookupName));
            strncpy(lookupName, args.u.item.name, (size_t)args.u.item.extra);
            target = G_GameWorld()->find_user_from_world_byaccid(
                G_GameWorld()->GetIDUserInIDMap(lookupName));
            if (target == 0)
            {
                user->Send(reject);
            }
            else
            {
                ENUM_ITEMSPACE space = (ENUM_ITEMSPACE)0;
                int slot = target->AddItem(args.len, (int)(unsigned short)args.u.item.wLen,
                                           (eItemAddReason)6, space, 0);
                if (slot >= 0)
                {
                    // ORIG 0x8ab0：成功发物品日志（先于 SendUpdateItemList）
                    {
                        cMyTrace trace(__PRETTY_FUNCTION__, 0x8ab0, 0);
                        trace("GM(%s) granted Item(%d) to %s.",
                              user->getCurCharacName(), slot,
                              target->getCurCharacName());
                    }
                    target->SendUpdateItemList((CUser::eSendTarget)1, space, slot);
                    resp.put_int(args.len);
                    resp.put_short(args.u.item.wLen);
                    resp.put_int(args.u.item.extra);
                    resp.put_str(args.u.item.name, args.u.item.extra);
                    resp.finalize(true);
                    user->Send(resp);
                    // ORIG 0x8aba
                    cMyTrace trace(__PRETTY_FUNCTION__, 0x8aba, 0);
                    trace("GM(%s) grant item(%d) to %s \n",
                          user->getCurCharacName(), args.len, args.u.item.name);
                }
            }
        }
        break;
    case 0xb:  // 复活 revive <name>
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) != 1)
            break;
        if (GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) <= GM_LV0)
            break;
        {
            packet.get_int(args.len);
            packet.get_str(args.u.kick.name, 0x1e, args.len);
            memset(lookupName, 0, sizeof(lookupName));
            strncpy(lookupName, args.u.kick.name, (size_t)args.len);
            target = G_GameWorld()->find_user_from_world_byaccid(
                G_GameWorld()->GetIDUserInIDMap(lookupName));
            if (target == 0)
            {
                user->Send(reject);
            }
            else
            {
                // ORIG 0x2680：target 有 party 才干活；无 party 独立再发 reject
                CParty* party = (CParty*)target->GetParty();
                if (party == 0)
                {
                    user->Send(reject);
                }
                else
                {
                    party->set_charac_live(target, (ENUM_USER_DUNGEON_STATE)1);
                    resp.put_int(args.len);
                    resp.put_str(args.u.kick.name, args.len);
                    resp.finalize(true);
                    user->Send(resp);
                    target->Send(resp);
                    // ORIG 0x8ae2
                    cMyTrace trace(__PRETTY_FUNCTION__, 0x8ae2, 0);
                    trace("GM(%s) revive %s \n", user->getCurCharacName(), args.u.kick.name);
                }
            }
        }
        break;
    case 0xc:  // 重置疲劳 reset fatigue <name>
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) != 1)
            break;
        if (GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) <= GM_LV0)
            break;
        {
            packet.get_int(args.len);
            packet.get_str(args.u.kick.name, 0x1e, args.len);
            memset(lookupName, 0, sizeof(lookupName));
            strncpy(lookupName, args.u.kick.name, (size_t)args.len);
            target = G_GameWorld()->find_user_from_world_byaccid(
                G_GameWorld()->GetIDUserInIDMap(lookupName));
            if (target == 0)
            {
                user->Send(reject);
            }
            else
            {
                target->RecoverFatigue(0);
                target->SendFatigue();
                resp.put_int(args.len);
                resp.put_str(args.u.kick.name, args.len);
                resp.finalize(true);
                user->Send(resp);
                target->Send(resp);
                // ORIG 0x8b09
                cMyTrace trace(__PRETTY_FUNCTION__, 0x8b09, 0);
                trace("GM(%s) reset fatigue %s \n", user->getCurCharacName(), args.u.kick.name);
            }
        }
        break;
    case 0xd:  // 封号 forbid play <days> <nameLen> <name>（发 SIG_FORBID_USER_TO_PLAY 到 DB 队列）
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) != 1)
            break;
        if (GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) <= GM_LV0)
            break;
        {
            packet.get_int(args.len);
            packet.get_int(args.u.chat.extra);
            packet.get_str(args.u.chat.name, 0x1e, args.u.chat.extra);
            memset(lookupName, 0, sizeof(lookupName));
            strncpy(lookupName, args.u.chat.name, (size_t)args.u.chat.extra);
            target = G_GameWorld()->find_user_from_world_byaccid(
                G_GameWorld()->GetIDUserInIDMap(lookupName));
            if (target == 0)
            {
                user->Send(reject);
            }
            else
            {
                Stream* stream = GlobalData::s_stream_pool->Acquire(
                    "PacketDispatcher_Impl_1.cpp", 0x8b28);
                CStreamGuard guard(stream, true);
                // ORIG：*guard → operator*（0x80c8c46），GetInBuffer 走 operator->（0x80c8c4e）。
                **guard << 0x93;
                **guard << (int)user->GetUID();
                SIG_FORBID_USER_TO_PLAY* sig =
                    guard->GetInBuffer<SIG_FORBID_USER_TO_PLAY>();
                memset(sig, 0, 9);
                gm_pkt::SigForbidPlayBody* body =
                    (gm_pkt::SigForbidPlayBody*)sig;
                body->m_characNo = target->getCurCharacNo();
                body->m_flag = 1;
                // ORIG 0x81fc20d：三分支落码 test/jg → cmp $0x63,jle（[1,100] 钳制），
                // 叶块序 = (<=0)→1 → (>99)→100 → 原值。
                if (args.len <= 0)
                    body->m_days = 1;
                else if (args.len > 99)
                    body->m_days = 100;
                else
                    body->m_days = args.len;
                GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
                resp.put_int(args.len);
                resp.put_int(args.u.chat.extra);
                resp.put_str(args.u.chat.name, args.u.chat.extra);
                resp.finalize(true);
                user->Send(resp);
                target->Send(resp);
                // ORIG 0x8b44
                cMyTrace trace(__PRETTY_FUNCTION__, 0x8b44, 0);
                trace("GM(%s) forbid play %s for %d days \n",
                      user->getCurCharacName(), args.u.chat.name, args.len);
            }
        }
        break;
    default:
        // ORIG 0x8b52：未知 GM 命令日志（%d=cmd，%s=accId）
        cMyTrace trace(__PRETTY_FUNCTION__, 0x8b52, 0);
        trace("Undifined GM Command : %d by %s", (int)args.cmd,
              NumberToString(user->get_acc_id(), 0));
        break;
    }
    return 0;
}