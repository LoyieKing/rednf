// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::leave_room（原 TU 第 1493 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

// [b28d-7 N82附] ORIG 0x085d7f6c：具名局部落 -0x44/-0x40/-0x3c，4.4 同集合落 -0x2c/-0x28/-0x24（整体浅移 0x18，帧同 171）；同 TU callee 定义位置已核（P210 不适用）⇒ 槽分配策略差封顶，P167 CLEAR。
// [b28d-solo-PvP_Room] N82附: ORIG 0x085d7f6c 具名局部槽整体浅移 0x18（帧同 171）
int PvP_Room::leave_room(CUser* user, bool& flag)
{
    // [b23S3C] P37/P52 日志形：ORIG ctor 前 6 值分两路物化 —— waiter/flag/mode
    // 落槽 -0x44/-0x40/-0x3c（具名局部），index/name/acc 经 callee-saved
    // edi/esi/ebx 跨 ctor 调用缓存（register 形）；varargs 装载序
    // ebx=acc,esi=name,edi=index,槽 mode/flag/waiter。
    // P16/P56：fail 分支直接 `return -1`（ORIG mov$-1;jmp epilogue，
    // 非槽写-1再共享尾读 —— 旧形多 1 条即 sheet gap=-1 来源）。
    int local_44 = get_waiter_count();
    int local_40 = flag;
    int local_3c = get_pvp_battle_mode();
    register int roomIndex = get_index();
    register const char* characName = user->getCurCharacName();
    register const char* accName = user->get_acc_name();
    // [L10] P221 反向试验实测（2026-09-10，已回退）：ORIG 对象槽 -0x30（浅于标量
    // -0x44/-0x40/-0x3c）；临时链与具名两形 4.4 均给对象深槽（-0x3c）、标量浅
    //（-0x2c/-0x28/-0x24）⇒ 对象/标量分区方向跨版本不可翻（N82附），维持临时链。
    CSwitchLog(__PRETTY_FUNCTION__, 0x6c2, 0, 0)("pvp@log %s,%s,room(%d),%d,%d,%d",
             accName, characName, roomIndex, local_3c, local_40, local_44);
    lock();
    if (m_seats[7] == user)
    {
        select_new_manager();
        flag = true;
    }
    int local_24 = get_user_seat(user);
    if (local_24 < 0)
    {
        return -1;
    }
    {
        if (get_state() == 2)
        {
            GiveOuterPenalty(user);
        }
        int local_20 = get_team(local_24);
        user->SetPvpIndex(-1);
        P_SEATSTATE(local_24) = 0;
        P_TEAMS(local_24) = 0xff;
        P_SEATS(local_24) = 0;
        m_joinCount = m_joinCount - 1;
        P_WIN(local_24) = 0;
        P_TEAMWIN(local_24) = 0;
        unlock();
        user->getHackAnalyzer()->reportHackInfo();
        user->getHackAnalyzer()->resetHackInfo();
        if (m_state == 2)
        {
            // [b23S3C] P9/P159：check_winner 头改 bool（ORIG 体尾 movw $0,%ax），
            // 调用点裸形即复现 ORIG test %al 直判。
            if (check_winner())
            {
                pvp_request_pvp_rank();
            }
        }
        m_relayMgr.OnLeaveRoom(local_24);
        ((IMatch*)m_matching)->OnLeaveRoom(user, this, local_20, local_24);
    }
    return local_24;
}
