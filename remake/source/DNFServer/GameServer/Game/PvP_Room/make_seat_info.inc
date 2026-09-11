// [b28d-7 N82附] ORIG 0x085db9e2：vptr/名称结果以 %edi/%esi/%ebx callee-saved 跨 3 调用缓存（push 对 +2），4.4 无 push 且改槽中转（199/193）；P167 CLEAR ⇒ 封顶。
// [b28d-solo-PvP_Room] N82附: ORIG 0x085db9e2 vptr/名称结果 callee-saved 跨 3 调用缓存，4.4 改槽中转
// [SweepPvPRoom] 193→198（ORIG 199）：P227 全表达式临时链复现 cMyTrace ctor 前求
// get_unique_id→%esi / team→%ebx 并跨 ctor 保活（补回 push/pop %esi/%ebx）；旧
// 「N82附 callee-saved 缓存封顶」判词作废。残余 1 条 = ORIG 0x085dba25 `mov %eax,%edx`
// （成员实参中转，P75/N7 装载序：多重集其余差异全为 %edx↔%eax 寄存器名互换）。
// [L11 复核 2026-09-11 · 封顶] 同 make_room_info：receiver 槽 -0x14 两侧同址，199 行差
//   全为 P75 装载序/寄存器角色（%edx 暂存 vs %eax 直压、`movswl %ax,%edx` vs `cwtl`）
//   ⇒ 4.1↔4.4 call-expansion 固有差，无源码解。
void PvP_Room::make_seat_info(char* buf, int seat)   // [b28c-16 SIGFIX] ORIG 串 void
{
    // ORIG 0x085db9e2（199；本侧 193，-6 全为 N 类形态差，multiset 核对无语义
    // 缺口）：ORIG 将 vptr 读值/getCurCharacName/get_acc_name 结果以 %edi/%esi/
    // %ebx callee-saved 跨 3 次调用缓存（push 对 +2、`mov (%eax),%edx` 直落
    // outgoing 槽 vs 4.4 `mov…,%eax; mov %eax,…` 两段，N7/N18a），错误码/日志
    // 串/PRETTY 符号逐条一致。
    // [b28d-t3-PoolBigThree] 121→**106**：else 段 `P_SEATS(seat)` 检查改正极性
    // `!=0`（ORIG 0x085dbc6x `je 85dbc83` 零分支落尾岛、uid/geo fall-through，
    // 旧 `==0` 形两分支体与 ORIG 错位）。残余 = N7/N18a callee-saved 缓存（上）。
    InterfacePacketBuf* local_18 = (InterfacePacketBuf*)buf;
    local_18->put_header(0, 0x2b);
    local_18->put_short(m_index);
    // [L10] P265 实测无效（同上 make_room_info 注释），记 N。
    local_18->put_byte(m_pvpBattleMode);
    if (seat == 0xffff)
    {
        int local_14 = get_waiter_count();
        local_18->put_byte(local_14);
        if (local_14 == 0)
        {
            LogManager::logFormat(1, "pvp.cpp", __PRETTY_FUNCTION__,
                                  0xe66, "Send Member 0 PvpRoom %d State(%d)", m_index, m_state);
        }
        lock();
        for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
        {
            if (P_SEATS(local_10) != 0)
            {
                local_18->put_byte(local_10);
                local_18->put_byte(P_TEAMS(local_10));
                local_18->put_short((int)P_SEATS(local_10)->get_unique_id() & 0xffff);
                local_18->put_byte((int)P_SEATS(local_10)->isAllowLocationGeoState());
                // [SweepPvPRoom] P227 全表达式临时链（旧 N82附 判词作废）：ORIG
                // 0x085dbb5e `call get_unique_id; movzwl %ax,%esi` 与 0x085dbb6f
                // `mov (%eax,%edx,4),%ebx`（team）都在 ctor（0x085dbb90）之前求值并
                // 以 callee-saved 跨 ctor 保活；两语句形把 ctor 提到求值之前 ⇒ 缺
                // push/pop %esi/%ebx 与两条 callee-saved 装载（gap 6 的主体）。
                cMyTrace(__PRETTY_FUNCTION__, 0xe74, 0)(
                    "RELAY_BATTLE SeatInfo player(%d), team(%d), uid(%d)\n",
                    local_10, P_TEAMS(local_10),
                    (int)P_SEATS(local_10)->get_unique_id() & 0xffff);
            }
        }
        unlock();
    }
    else
    {
        local_18->put_byte(1);
        lock();
        local_18->put_byte(seat);
        local_18->put_byte(P_TEAMS(seat));
        // [b28d-t3-PoolBigThree] P59 块序：ORIG 0x085dbc6x `je 85dbc83`（零分支
        // 落尾岛）非零分支 fall-through 直落 uid/geo ⇒ 正极性 `!= 0`。
        if (P_SEATS(seat) != 0)
        {
            local_18->put_short((int)P_SEATS(seat)->get_unique_id() & 0xffff);
            local_18->put_byte((int)P_SEATS(seat)->isAllowLocationGeoState());
        }
        else
        {
            local_18->put_short(0xffff);
            local_18->put_byte(0);
        }
        unlock();
    }
    local_18->finalize(true);
}