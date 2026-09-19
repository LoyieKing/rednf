// [b28d-7 P75] ORIG 0x085db7f6：成员值先物化 %edx 再压参（`mov (%eax),%edx; mov %edx,0x4(%esp)`），4.4 折成 eax 直压（P75 装载时刻/寄存器角色）⇒ 封顶，P167 CLEAR。
// [b28d-solo-PvP_Room] 本轮：P59/P272 —— 循环内 `P_SEATS(i)==0` 改正极性 `!=0`（ORIG
// `test; je <T>` 非零分支直落）⇒ 分支布局归位；残余仅 P75 装载序。
// [SweepPvPRoom] 残余 1 条 = N4/P30：ORIG 0x085db860 `lea 0x9(%eax),%ecx`（put_str 实参
// 单条取址）vs 4.4 `mov this,%edx; add $0x9,%edx`；`(char*)&m_roomName`/`&m_roomName[0]`/
// `m_roomName + 0` 三形实测均仍出 add（139 不变）⇒ add-vs-lea 伪影封顶。
// [L11 复核 2026-09-11 · 封顶] receiver 槽 -0x14 两侧同址；45/140 行差全为 P75 装载序
//   （ORIG `mov (%eax),%edx; mov %edx,0x4(%esp)` 值经 %edx 暂存 / 4.4 %eax 直压 +
//   `movsbl %al,%edx` vs `cwtl` 符号扩展形）⇒ 跨版本固有。
int PvP_Room::make_room_info(char* buf)
{
    InterfacePacketBuf* local_ec = (InterfacePacketBuf*)buf;
    local_ec->put_short(m_index);
    local_ec->put_byte((int)m_mode8);
    if (m_mode8 == 0)
    {
        size_t len = strlen(m_roomName);
        local_ec->put_int((int)len);
        // [SweepPvPRoom] N4/P30 封顶：ORIG 0x085db860 `lea 0x9(%eax),%ecx`（单条取址）
        // vs 4.4 `mov this,%edx; add $0x9,%edx`；`(char*)&m_roomName`/`&m_roomName[0]`/
        // `m_roomName + 0` 三形实测均仍出 add（139 不变）⇒ add-vs-lea 伪影。
        local_ec->put_str(m_roomName, (int)len);
    }
    local_ec->put_byte(m_state);
    local_ec->put_byte(m_managerSeat);
    // [SweepPvPRoom] P200：去掉冗余 (int) cast ⇒ short 实参直传复现 ORIG
    // `movswl %ax,%edx`（cast 形恒退化成 `cwtl` 单条，少 1 条）。
    local_ec->put_short(m_requestedMapNo);
    // [L10] 判词（三形实测 2026-09-10，三件 gap=1 同源）：ORIG 0x085db81d 段
    //  `mov 0x4(%eax),%eax; mov %eax,%edx; mov %edx,0x4(%esp)`（字段→eax→edx 两段
    //  中转），make_seat_info 0x085dba25 / make_state_info 0x085dbdc4 同形。
    //  ① 字段 unsigned + 实参 (PVP_BATTLE_MODE) cast ⇒ 4.4 仍单段（P265 不成立）；
    //  ② 字段改 int ⇒ 中转出现（三件各 +1）但 set_pvp_mode NEAR + revengeMission
    //     DIFF 双回归（ae 130→128、near 7→8、diff 18→19）⇒ 净负，已回退；
    //  ③ 原形（unsigned，直传）= 单段。
    //  ⇒ 记 N（跨版本字段-实参两段中转；字段类型旋钮须全消费段联判，已测并回退）。
    // [L10b 复核 2026-09-10] 再实测**字段 enum 化**（`PVP_BATTLE_MODE m_pvpBattleMode`
    // + 全部赋值点 cast）：TU 四数 34/131/6/18 逐值不变、本件仍 139（ORIG 140）⇒
    // 4.4 对 enum 字段 load 与 int/unsigned 同形。探针 /tmp/probe_p265b.cpp 九形
    // （int/unsigned/enum 字段 × int/enum 形参 × 有无 (E) cast）**全部**单段
    // `mov (%eax),%eax; mov %eax,(%esp)`（4.4.7）⇒ ORIG 的 `mov %eax,%edx` 属 4.1
    // 保守 regmove（N18a 族，反向亦不可造），已封顶。
    local_ec->put_byte(m_pvpBattleMode);
    for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
    {
        local_ec->put_byte(P_TEAMS(local_10));
        // [b28d-solo-PvP_Room] P59/P272：ORIG 0x085db7f6 为 `test; je <T>`，
        // 非零分支直落、零分支落 <T> ⇒ 源形须正极性 `!= 0`（旧 `== 0` 反产 jne 布局）。
        if (P_SEATS(local_10) != 0)
        {
            local_ec->put_short((int)P_SEATS(local_10)->get_unique_id() & 0xffff);
            local_ec->put_byte((int)P_SEATS(local_10)->isAllowLocationGeoState());
        }
        else
        {
            local_ec->put_short(0xffff);
            local_ec->put_byte(0);
        }
    }
    local_ec->put_byte(m_isExistPassword);
    local_ec->put_int(((IMatch*)m_matching)->GetIndentification());
#if !remake_verify
    return 0;   // DNF-FIX: 非 void 函数落底 —— g++ 13 在此发 ud2（ORIG 的 GCC 4.x 只是返回残留 eax）
#endif
}