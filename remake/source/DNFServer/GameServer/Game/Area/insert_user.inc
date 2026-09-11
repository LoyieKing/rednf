// ---------------------------------------------------------------------------
// insert_user：进图（村庄广播 0x18 / 普通广播 0x17 + 各管理器 OnMoveArea）
// ---------------------------------------------------------------------------
// [b28d-solo-Area] verdict=AE（P167 CLEAR 63=63；313=313）。修法：①try/catch 补全
// （086c275f..086c2821，__cxa_begin_catch×1/end_catch×2 + errno/strerror），**catch 末尾
// return** 复现 ORIG 086c289d `end_catch; jmp 086c2a22`（原实现 fall-through 继续发包 =
// 真语义 bug：异常路径本应直接返回）；②`unsigned short id;` **声明前置于 PacketGuard**
// 使 4.4 槽位集合与 4.1 全等（PacketGuard -0x30 / it -0x34 / id -0x22；旧形 id 落 -0x32）；
// ③insert 实参 `pair<const unsigned short, CUser*>(...)`（C1ItRS2_EEOT_OT0_，P234/P315）；
// ④logFormat/put_short 按 P92 重新调 get_unique_id；iterator 先声明后赋值（P50）。
void Area::insert_user(CUser* user)
{
    // [b28d-solo-Area] 原 N82 封顶判词作废：`unsigned short id;` 前置于 PacketGuard
    // 使 4.4 槽位集合与 4.1 全等（-0x30/-0x34/-0x22），帧 0x60 一致。
    unsigned short id;
    PacketGuard packet;

    if (m_areaType == 1)
    {
        user->set_state(ch_state_3);
        packet.put_header(0, 0x18);
        packet.put_byte(m_villageIndex);
        packet.put_byte(user->get_area(true));
        packet.put_short(1);
        packet.put_short(user->get_unique_id());
        packet.put_short(user->get_posX());
        packet.put_short(user->get_posY());
        packet.put_byte(user->get_direction());
        packet.put_byte(user->get_charac_visible_values());
        packet.finalize(true);
        user->Send(packet);
        return;
    }

    if (m_areaType == 2)
        user->set_state(ch_state_4);
    else
        user->set_state(ch_state_3);

    // [b28d-solo-Area] ORIG 086c2734 iterator 默认构造（P50 先声明后赋值）；
    // 086c27a1 失败分支 logFormat 与 086c28ad put_short 都重新调 get_unique_id
    // （P92 按 ORIG 来源读取，不用 id 槽）。
    std::map<unsigned short, CUser*>::iterator it;
    id = user->get_unique_id();
    // [b28d-solo-Area] ORIG 086c275f..086c2821 为 try 块（catch @086c2842，
    // __cxa_begin_catch×1 + __cxa_end_catch×2）：find/insert/put_header 全在
    // try 内；insert 用 `pair<const unsigned short, CUser*>`（ORIG mangled
    // `C1ItRS2_EEOT_OT0_`），catch 打印 errno 串。
    try
    {
        it = m_UsersInArea.find(id);
        if (it != m_UsersInArea.end())
        {
            LogManager::logFormat(1, "world.cpp",
                                     __PRETTY_FUNCTION__, 0x1a7,
                                     "m_UsersInArea.find(%d)",
                                     (int)user->get_unique_id());
            return;
        }

        m_UsersInArea.insert(std::pair<const unsigned short, CUser*>(
            user->get_unique_id(), user));

        packet.put_header(0, 0x17);
    }
    catch (...)
    {
        LogManager::logFormat(1, "world.cpp", __PRETTY_FUNCTION__, 0x1af,
                                 "[EXCEPTION insert_user In AREA] - code = %s",
                                 strerror(errno));
        // ORIG 086c289d `call __cxa_end_catch; jmp 086c2a22`（直达 epilogue）
        // ⇒ catch 末尾必须 return（异常时不继续发包）。
        return;
    }

    packet.put_short(user->get_unique_id());
    packet.put_byte(m_villageIndex);
    packet.put_byte(user->get_area(true));
    packet.put_short(user->get_posX());
    packet.put_short(user->get_posY());
    packet.put_byte(user->get_direction());
    packet.put_byte(user->get_charac_visible_values());
    packet.finalize(true);

    if (user->IsCurCharacVisible())
        send_to_all(packet);
    else
        user->Send(packet);

    send_area_users(user);
    pvp_assault::GetInstanceAssaultMgr()->OnMoveArea(user);
    GlobalData::s_villageObjectMgr->on_move_area(user);
    GlobalData::s_villageMonsterMgr->OnMoveArea(user);
}