// ---------------------------------------------------------------------------
// check_allmember_loading —— ORIG T 0x085b17a4
// 报告：docs/class_func_reports/CParty/check_allmember_loading.md
// ---------------------------------------------------------------------------
// [b28d-solo-CPartyDepsD2] gap 16→-7（454 vs 461），p167 真缺口归零（仅余
// C1/C2、D1/D2 同址别名 + 1 处 _Unwind_Resume，P93/N104）。本轮真修：
// ①[真缺口] trace 第 2 参由 `m_partyIndex` 直读改 out-of-line `GetPartyIndex()`
//   （ORIG 0x085b17d4 call，非 mov 0x64）；②[真缺口] GetDungeon 单次调用（旧码
//   `if(bf(this).GetDungeon()!=0){CDungeon* d=bf(this).GetDungeon();}` 调两次，
//   ORIG 0x085b1d04 单次存 -0x24）。形态（P0/P24/P227/P292）：首/次循环改
//   `if(!_checkValidUser) continue;` 否定早退、pcRoomTrace/user 具名局部删除、
//   两处 trace 改全表达式临时链（ORIG ctor 前求 varargs）、三处 key 内联实参、
//   acc-tick 表达式内联（LHS-first）、GetPCRoomNo 改空 if 体复现 `test;jmp`。
// 残余：帧 0x9c vs 0xac、flag 形参 4.1 cmpb 直比 vs 4.4 movzbl+xor（P209/N）、
// EH 尾块相对序（N104）、PCRoom trace 两 vararg 寄存器 esi↔ebx 互换（N7）。
// [b28d-solo-CPartyDepsD2²] 定位：p167 唯一 EXTRA = `_Unwind_Resume`（+1）——
// ORIG 0x085b17d9..0x085b1810 首个 cMyTrace 块尾**无 cleanup**（直 `jmp +0x6b6`），
// 我方多 13 条 EH cleanup（`mov %edx,%ebx; mov %eax,%esi; … call ~cMyTrace;
// mov %esi,%eax; mov %ebx,%edx; mov %eax,(%esp); call _Unwind_Resume`，子块入口
// +0x1bd）⇒ 4.4 对「同 TU 可见性未知的 cMyTrace ctor call」布 landing pad（4.1 不布）。
// 按 P216 需在 GameTypes.h 给 `cMyTrace::cMyTrace(char const*, int, int)` 声明+
// 定义加 `throw()`（全仓热点头，属共享头 ⇒ 已上报 Main 派属主，本泳道不自行落）。
// 该 13 条即 gap −7 的主体（另 6 条为槽序差）。
void CParty::check_allmember_loading(bool flag)
{
    const int memberCount = get_member_count();
    if (memberCount == 0)
    {
        // ORIG 0x085b17d4 为 out-of-line `call CParty::GetPartyIndex()`（非 m_partyIndex 直读）。
        // P227 全表达式临时链：0x085b17d9 先 call GetPartyIndex 存 %ebx，再 ctor
        // （0x085b17f9），最后压参。
        cMyTrace(__PRETTY_FUNCTION__, 0x30e0, 0)(
            "<IN_DUNGEON_NULL_PARTY> not_member PartyIndex(%d)", GetPartyIndex());
        destroy();                                       // T 0x0859ad4c
        LogManager::logFormat(
            1, "party.cpp", __PRETTY_FUNCTION__, 0x30e3,
            "CParty::check_allmember_loading : 1 Using: %d, Member = 0\n",
            (unsigned int)(unsigned char)m_field68);
        return;
    }

    int loadedCount = 0;                                 // -0x2c
    for (int i = 0; i < 4; ++i)
    {
        // P292/P133：ORIG 0x085b1881 `xor $1; test; jne`（bool callee 否定早退）；
        // 合并 `&&` 会多 setcc 物化。
        if (!_checkValidUser(i))
            continue;
        if (m_battleData.m_arr50[i])                     // +0x37c+i byte
            ++loadedCount;
    }
    if (loadedCount != memberCount)
    {
        // ORIG 0x085b18b8 `cmpb $0x0,-0x6c(%ebp); je`（4.1 形参 spill 后内存直比）；
        // 4.4 对 bool 形参恒 movzbl+xor 物化（P209/N，`==0` 写法实测同形）。
        if (!flag)
            return;
        if (loadedCount < 1)
            return;
    }

    bf(this).ResetFirstEnterDungeon();                   // T 0x08307662

    {   // 广播 0x1e 包：put_int(BF+0x134)
        PacketGuard packet;
        packet.put_header(0, 0x1e);
        packet.put_int(bf(this).m_field134);
        packet.finalize(true);
        send_to_party(packet);
    }

    // 副本游玩时间累计：acc(+0x358/0x35c) += now - moveTick(+0x360/0x364)。
    // [b28d-solo-CPartyDepsD2] ORIG 0x085b195a..0x085b1ee 为 64 位
    // sub/sbb + add/adc 链（now 零扩展 64 位，具名 now/moveTick 槽 -0x80/-0x78）。
    // P0/P211：ORIG 0x085b1942 先读 acc(+0x358/0x35c) 再 call getCurTickCount
    // （LHS-first）⇒ 表达式内联，不设 now/moveTick 具名局部。
    bd(this).m_accTick += GlobalData::s_systemTime_.getCurTickCount()
                          - bd(this).m_moveTick;                 // +0x360/0x364
    // 地图基准 tick：base(+0x33c/0x340) = now - enterTick(+0x344/0x348)（无局部槽）。
    bd(this).m_baseTick = GlobalData::s_systemTime_.getCurTickCount()
                          - bd(this).m_enterTick;

    // PCRoom 统计上下文：非自动建队且人数>1 时开启追踪
    // P0：ORIG 0x085b19f9 直接 `movzbl %al,%ebx`（无 bool 槽）⇒ 内联实参。
    traceHack(this).traceBegin(GlobalData::s_systemTime_.getCurSec(),
                               !(IsAutoCreated() || get_member_count() == 1));

    for (int i = 0; i < 4; ++i)
    {
        if (!_checkValidUser(i))
            continue;
        // P0：ORIG 每点重算 m_member[i].m_pUser（无指针槽往返）。
        if (m_member[i].m_pUser->get_state() < 3 ||
            m_member[i].m_pUser->getCurCharacR() == 0)   // thunk 0x080da38c
        {
            // P227：ORIG 0x085b1ad4 NumberToString→%ebx、0x085b1af5 get_state→%esi
            // 均在 ctor（0x085b1b15）之前求值并跨 ctor 保活。
            cMyTrace(__PRETTY_FUNCTION__, 0x3123, 5)(
                "PCROOM : Dispatcher_RequestPCRoomPlayerCount => User State : %d, "
                "m_id : %s",
                m_member[i].m_pUser->get_state(),
                NumberToString(m_member[i].m_pUser->get_acc_id(), 0));  // T 0x0810904b
        }
        // ORIG 0x085b1b54 `call; test %eax,%eax; jmp`（4.1 空 if 体占位）。
        if (m_member[i].m_pUser->GetPCRoomNo())
        {
        }
    }

    // 开启反作弊采样
    for (int i = 0; i < 4; ++i)
    {
        // P292：ORIG 0x085b1b87 `xor $1; test; jne`（bool callee 否定早退）。
        if (!_checkValidUser(i))
            continue;
        memberUser(this, i)->getHackAnalyzer()->beginCollectHackInfo(
            m_member[i].m_pUser);
    }

    m_pad119 = 1;                                        // +0x119 装载检查标志
    bf(this).onStartMap();                               // T 0x0830dc74

    // 电梯副本收尾（BF+0x194==0x35 即 m_dungeonIndex==53 且 BF+0x13a 标志；
    // ORIG mov 0xcb8(%eax),%eax; cmp $0x35 / movzbl 0xc5e）
    if (bf(this).m_dungeonIndex == 0x35 && bf(this).m_elevatorFlag != 0)
    {
        bf(this).m_elevatorTimer = 0;                    // +0x139
        bf(this).m_elevatorState = 2;                    // +0x138
        bf(this).m_elevatorFlag = 0;                     // +0x13a
        // P24：ORIG 0x085b1c49/0x085b1c4e 把 gen_timer_key 结果留 %ebx、
        // m_partyIndex 留 %esi 跨 G_TimerQueue() call ⇒ 内联实参。
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, m_partyIndex,
                                    (TIMER_MESSAGE)0x28, 0xf,
                                    gen_timer_key((TIMER_MESSAGE)0x28), 0);
    }

    // 血票剩余时间提醒（BF+0x128=m_villageMonsterId 槽按 short 判非 0）
    if (bf(this).m_villageMonsterId != 0)
    {
        // P24：ORIG 0x085b1cb8/0x085b1cbd/0x085b1cc6 三值均跨 G_TimerQueue() 保活。
        G_TimerQueue()->InsertTimer(
            (TimerEntry::OBJ_TYPE)1, m_partyIndex, (TIMER_MESSAGE)0x27,
            bf(this).m_field134, gen_timer_key((TIMER_MESSAGE)0x27),
            (unsigned long long)(unsigned short)bf(this).m_villageMonsterId);
    }

    // 任务 NPC 副本且有 boss 房：EPLP 状态机切换 + 0xfe 包
    CDungeon* dungeon = bf(this).GetDungeon();           // ORIG 单次 call 存 -0x24
    if (dungeon != 0)
    {
        if (dungeon->getQuestNpcDungeon() && checkBossRoom())
        {
            SetEPLPState(2);
            // P24：ORIG 0x085b1d6e（key→%esi）/0x085b1d7b（index→%ebx）先于
            // G_TimerQueue() 求值。
            G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, GetPartyIndex(),
                                        (TIMER_MESSAGE)0x15, 600,
                                        gen_timer_key((TIMER_MESSAGE)0x15), 0);
            set_state(1);
            PacketGuard packet;
            packet.put_header(0, 0xfe);
            packet.finalize(true);
            send_to_party(packet);
        }
    }
}