void CFairMatch::OnEndFight(PvP_Room* room)
{
    // b28d-solo-CFairMatchRooms：P211/P274 —— 局部 unsigned 与 _ReturnTeam 的
    // int 形参异型，令 4.4 先求值第 3 参再写第 4 参常量（ORIG 4.1 装载序）。
    const unsigned int mgrTeam = room->get_manager_team();
    m_field04 = 0;
    // b28d-solo-CFairMatchRooms：ORIG（Ghidra C）为 if/else 双分支布局（timer
    // 在 then、_ReturnTeam 在 else），且 gen_timer_key 结果直接进 %esi 跨
    // get_index/G_TimerQueue 调用保活（无 key 具名槽）。
    // ORIG `cmp $0x1,%al; setne %al; test; je <timer 块>` ⇒ 否定比较 +
    // _ReturnTeam 为 then 直落、timer 为 else 尾块（P222 反向布局）。
    if (static_cast<unsigned char>(GetPlayCountPerOneTeam()) != 1) {
        // b28d-solo-CFairMatchRooms：ORIG 在 -0x1c 保存 _ReturnTeam 返回值（后续
        // 未读的死写入，P171 照抄）；「声明即初始化」形令 4.4 给 mgrTeam 深槽
        // -0x20、返回值浅槽 -0x1c（探针 /tmp/fm_p4.cpp c/d 形）。
        PvP_Room* releasedRoom = _ReturnTeam(room, mgrTeam, 0);
        (void)releasedRoom;
        if (mgrTeam != 1) {
            // b28d-solo-CFairMatchRooms：P83/N84——ORIG 是结构整体赋值
            // `rep movsl` 0x12 dwords（TeamInfoThird 0x48 字节）。
            m_teams[0] = m_teams[1];
        }
        m_teams[0].refresh();
        m_teams[1].reset();
    } else {
        // 单人模式：0x3e 定时器 0ms 兜底唤醒战房清算链
        G_TimerQueue()->InsertTimerInMilisecond(
            static_cast<TimerEntry::OBJ_TYPE>(1), room->get_index(),
            static_cast<TIMER_MESSAGE>(0x3e),
            static_cast<long long>(0),
            static_cast<int>(
                room->gen_timer_key(static_cast<TIMER_MESSAGE>(0x3e))),
            0);
    }
