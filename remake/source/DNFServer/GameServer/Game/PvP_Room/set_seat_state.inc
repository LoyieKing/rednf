// [b28d-7 N82] ORIG 0x085d77c0：push %edi/%esi + 槽 -0x3c/-0x40/-0x44，4.4 无 push 且槽 -0x1c（帧 0x6c vs 0x64）；P167 CLEAR ⇒ 封顶。
// [b28d-solo-PvP_Room] 本轮（403→406）：P227 CSwitchLog 全表达式临时链 ⇒ get_acc_name→%esi/
// getCurCharacName→%ebx 提前到 ctor 前（与 ORIG 同向）；残余 N82 槽号（-0x3c..-0x44 vs
// -0x14..-0x1c）+ 块序重排。
// [SweepPvPRoom] 406→409（ORIG 409，gap 归零）：P231 裸 register 使 idx 入 %edi 跨
// CSwitchLog ctor 保活（补回 push/pop %edi）+ P97 两处 change_team 第二实参改形参
// state（ORIG 0x085d78b2 `mov 0x14(%ebp),%eax; mov %eax,0x8(%esp)`，旧形硬编码
// 0xfe/0xff 立即数）。残余 = N82 槽号整体偏移 + P251/P59（if 链 vs switch 寄存器比较
// `mov 0x14(%ebp),%eax; cmp $0xfe,%eax`）+ 1 处 test/sete 物化。
int PvP_Room::set_seat_state(CUser* user, int seat, ENUM_SEAT_STATE state,
                             CUser** out, bool& a, bool& b, bool& c)
{
    if (user == 0)
    {
        return 0;
    }
    int local_24;
    {
        // [L11 已证实 2026-09-11] 同 leave_room：ORIG 0x085d77c0 的 -0x44/-0x40/-0x3c
        // 是**实参求值的匿名溢出槽**（逆序：wc,st,mode 溢出；idx,charac,acc 落
        // callee-saved edi/ebx/esi），非具名局部槽。旧形攒具名 wc/st/mode/idx 导致
        // 4.4 给槽 -0x28/-0x24/-0x20 + 对象深槽 -0x3c。全内联实参形即复现。
        CSwitchLog(__PRETTY_FUNCTION__, 0x594, 0, 0)(
            "pvp@log %s,%s,room(%d),%d,%d,%d,%d", user->get_acc_name(),
            user->getCurCharacName(), get_index(), get_pvp_battle_mode(), seat,
            (int)state, get_waiter_count());
    }
    // [SweepPvPRoom-2] P272/P59 块序交换：ORIG 0x085d7888 `mov 0x14(%ebp),%eax;
    // cmp $0xfe,%eax; je <A 体>` + `cmp $0xff,%eax; jne <C 体>`（B 体直落）；
    // 反写 `if (state != 0xfe) { if (state == 0xff) B else C } else { A }` 复现该
    // 物理序（旧 `if (state == 0xfe) A else if …` 形 4.4 把 A 体直落、B/C 后置）。
    // [SweepPvPRoom-2] P286/P324：ORIG 0x085d7888 的「形参载入 + 寄存器比较」
    // 链（`mov 0x14(%ebp),%eax; cmp $0xfe,%eax; je A; cmp $0xff,%eax; jne C`）
    // 由 switch 复现（探针 probe_enum3 逐条同形）；叶块物理序 = 书写序
    // ⇒ case 顺序 0xff / 0xfe / default（旧 if 反写形 4.4 恒 cmpl 内存直比）。
    switch (state)
    {
    case (ENUM_SEAT_STATE)0xff:
        {
            if (m_seats[7] != user)
            {
                return 8;
            }
            // [SweepPvPRoom] P97：ORIG 同形（第二实参取形参 state，非立即数 0xff）。
            change_team(seat, (int)state);
            a = true;
        }
        break;
    case ENUM_SEAT_STATE_0xfe:
        {
        if (m_state == 2)
        {
            if (P_SEATS(seat) == 0)
            {
                return 4;
            }
            if (m_seats[7] == P_SEATS(seat))
            {
                select_new_manager();
                a = true;
                b = true;
            }
            *out = P_SEATS(seat);
            GiveOuterPenalty(*out);
            P_SEATS(seat)->SetPvpIndex(-1);
            local_24 = get_team(seat);
            lock();
            P_SEATSTATE(seat) = 0;
            P_TEAMS(seat) = 0xff;
            P_SEATS(seat) = 0;
            m_joinCount = m_joinCount - 1;
            P_WIN(seat) = 0;
            unlock();
            m_relayMgr.OnLeaveRoom(seat);
            ((IMatch*)m_matching)->OnLeaveRoom(*out, this, local_24, seat);
            if (get_waiter_count() == 0)
            {
                reset();
                a = true;
                b = true;
                c = true;
            }
            else
            {
                if (check_winner() != 0)
                {
                    b = true;
                    pvp_request_pvp_rank();
                }
                a = true;
            }
        }
        else if (m_seats[7] == user)
        {
            // [Lane10] P292/T3（callee bool 化）+ P272/P257 块序：ORIG 0x085d9690
            // `mov (%eax,%edx,4),%eax; test %eax,%eax; je <then 块>`（==0 时跳到
            // **then 块**，then 物理居函数后段、else 体直落）⇒ 源码须「交换两体 +
            // 取反条件」。旧形 `if (==0) { then } else { else体 }` 产 `jne <else>`
            // （极性反）。then 内 `call is_closeable_seat_state; test %al,%al; je
            // <后段>`（无 setcc ⇒ callee bool，调用点正极性）。
            if (P_SEATS(seat) != 0)
            {
                P_SEATS(seat)->SetPvpIndex(-1);
                if (P_SEATS(seat) == m_seats[7])
                {
                    select_new_manager();
                    b = true;
                }
                *out = P_SEATS(seat);
                P_SEATSTATE(seat) = 0;
                P_TEAMS(seat) = 0xff;
                P_SEATS(seat) = 0;
                m_relayMgr.OnChangeTeam(seat, 0xff);
                if (get_waiter_count() == 0)
                {
                    reset();
                    a = true;
                    b = true;
                    c = true;
                }
                else
                {
                    a = true;
                    b = true;
                }
            }
            else
            {
                if (is_closeable_seat_state())
                {
                    // [SweepPvPRoom] P97：ORIG 0x085d78b2 `mov 0x14(%ebp),%eax;
                    // mov %eax,0x8(%esp)` ⇒ 第二实参是形参 state，非硬编码 0xfe。
                    change_team(seat, (int)state);
                    a = true;
                }
                else
                {
                    return 0x13;
                }
            }
        }
        else if (P_SEATS(seat) != 0)
        {
            P_SEATS(seat)->SetPvpIndex(-1);
            *out = P_SEATS(seat);
            int local_20 = get_team(seat);
            P_SEATSTATE(seat) = 0;
            P_TEAMS(seat) = 0xff;
            P_SEATS(seat) = 0;
            m_joinCount = m_joinCount - 1;
            m_relayMgr.OnLeaveRoom(seat);
            ((IMatch*)m_matching)->OnLeaveRoom(*out, this, local_20, seat);
            if (get_waiter_count() == 0)
            {
                reset();
                a = true;
                b = true;
                c = true;
            }
            else
            {
                a = true;
            }
        }
        }
        break;
    default:
        {
            if (P_SEATS(seat) != user)
            {
                return 8;
            }
            change_team(seat, (int)state);
            a = true;
        }
        break;
    }
    return 0;
}