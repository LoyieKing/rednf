int DisPatcher_GetItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)

{
    stGetItemParam& out = static_cast<stGetItemParam&>(param);
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
    // [G5-ProcB] ORIG 0x081c3642 `mov 0x10(%ebp),%eax; mov %eax,-0x20(%ebp)`：
    // msg 视图具名局部在两级 errNo 检查之后初始化（ORIG 参数槽 -0x24 先、msg 槽
    // -0x20 后）。函数头声明多 1 条 store 且槽基下移（我方帧 0x60 vs ORIG 0x50）。
    MSG_GET_ITEM& req = *reinterpret_cast<MSG_GET_ITEM*>(&msg);
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
        // [K6-DpcProcess] ORIG 0x081c375d `cmpl $0x0,-0x1c(%ebp)` + 0x081c3761
        // `je 0x081c3cd2`：party 为空与其余全部失败点共用**函数尾唯一**的
        // errNo=0x7fffffff 岛（ORIG 各失败点形均为 `je/jmp 0x81c3cd2` 直跳）；
        // 我方旧形把该三指令块在每处内联 ⇒ 6 处共多 12 条。改后统一 goto。
        if (party == 0)
        {
            goto fail_7fffffff;
        }
        {
            // ORIG 0x81c3782..0x81c3804：GetItemTimeChecker(now, m_1a, m_1c, 0x5dc)
            // 非 0 且双通道 diff（+0x8e444/+0x8e446）任一 >500 → 计 0x130
            // [DPC_Process] ORIG 0x081c3790 `movl $SYM,(%esp); call`（getCurTickCount）
            // 紧接 0x081c3795 `mov %eax,0x4(%esp)` —— 返回值**直传第 1 实参，无具名槽**；
            // 两字段先入 esi/ebx 跨 call 保活（0x081c3762/0x081c3772）。旧文 nowTick 具名
            // 局部多 store/load 两条并使字段读落到 call 之后（edx/eax 而非 callee-saved）。
            // [K6-DpcProcess 2026-09-11] P225 推广：ORIG 0x081c37a6 `test %eax,%eax` /
            // 0x081c37a8 `setne %al` / 0x081c37ab `test %al,%al` / 0x081c37ad `je 0x249`
            // 与 0x081c37b2..0x081c37cd 的 `jg 0x20f` / `jle 0x249` 是**逐条件直跳**；
            // 旧的 `A != 0 && (B || C)` 单条件式使 4.4 把 `B || C` 物化成
            // `mov $1/%eax; jmp; mov $0/%eax; test %al,%al; je`（+5 条）。拆嵌套 if 复现直跳。
            if (user->GetItemTimeChecker(
                    GlobalData::s_systemTime_.getCurTickCount(),
                    req.m_1a, req.m_1c, 0x5dc) != 0)
            {
                // CUser+0x8e444/0x8e446 = 内嵌 m_mcap(+0x8e3f0) 的 +0x54/+0x56
                // （CMCAPManager.h m_timeChkDiffA/m_timeChkDiffB，均 short）；ORIG
                // 0x081c37b2 `movzwl 0x8e444(%eax); cmp $0x1f4,%ax; jg`（0x081c37cd
                // `jle`）= 2B 比较 + **有符号序**，与头内 short 声明一致。
                // [J5-ProcD 2026-09-11] J1Dispatchers 实证修法：裸偏移/reinterpret
                // 视图产 `add $0x8e444,%eax; movzwl (%eax),%eax` 两条；**具名成员直接
                // 访问**才产 ORIG 的单条 `movzwl 0x8e444(%eax),%eax`（disp 折叠）。
                if (user->m_mcap.m_timeChkDiffA > 0x1f4 ||
                    user->m_mcap.m_timeChkDiffB > 0x1f4)
                {
                    user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x130, 1, 0, 0);
                }
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
            // [K6-DpcProcess 2026-09-11 第5轮负结论] 本处及 state==10/state==8 两处同族：
            // ORIG 0x081c390c / 0x081c3b93 / 0x081c3cae 均为直分支形
            // `call; test %al,%al; je 0x081c3cd2`（2 条）；`!f()` 与 `f() == 0` 两种写法
            // 在 4.4 -O0 下**产出相同**（`call; xor $0x1,%eax; test %al,%al; jne`，3 条）
            // —— 父泳道复编读数 490 无变化（P168a② 的 `==0` 旋钮对本族无效）。
            // 故这是 4.4 对 bool 返回值"失败判定"恒插 xor 归一的跨版本差异（4.1 直出 je）
            // ⇒ 记 N，勿再试（保留 `== 0` 直白语义；`!` 形与其机器码等价）。
            if (party->get_item(user, req.m_0d, req.m_0f == 1, (char)req.m_10,
                                outIdx, outH, outB) == 0)
            {
                goto fail_7fffffff;   // ORIG 0x081c390c `je 0x081c3cd2`
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
            // [DPC_Process] ORIG 0x081c39b1 `mov -0x1c(%ebp),%eax; add $0xb24,%eax;`
            // 直接链式取 index（**无 bf 具名槽**）；旧文 `CBattle_Field* bf` 多 store
            // 一条且后续每处使用都退回槽读。
            const int dungeonIdx = party->m_battleField.get_dungeon_index();
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
    }
    if (user->get_state() == 10)
    {
        WongWork::CDeathTower* tower = user->getDeathTower();  // thunk 直调 0x86552a4
        // [K6-DpcProcess] ORIG 0x081c3b67/0x081c3bb0 两处 `je 0x081c3cd2`（与 state==5 同岛）
        if (tower == 0)
        {
            goto fail_7fffffff;
        }
        {
            if (tower->handlePickupItem(user, req.m_0d,
                                        req.m_0f == 1, (char)req.m_10) == 0)
            {
                goto fail_7fffffff;
            }
            out.m_errNo = 0;
            return 0;
        }
    }
    if (user->get_state() == 0xc)
    {
        // ORIG 0x81c3bdf..0x81c3c19：CBossTowerFunc::GetItem(user, m_0d, m_0f==1,
        // (char)m_10)，随后无条件置 m_errNo=0x7fffffff（asm +0x659）
        Global::CBossTowerFunc::GetItem(user, req.m_0d,
                                        req.m_0f == 1, (char)req.m_10);
        goto fail_7fffffff;   // ORIG 0x081c3c19 `jmp 0x081c3cd2`（无条件的共享岛直跳）
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
        if (room->HandleGetItem(user, req.m_0d) == 0)
        {
            goto fail_7fffffff;   // ORIG 0x081c3cae `je 0x081c3cd2`
        }
        out.m_errNo = 0;
        return 0;
    }
    out.m_errNo = 0x13;
    return 0;

    // [K6-DpcProcess] ORIG 0x081c3cd2（函数尾唯一岛）：
    //   mov -0x24(%ebp),%eax; movl $0x7fffffff,0x4(%eax); mov $0x0,%eax
    // 上述全部失败路径（0x3761/0x390c/0x3b67/0x3bb0/0x3c19/0x3cae）都直跳此处，
    // 旧形在每处内联该 3 条 ⇒ 结构性多 2 条/处。
fail_7fffffff:
    out.m_errNo = 0x7fffffff;
    return 0;
}
