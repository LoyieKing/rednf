// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::join_room（原 TU 第 1274 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

// §6.2 巨件核销：P167 102/102 [0缺0多]；gap 702→+7；残差归因 N82（ORIG 帧
// 0x9c/21 槽 vs 4.4 帧 0xac/23 槽，PacketGuard/CSwitchLog 对象槽平移）+ 4.4
// 布尔物化（sete/cmpb/movb/jcc+1）；逐块人工比对语义与 Ghidra C 一致（含
// OnJoinRoom(seat) 实参、m_state==2 判 0x13、双 PacketGuard 布局；详见
// docs/asm_translation_patterns.md 负例表）⇒ N 封顶。
// [b28d-7 N82] ORIG 0x085d6cf2：帧 $0x9c + 形参 spill 槽 -0x7c 起，4.4 帧 $0xac + 槽 -0x8c 起（不按 lifetime 复用）；702/708、P167 CLEAR ⇒ 封顶。
// [b28d-solo-PvP_Room] 本轮（708→706）：`flag ? 1 : 0`→`(int)flag`（ORIG movzbl 直扩）。
// [b28d-t3-PoolBigThree] **679→316**：bVar3 三段 if/else-if+槽读回 改 `if (a==0xf ||
// a==0x10)` 直判（ORIG 0x085d6dae..6de0 双调用+值岛 %eax 实证，见函数体判词）。
// 残余 316 = N82 帧 0x9c/0xac + 槽号整体平移（-0x6c↔-0x74 族）+ N7（ORIG iVar9→%ebx/
// iVar10→%esi callee-saved 缓存 cmp+setl/setle 物化，4.4 槽往返+jge/jg 直判，
// @0x085d6df1..）+ PacketGuard ctor 发射时刻。p167 102/102 CLEAR。
// [SweepPvPRoom] 复核 gap=2（700 vs 702）：多重集差为 ORIG 的 %edi/%esi/%ebx
// callee-saved 缓存（`mov %eax,%edi/%esi/%ebx` + 对应 push/pop）vs 4.4 槽往返 +
// 帧 $0x9c/$0xac；p167 call 差集空（dual_rank miss=0 extra=0）⇒ N82/N82附 封顶。
int PvP_Room::join_room(CUser* user, int& seat, bool flag)
{
    int local_7c[3];
    {
        // ORIG _ZZ…__PRETTY_FUNCTION__ 同文（P139）
        // [b28d-solo-PvP_Room] ORIG 0x085d6cf2：flag 以 `movzbl -0x7c(%ebp),%eax;
        // mov %eax,-0x88(%ebp)` 直扩（bool→int 转换），`flag ? 1 : 0` 会多 mov$1/$0
        // 物化岛 ⇒ 改 (int)flag。
        // [Lane10] P227/P37 全表达式临时链：ORIG 0x085d6d0a..0x085d6d47 在 CSwitchLog
        // ctor(0x085d6d6f) **之前**右起求全部 varargs 并以 callee-saved 跨 ctor 保活
        // （get_waiter_count→%edi、get_acc_name→%esi、getCurCharacName→%ebx；成员经
        // -0x88/-0x84/-0x80 槽）。具名对象两语句形把 ctor 提到求值之前 ⇒ 改临时链。
        // 末实参内联（ORIG 该值无槽往返 = P0 无具名局部）；第二次 get_waiter_count
        // （0x085d6e9c）才落 -0x44。
        CSwitchLog(__PRETTY_FUNCTION__, 0x4a8, 0, 0)(
            "pvp@log %s,%s,room(%d),%d,%d,%d", user->get_acc_name(),
            user->getCurCharacName(), m_index, m_pvpBattleMode, (int)flag,
            get_waiter_count());
    }
    // [b28d-t3-PoolBigThree] ORIG 0x085d6dae..6de0 实测：`call GetChannelType; cmp
    // $0xf; je <true岛>; call GetChannelType; cmp $0x10; jne <false岛>; mov $1,%eax;
    // jmp; mov $0,%eax; test %al; je` = **|| 短路求值直进条件**（两次独立调用、值岛
    // 在 %eax、无 bool 槽）。旧三段 if/else-if 产 `sete;test;je` + `movb $1/$0,-0x4d`
    // 槽往返（两处 +7 条）⇒ 改 if (a==0xf || a==0x10) 直判形。
    if ((G_GameWorld()->GetChannelType() == 0xf) ||
        (G_GameWorld()->GetChannelType() == 0x10))
    {
        // [Lane10] P0 双向判读 + 真语义修复（判据⑦族/比较操作数）：
        // ORIG 0x085d6def `mov %eax,%ebx`（GetMaxGradePvPChannel 结果，无槽往返
        // ⇒ 原码无具名局部）+ 0x085d6dfc `cmp %eax,%ebx; setl %al; test %al,%al;
        // je` ⇒ 内联比较表达式。
        // ORIG 0x085d6e2a `mov %eax,%esi`（GetGiveItem 结果，跨 2 个 call 保活、
        // 无槽）+ 0x085d6e4d `cmp %eax,%esi; setle` ⇒ 第二处比较的**左操作数是
        // GetGiveItem()**，非第一处的 GetMaxGradePvPChannel。旧重建写
        // `iVar9 <= iVar11`（iVar11 已被 get_pvp_item_give 覆盖）与 ORIG 语义
        // 不符（真 bug；Ghidra C 行 792/797 复用 iVar9=GetGiveItem 互证）。
        if (G_CDataManager()->GetMaxGradePvPChannel() < user->get_pvp_grade())
        {
            return 0xd;
        }
        if (((int)((fair_pvp::CFairPvPScore*)user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)7))->GetGiveItem())
            <= G_CDataManager()->m_serverParameter.get_pvp_item_give(user->get_pvp_grade()))
        {
            return 7;
        }
    }
    if (user->getCurCharacR() == 0)
    {
        return 0x13;
    }
    if (m_state == 2)
    {
        return 0x13;
    }
    // [Lane10] P226/P232 声明时机：ORIG 0x085d6ea1 才把第二次 get_waiter_count 写入
    // -0x44 槽，PacketGuard(-0x60/-0x6c) 构造紧随其后（0x085d6eaa/0x085d6eb5）；
    // 旧形把两 PacketGuard 声明在函数头 ⇒ ctor 提前发射到 CSwitchLog 之前。
    int local_48 = get_waiter_count();
    PacketGuard local_64;
    PacketGuard local_70;
    local_64.put_header(0, 2);
    local_70.put_header(0, 0xb);
    local_64.put_byte(1);
    local_64.put_short(local_48);
    local_70.put_byte(local_48);
    for (int local_3c = 0; local_3c < 8; local_3c = local_3c + 1)
    {
        if (P_SEATS(local_3c) != 0)
        {
            P_SEATS(local_3c)->make_basic_info((char*)&local_64, 1);
            local_70.put_short((int)P_SEATS(local_3c)->get_unique_id() & 0xffff);
            local_70.put_int(P_SEATS(local_3c)->m_network.get_inner_ip());
            local_70.put_int(P_SEATS(local_3c)->m_network.get_ip());
            local_70.put_short(P_SEATS(local_3c)->m_network.get_port() & 0xffff);
            local_70.put_int((int)P_SEATS(local_3c)->get_acc_id());
            local_70.put_byte(P_SEATS(local_3c)->m_network.get_nat_type());
            local_70.put_int(P_SEATS(local_3c)->m_network.get_mtu());
            // ORIG 0x085d709d..0x085d70c3（P48/P0 去缓存）：条件臂与 else 臂各自
            // 独立调用 getCurCharacR() 一次，不缓存到具名局部（缓存则单 call 不同形）。
            // [SweepPvPRoom-2] P59/P292 极性：ORIG 0x085d7095 `test; setne; test; je`
            // ⇒ 源形正极性 `!= 0`（旧 `== 0` 形 4.4 产 sete、且 setne 差集 +1）。
            if (P_SEATS(local_3c)->getCurCharacR() != 0)
            {
                // [Lane10] Main「视图 cast 反模式」洞察应用：ORIG 0x085d70xx
                // `movzbl 0x75(%eax),%eax` 单条 disp 折叠；旧形 `*(unsigned char*)
                // ((const char*)p + 0x75)`（及数组下标形）恒产 `add $0x75,%eax;
                // movzbl (%eax),%eax` 两条。改**权威头具名字段直访问**
                // （CUser.h:545 `unsigned char m_stamina; // +0x75`，宽度证据
                // SelectCharac 恢复/make_charac_stat 序列化）即复现单条 disp。
                local_70.put_byte(P_SEATS(local_3c)->getCurCharacR()->m_stamina);
            }
            else
            {
                local_70.put_byte(0);
            }
        }
    }
    unlock();
    local_64.finalize(true);
    local_70.finalize(true);
    lock();
    int local_44 = ((IMatch*)m_matching)->CheckJoinable(this, user);
    if (local_44 != 0)
    {
        unlock();
        return local_44;
    }
    int local_40 = -1;
    for (int local_38 = 0; local_38 < 8; local_38 = local_38 + 1)
    {
        if ((P_SEATS(local_38) == 0) && (P_TEAMS(local_38) == 0xff))
        {
            local_40 = local_38;
            P_SEATS(local_38) = user;
            user->SetPvpIndex((short)m_index);
            seat = local_38;
            break;
        }
    }
    if (local_40 == -1)
    {
        unlock();
        return 4;
    }
    user->Send(local_64);
    user->Send(local_70);
    if (m_pvpBattleMode == 1)
    {
        P_TEAMS(local_40) = 0;
        m_relayMgr.OnChangeTeam(local_40, 0);
    }
    else if (m_pvpBattleMode == 4)
    {
        P_TEAMS(local_40) = 0;
    }
    else if (flag)
    {
        int local_34 = 1;
        P_TEAMS(local_40) = 1;
        m_relayMgr.OnChangeTeam(local_40, 1);
    }
    else
    {
        local_7c[0] = 0;
        local_7c[1] = 0;
        local_7c[2] = 0;
        for (int local_28 = 0; local_28 < 8; local_28 = local_28 + 1)
        {
            if ((0 < P_TEAMS(local_28)) && (P_TEAMS(local_28) < 3))
            {
                local_7c[P_TEAMS(local_28)] = local_7c[P_TEAMS(local_28)] + 1;
            }
        }
        int local_30 = 8;
        int local_2c = 1;
        for (int local_24 = 1; local_24 < 3; local_24 = local_24 + 1)
        {
            if (local_7c[local_24] <= local_30)
            {
                local_2c = local_24;
                local_30 = local_7c[local_24];
            }
        }
        P_TEAMS(local_40) = local_2c;
        m_relayMgr.OnChangeTeam(local_40, local_2c);
    }
    unlock();
    local_64.clear();
    local_64.put_header(0, 2);
    local_64.put_byte(1);
    local_64.put_short(1);
    user->make_basic_info((char*)&local_64, 1);
    local_64.finalize(true);
    send_to_pvp(local_64);
    local_70.clear();
    local_70.put_header(0, 0xb);
    local_70.put_byte(1);
    local_70.put_short((int)user->get_unique_id() & 0xffff);
    local_70.put_int(user->m_network.get_inner_ip());
    local_70.put_int(user->m_network.get_ip());
    local_70.put_short(user->m_network.get_port() & 0xffff);
    local_70.put_int((int)user->get_acc_id());
    local_70.put_byte(user->m_network.get_nat_type());
    local_70.put_int(user->m_network.get_mtu());
    // ORIG 0x085d..（P48/P0 去缓存）：条件臂与 else 臂各自独立调用
    // getCurCharacR() 一次（Ghidra C 双臂 getter），不缓存到具名局部。
    // [SweepPvPRoom-2] P59/P292 极性：ORIG 0x085d755c `test; setne; test; je`
    // ⇒ 源形正极性 `!= 0`（旧 `== 0` 形 4.4 产 sete）。
    if (user->getCurCharacR() != 0)
    {
        // [Lane10] 同 1327 处：改权威具名字段 m_stamina（CUser.h:545，+0x75）。
        local_70.put_byte(user->getCurCharacR()->m_stamina);
    }
    else
    {
        local_70.put_byte(0);
    }
    local_70.finalize(true);
    send_to_pvp(local_70);
    local_64.clear();
    local_64.put_header(1, 0x36);
    local_64.put_byte(1);
    lock();
    for (int local_20 = 0; local_20 < 8; local_20 = local_20 + 1)
    {
        local_64.put_byte(P_SEATSTATE(local_20));
    }
    unlock();
    local_64.finalize(true);
    user->Send(local_64);
    user->m_winPointBuffer = 0;   // ORIG CUser.h:2282 具名字段
    m_joinCount = m_joinCount + 1;
    m_relayMgr.OnJoinRoom(seat);
    ((IMatch*)m_matching)->OnJoinRoom(user);
    return 0;
}
