// [SweepCParty] 补真缺口：put_byte(0)+put_short(get_member_count())（第一分支）、
// send_all(packet, m_manager) 二参版、memset→char[0x40]={0}（rep stos）、cMyTrace 具名化
// （去 placement new）。p167 58=58 CLEAR（余 C1/C2、D1/D2 别名）。
void CParty::ReturnToVillage()
{
    // b28d-49 判词：N 封顶 + 缺口。P167 MISS put_byte/put_short/GameWorld::send_all(PacketGuard,CUser*)/
    // get_member_count 4 项 + EXTRA new/delete（P90）。@0x85aca60
    reinterpret_cast<CBattle_Field*>(&m_battleField)->setBloodState((ENUM_BLOOD_STATE)0);
    PacketGuard packet;
    packet.put_header(0, 2);
    // [G7 块序修正·ORIG 0x085acab4 直证] ORIG `test %al,%al; je 85acd53` ⇒ fall-through 为
    // `m_autoCreated != 0`（cMyTrace 路径），`== 0`（循环路径）落 je 目标；旧码臂序相反
    // ⇒ 4.4 多发 `xor $1` 且 A 块前移。按 ORIG 臂序交换两臂（语义不变）。
    if (m_autoCreated)
    {
        if (m_battleField.m_pDungeon != 0)
        {
            char names2[0x100] = {0};   // ORIG rep stos 0x40 dword = 0x100 字节（+0x32b）
            // 同首分支：cdecl 右→左求值 + 寄存器保活（P329/P52 单语句形）。
            m_manager->m_historyLog.LeaveDungeon(
                m_battleField.m_pDungeon->GetDungeonName(),
                reinterpret_cast<CBattle_Field*>(&m_battleField)->get_dungeon_diff(),
                _getMemberNames(names2),
                m_battleField.m_bloodState);
            // [P0/P329] ORIG 0x085acb2x：两 bool callee 各 `test %al,%al; je`（短路直跳，
            // 无 bool 槽）；receiver 实参 m_manager 经 %ebx 保活（无名局部）。
            if (IsSinglePlay() && m_manager->isDungeonClearHackCheck())
            {
                m_manager->getHackAnalyzer()->addServerHackCnt(
                    m_manager, (WongWork::ENUM_HACKTYPE)0x19c, 1, 0, 0);
            }
        }
        // [P0] ORIG 0x085acc3x：m_manager 经 %ebx 直传（无 mgr2 槽）。
        G_GameWorld()->out_from_dungeon(m_manager);
        packet.put_byte(0);
        packet.put_short(1);
        // [P227] ORIG 0x085acc2x：accIdStr/userState/partyIdx 三实参在 cMyTrace ctor 前
        // 右→左求值、结果留 %ebx/%esi/%edi 跨 ctor 保活（无具名槽；%edi 由 movswl 得）；
        // ctor 为全表达式临时链（无具名 trace 对象，P221 同族）。
        cMyTrace(__PRETTY_FUNCTION__, 0x2714, 0)(
            "<IN_DUNGEON_NULL_PARTY> m_id(%s), user_state(%d), PartyIndex(%d)",
            NumberToString(m_manager->get_acc_id(), 0), m_manager->get_state(),
            (int)m_manager->GetPartyIndex());
        m_manager->SetPartyIndex(-1);
        if (m_manager->getMoveSpace() != (UserSpace::T)1)
        {
            m_manager->set_state(ch_state3);
        }
        m_manager->make_basic_info((char*)&packet, 0);
        destroy();
        packet.finalize(true);
        // [P0] ORIG 0x085acfxx：getManager() 结果直传（无 gm 槽）。
        // [P222/P137] ORIG 0x085acd0a `cmp $0x1; setne %al; test; je <send_to_party>`
        // ⇒ 原码条件写成 != 1（真臂 send_all 直落、else 臂 send_to_party 居后）。
        if (getManager()->getMoveSpace() != (UserSpace::T)1)
        {
            G_GameWorld()->send_all(packet, m_manager);
        }
        else
        {
            send_to_party(packet);
        }
    }
    else
    {
        // ORIG 0x085acaad/0x085acac2：put_byte(0) + put_short(get_member_count())
        // （旧码漏此两句 —— 真缺口）。
        packet.put_byte(0);
        packet.put_short(get_member_count());
        // ORIG 0x085acacd `mov $0x40,%edx; rep stos` = 64 dword = **0x100 字节**
        // （P214③/P116：rep stos 立即数是 dword 次数不是字节长度）⇒ 缓冲 char[0x100]，
        // 且 -0x138+0x100 = -0x38 恰接 PacketGuard 槽。
        char names[0x100] = {0};
        // [R9a07 ④ 修复·ORIG 直证] ORIG 0x085acd96..0x085acda6：for 循环**外**独立调用一次
        // `_getMemberNames(buf)`（`lea -0x138(%ebp),%eax; mov %eax,0x4(%esp); mov 0x8(%ebp),%eax;
        // mov %eax,(%esp); call`，返回值丢弃；紧随 0x085acdab `movl $0x0,-0x1c(%ebp)` 循环变量
        // 初始化）；循环内 LeaveDungeon 第 3 实参为**直接取址** `lea -0x138(%ebp),%eax;
        // mov %eax,0xc(%esp)`（0x085ace2f）。旧码把该调用内联进循环内实参位 ⇒ 动态调用次数/
        // 时机与 ORIG 不同（两侧静态站点数相同，p167 调用差集对此类位置搬移是盲的）。
        _getMemberNames(names);
        for (int i = 0; i <= 3; ++i)
        {
            if (_checkValidUser(i))
            {
                if (m_battleField.m_pDungeon != 0)
                {
                    // [V331b10 核验修复 2026-09-12] 本处（else 分支循环内）ORIG 0x085ace0e..25
                    // 直证 receiver = `mov -0x1c(%ebp),%edx; mov 0x8(%ebp),%edi; …24*i…;
                    // lea (%edi,%eax,1); add $0x78; mov (%eax),%eax; lea 0x79700(%eax),%edx`
                    // = **m_member[i].m_pUser**（0x78+i*0x18），非 m_manager（this+0x74）。
                    // 旧注释/旧码把它与 m_autoCreated 分支（ORIG 0x085acb2b 确为 this+0x74）
                    // 混为一处，导致历史日志写到 manager。
                    // 三实参按 cdecl 右→左求值并用 %edi/%esi/%ebx 跨调用保活（P329/P52）。
                    m_member[i].m_pUser->m_historyLog.LeaveDungeon(
                        m_battleField.m_pDungeon->GetDungeonName(),
                        reinterpret_cast<CBattle_Field*>(&m_battleField)->get_dungeon_diff(),
                        names,
                        m_battleField.m_bloodState);
                }
                if (m_battleField.m_pDungeon != 0)
                {
                    // [P0/P329] 同首分支：短路直跳，无 bool/mgr 槽。
                    if (IsSinglePlay() && m_manager->isDungeonClearHackCheck())
                    {
                        m_manager->getHackAnalyzer()->addServerHackCnt(
                            m_manager, (WongWork::ENUM_HACKTYPE)0x19c, 1, 0, 0);
                    }
                }
                CUser* u = m_member[i].m_pUser;
                G_GameWorld()->out_from_dungeon(u);
                m_member[i].m_pUser->make_basic_info((char*)&packet, 0);
            }
        }
        set_state(1);
        battle_data_reset();
        packet.finalize(true);
        if (IsExistInvisible())
        {
            send_to_party(packet);
        }
        else
        {
            // [2026-09-11 父泳道 ORIG 直证 085acf84..93] 本点 ORIG 为**单参版**
            // `G_GameWorld(); lea -0x38(%ebp),%edx; mov %edx,0x4(%esp); mov %eax,(%esp);
            // call _ZN9GameWorld8send_allER11PacketGuard`（多传第 2 参为转写错误；
            // 二参版在 m_autoCreated 分支 0x85acd32，那处正确勿动）。
            G_GameWorld()->send_all(packet);
        }
    }
    SetEPLPState(0);
}