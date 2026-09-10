int DisPatcher_GetItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stGetItemParam& out = static_cast<stGetItemParam&>(param);
    MSG_GET_ITEM& req = *reinterpret_cast<MSG_GET_ITEM*>(&msg);
    out.m_errNo = check_error(user, msg);
    if (out.m_errNo >= 1)
    {
        return 0;
    }
    if (out.m_errNo < 0)
    {
        return LineFunc(0xe5c,
                        __PRETTY_FUNCTION__,
                        out.m_errNo, user->get_acc_id());
    }
    if (req.m_17 != 1)
    {
        user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x3bb, 1, 0, 0);
    }
    if (req.m_15 % 7 != 0)
    {
        user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x12f, 1, req.m_11, req.m_13);
    }
    // [BLOCKED 依赖缺失] cUserHistoryLog::SetItemXYPos（ORIG W 0x0822f084
    // _ZN15cUserHistoryLog12SetItemXYPosEtt）无头文件声明且全仓无定义：
    // 待补齐后于 addServerHackCnt(0x12f) 之后调用
    // user->m_historyLog.SetItemXYPos(req.m_1a, req.m_1c)。
    user->m_historyLog.SetItemXYPos(req.m_1a, req.m_1c);   // ORIG 0x81c3731
    if (user->get_state() == 5)
    {
        CParty* party = (CParty*)user->GetParty();
        if (party != 0)
        {
            // ORIG 0x81c3782..0x81c3804：GetItemTimeChecker(now, m_1a, m_1c, 0x5dc)
            // 非 0 且双通道 diff（+0x8e444/+0x8e446）任一 >500 → 计 0x130
            const unsigned int nowTick =
                GlobalData::s_systemTime_.getCurTickCount();
            if (user->GetItemTimeChecker(nowTick, req.m_1a, req.m_1c, 0x5dc) != 0 &&
                // CUser+0x8e444/0x8e446 = 内嵌 m_mcap(+0x8e3f0) 的 +0x54/+0x56
                // （CMCAPManager.h m_timeChkDiffA/m_timeChkDiffB）；ORIG 0x081c37b2
                // `movzwl 0x8e444(%eax); cmp $0x1f4,%ax; jg`（0x081c37cd `jle`）=
                // 2B 比较 + **有符号序**，与头内 short 声明一致 ⇒ 取 short 视图
                // （轴 B 正向修复：旧 unsigned short 视图产 ja/jbe）。
                (*reinterpret_cast<const short*>(
                     &user->m_mcap.m_timeChkDiffA) > 0x1f4 ||
                 *reinterpret_cast<const short*>(
                     &user->m_mcap.m_timeChkDiffB) > 0x1f4))
            {
                user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x130, 1, 0, 0);
            }
            // ORIG 0x81c3809..0x81c387d：GetItemSerialChecker(m_1a, m_1c, m_0d)==2 → 计 0x131
            if (user->GetItemSerialChecker(req.m_1a, req.m_1c, req.m_0d) == 2)
            {
                user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x131, 1, 0, 0);
            }
            // ORIG 0x81c3882..0x81c38a0：IsRoutingState → m_errNo=0xd8
            if (party->IsRoutingState())
            {
                out.m_errNo = 0xd8;
                return 0;
            }
            int outIdx = 0;
            unsigned char outH = 0;
            bool outB = false;
            if (!party->get_item(user, req.m_0d, req.m_0f == 1, (char)req.m_10,
                                 outIdx, outH, outB))
            {
                out.m_errNo = 0x7fffffff;   // ORIG 0x81c3cd2
                return 0;
            }
            // ORIG 0x81c3912..0x81c39ac：IsAutoCreated || member_count==1 →
            // checkGetItemPacket(seed, m_11, m_13)==0 → 计 0x12e
            if (party->IsAutoCreated() || party->get_member_count() == 1)
            {
                if (checkGetItemPacket(user->getBackupSeed(),
                                       req.m_11, req.m_13) == 0)
                {
                    user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x12e, 1, 0, 0);
                }
            }
            // ORIG 0x81c39b1..0x81c3b2b：CBattle_Field(party+0xb24).get_dungeon_index()
            // → find_dungeon；null 或 outIdx==-1 → logFormat 0xf14；否则
            // m_entryRestrictFlag && m_10∈{2,3,4} && !outB → 0x56 包 send_all 广播
            CBattle_Field* bf =
                &party->m_battleField;
            const int dungeonIdx = bf->get_dungeon_index();
            CDungeon* dungeon = G_CDataManager()->find_dungeon(dungeonIdx);
            if (dungeon == 0 || outIdx == -1)
            {
                LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp",
                                      __PRETTY_FUNCTION__, 0xf14,
                                      "find_dungeon(%d) fail", dungeonIdx);
            }
            else if (dungeon->m_entryRestrictFlag != 0 &&
                     (req.m_10 == 2 || req.m_10 == 3 || req.m_10 == 4) &&
                     !outB)
            {
                PacketGuard packet;
                packet.put_header(0, 0x56);
                packet.put_byte(5);
                packet.put_short((int)(user->get_unique_id() & 0xffff));
                packet.put_int(outIdx);
                packet.finalize(true);
                G_GameWorld()->send_all(packet);
            }
            out.m_errNo = 0;
            return 0;
        }
        out.m_errNo = 0x7fffffff;
        return 0;
    }
    if (user->get_state() == 10)
    {
        WongWork::CDeathTower* tower = user->getDeathTower();  // thunk 直调 0x86552a4
        if (tower != 0)
        {
            if (!tower->handlePickupItem(user, req.m_0d,
                                         req.m_0f == 1, (char)req.m_10))
            {
                out.m_errNo = 0x7fffffff;
                return 0;
            }
            out.m_errNo = 0;
            return 0;
        }
        out.m_errNo = 0x7fffffff;
        return 0;
    }
    if (user->get_state() == 0xc)
    {
        // ORIG 0x81c3bdf..0x81c3c19：CBossTowerFunc::GetItem(user, m_0d, m_0f==1,
        // (char)m_10)，随后无条件置 m_errNo=0x7fffffff（asm +0x659）
        Global::CBossTowerFunc::GetItem(user, req.m_0d,
                                        req.m_0f == 1, (char)req.m_10);
        out.m_errNo = 0x7fffffff;
        return 0;
    }
    if (user->get_state() == 8)
    {
        WarRoom* room = G_CGameManager()->GetWarRoom(
            user->GetWarRoomIndex());  // thunk 0x822fe84/0x80cc18e/0x82a25d0
        if (room == 0)
        {
            return LineFunc(0xf32,
                            __PRETTY_FUNCTION__,
                            0, user->get_acc_id());
        }
        if (!room->HandleGetItem(user, req.m_0d))
        {
            out.m_errNo = 0x7fffffff;
            return 0;
        }
        out.m_errNo = 0;
        return 0;
    }
    out.m_errNo = 0x13;
    return 0;
