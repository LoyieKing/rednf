void CGameManager::Send_userinfos_to_upper_server(unsigned char channelType)
{
    // b28d-lane12 旧 N82 判词（gap 32）已被本轮布局修正推翻。
    // b28d-solo-CGameManager 判词：DIFF·N115/N82（ours 302 / orig 303，gap 1）。
    //   本轮真布局修复：Packet_Monitor_UDP_Reply_UserInfo.h 的
    //   ST_MonitorReplyUserInfo 条目步长 0x52→0x4e（m_job 4B→1B；ORIG
    //   0x08296369..0x082964ca 逐字段 `imul $0x4e,%ebx,%edx` + 1B/2B/4B 写偏移
    //   实证）+ 去 item 引用缓存改逐字段 pkt.m_items[userCount]（P157/P183：
    //   ORIG 每字段重发 imul/lea/sub 全链地址重算）⇒ gap 37→1。
    //   残余 = 4.4 缺 push %esi（ORIG 用 %esi 跨循环缓存 group）+ 帧 0x1764 vs
    //   0x1750（N115：4.4 恒预留 16B 对齐 outgoing）+ 槽号整体平移 0x10（N82）；
    //   P167 CLEAR 43/43。
    // [K9GameRestA.CGameMgr12Run 2026-09-11 复核] DIFF 304/302：结构差唯一 = ORIG
    //   0x08296528..0x08296531 `cmpl $0x4a,-0x18(%ebp); setg %al; test %al,%al; je`
    //   （4.1 对 `++idx > 0x4a` 的 bool 物化）vs 4.4 直产 `jle`（-2 条，P168a/N122）
    //   ⇒ 源码不可择，封顶；串/实参序已核（§6.5）。
    int userCount = 0;
    Packet_Monitor_UDP_Reply_UserInfo pkt;
    pkt.m_channel = (unsigned char)((CEnvironment*)G_CEnvironment())->get_channel_no();  // ORIG p[11]
    // b28d-solo-CGameManager: ORIG 0x082961be 两独立守卫（`test %al;je` +
    // `cmpb $0xc9;jne` 同落点）⇒ 拆嵌套 if（P225 推广）；合并 && 会多
    // mov$1/mov$0+test 物化段（实测）。
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        if (channelType == 0xc9)
        {
            int idx = 0;
            int group = GlobalData::s_monitor_proxy_mgr->GetStartIndex();
            while (idx < GlobalData::s_monitor_proxy_mgr->GetEndIndex())
            {
                Packet_ChannelType ctype;
                // ORIG 写 pkt+0xa/+0xe（12 字节头旧读法 +0xc/+0x10 为误对齐）。
                ctype.m_channel = (unsigned int)((CEnvironment*)G_CEnvironment())->get_channel_no();
                ctype.m_type = (int)G_GameWorld()->GetChannelType();
                // ORIG 0x08296260..0x08296289：packetSize→%esi、&ctype→%ebx 先物化并跨
                // GetServerProxy 调用保活（push %esi），随后才压 SendTcpPacket 实参
                // ⇒ 必须单语句链式 receiver（P52/P23），具名 proxy 会把实参装载推到
                // receiver 之后并丢掉 %esi 独占。
                GlobalData::s_monitor_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)group)
                    ->SendTcpPacket((char*)&ctype, ctype.packetSize);
                group = GlobalData::s_monitor_proxy_mgr->GetNextIndex(idx);
            }
        }
    }
    // b28d-laneCGM: P223 空 then 占位岛复现——ORIG 0x082962c3 `call empty; test %al;
    //   jne +0x478(nop 岛)` + 循环退出段 `jmp +0x479`（跨过 nop 落 epilogue）。
    //   写 `if (!empty) { body }` 时 4.4 jump-thread 掉 jmp+nop（15 分支 vs ORIG 16）。
    if (m_userByAccId.empty())
        goto L_user_by_acc_id_done;
    {
        int groupIdx = 0;
        int group = GlobalData::s_monitor_proxy_mgr->GetStartIndex();
        while (groupIdx < GlobalData::s_monitor_proxy_mgr->GetEndIndex())
        {
            // b28d-laneCGM 真语义修正（ORIG 0x08296324 `mov %eax,-0x14(%ebp)` 在循环头
            //   **无条件**执行 ⇒ 原码是该轮 `it->second` 的具名 user，不是"最后一个写入
            //   成功者"）：我方旧形 `CUser* lastUser = 0;`（仅 if 体内更新）在末轮
            //   user 不满足条件时把**上一轮**的 user 传给 SendUserInfoToUpperServer，
            //   与 ORIG 传"末轮 user"不同；且 `= 0` 多一条 `movl $0x0,SLOT`（ORIG 裸声明）。
            CUser* user;
            std::map<unsigned int, CUser*>::iterator it = m_userByAccId.begin();
            for (; it != m_userByAccId.end(); ++it)
            {
                user = it->second;
                if (user->get_state() >= 2 &&
                    (int)(size_t)user->GetServerGroup() == group)
                {
                    // b28d-solo-CGameManager 修法（P157/P183）：ORIG 每字段重发
                    // `mov -0x18(%ebp),%ebx; imul $0x4e,%ebx,%edx; lea; sub $K;
                    // mov disp(%edx)` 全链地址重算（0x08296369..0x082964ca 逐字段
                    // 实证），无 item 引用缓存 ⇒ 逐字段写 pkt.m_items[userCount]。
                    pkt.m_items[userCount].m_dbid = user->get_acc_id();
                    pkt.m_items[userCount].m_charNo = user->get_charac_no(-1);
                    pkt.m_items[userCount].m_channel = user->GetUID();  // 游戏端写 UID（见包头注释）
                    pkt.m_items[userCount].m_guildKey = user->get_charac_guildkey();
                    pkt.m_items[userCount].m_job = user->get_charac_job();
                    pkt.m_items[userCount].m_grade = user->getCurCharacGrowType();
                    pkt.m_items[userCount].m_level = user->get_charac_level();
                    pkt.m_items[userCount].m_memberKey = user->get_charac_memberkey();
                    strcpy(pkt.m_items[userCount].m_name, user->getCurCharacName());
                    // b28d-laneCGM 字段写序照 ORIG 0x08296497..0x0829651f：getSex →
                    //   GetSsnString/memcpy(6) → GetPeerIP2/strcpy(ip)（原形把 ip 提前，
                    //   fn_diff 归一化后仍差 20 条块序）。
                    pkt.m_items[userCount].m_sex = user->getSex();
                    memcpy(pkt.m_items[userCount].m_ssn, user->GetSsnString(), 6);
                    char ipBuf[16];
                    user->m_network.GetPeerIP2(ipBuf, 0x10);  // ORIG (CNetwork*)(user+0xe0)
                    strcpy(pkt.m_items[userCount].m_ip, ipBuf);
                    userCount = userCount + 1;
                    // [g12 P-G7a 试形] ORIG 0x08296528 物化 `cmpl $0x4a,-0x18(%ebp); setg %al;
                    //   test %al,%al; je`（4.1 条件位 bool 物化）；4.4 对 `if` 恒直折 `jle`。
                    //   改循环条件位（while + goto）以制造同形物化。
                    while (userCount > 0x4a)
                    {
                        pkt.packetSize = 0x16e6;
                        pkt.m_count = (unsigned char)userCount;  // ORIG p[10]
                        SendUserInfoToUpperServer(pkt, channelType, user, (ENUM_SERVER_GROUP)group);
                        userCount = 0;
                        goto L_send_userinfos_upper_done;
                    }
                L_send_userinfos_upper_done:;
                }
            }
            if (userCount != 0)
            {
                pkt.packetSize = (unsigned short)(userCount * 0x4e + 0xc);
                pkt.m_count = (unsigned char)userCount;
                SendUserInfoToUpperServer(pkt, channelType, user, (ENUM_SERVER_GROUP)group);
                userCount = 0;
            }
            group = GlobalData::s_monitor_proxy_mgr->GetNextIndex(groupIdx);
        }
    }
L_user_by_acc_id_done:;
}