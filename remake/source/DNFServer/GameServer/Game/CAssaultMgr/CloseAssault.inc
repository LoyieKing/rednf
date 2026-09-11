// ORIG T 0x82e9ad6 size 0x52d：关场主流程 —— 全员按 PowerWar/普通分支
// 发关闭包并回补体力，广播状态与图标， ghost 记录，定时两队重载，
// 清场后归还池。
// b28d-solo-CAssaultMgr: N 封顶（§6.2 三件套，S174 复验）— P167 calls 60 vs 60
// **CLEAR**；gap -15→-3（372 vs 375）、ndiff 593→35。本轮修复：
// ①4 个 bool 局部（powerWarVill/gainChaosKill/powerWar/reloadParty）内联为
//   短路条件（ORIG 无 bool 槽、我方 cmpb×4 全消）；②ghost 分支改正极性
//   `if (IsCurCharacGhost())`（ORIG 082e9bd6 正极性，消 xor$1）；
// ③P191 双初始化：`int i=0` 函数头声明、两循环复用（对齐 ORIG 082e9ade 双
//   初始化 + 两处 `movl $0,-0x28`）；④now/chaos/level/sps 内联（ORIG 无槽往返，
//   P0）；⑤stamina 内联（ORIG `movzbl %al,%edx` 直传）。
// 残差仅 N18b 比较操作数序（ORIG `cmp %edx,%eax`）、N121 尾 jmp 岛 ×2、
// N104 EH 块相对序（ORIG cleanup 居中 vs 我方居末），无语义缺口。
// b28d-solo-CAssaultMgr（2026-09-09）: N 封顶（§6.2 三件套；清单 DIFF 235）。
// 本轮修正：`PacketGuard packet` 由函数级作用域改**独立块**——ORIG 082e9ec0
// 在 _SendPacketToArea 后立即 `jmp` 跳过 landing pad 直落 D1 ⇒ 原码是独立块
//（旧形析构落函数尾，多尾岛 ×2 + EH 块位置整体偏移；归一化 ratio 0.449→0.572）。
// 修正后 clean 口径 orig 374 / ours 379（+5 = N22 比较段 1 条 + N121 尾
// `jmp+nop` 岛 ×2）；p167 调用点差集 CLEAR（ORIG 60 = OUR 60）；帧 `sub $0x40`
// 双方一致。残差族：N22/N18b（`call GetTeam; mov this,%edx; mov 0x124(%edx),%edx;
// cmp %edx,%eax` vs 4.4 `mov %eax,%edx; ...; cmp %eax,%edx`，字段/返回异型旋钮
// 需改冻结头，S2 归 Main）+ N121（循环尾与 FreeAssaultPlace 后 `jmp+nop` 岛，
// 三形实测不消）+ N123/重定位（`mov 0x0,%eax` 系 .o 未解析 R_386_32，P264）。
// 无语义缺口。
// [round1-L29 2026-09-10 复核] difflib 唯一差异 = 比较两操作数装载序（ORIG 0x082e9ad6：
//   `mov 0x8(%ebp),%edx; mov 0x124(%edx),%edx; cmp %edx,%eax` = call 结果留 %eax；
//   我方 `mov %eax,%edx; mov this,%eax; mov 0x124(%eax),%eax; cmp %eax,%edx`）⇒ N22/N18b 封顶。
//   p167：calls 60=60；MISSING/EXTRA 仅 PacketGuard C1/C2·D1/D2 同址别名（P93 良性）。
// [round5-A29 破 N · 类型旋钮净账留痕，2026-09-10] 上述 N22/N18b 判词**已作废**：按
//   P254/P248档③ 对 `CAssaultPlace::m_state`(+0x10c) 与 `m_winTeam`(+0x124) 做 **enum 同型化**
//   （本 TU 侧先落 7 处显式 cast：`m_state = (ENUM_ASSAULT_STATE){0,6,7}` ×6 +
//   `m_winTeam = (ENUM_ASSAULT_TEAM)0`）⇒ 4.4 恢复 ORIG 的两段装载
//   `mov 0x8(%ebp),%edx; mov 0x124(%edx),%edx; cmp %edx,%eax`，**本件 DIFF→AE 归零**。
//   **净账（Main「类型旋钮先算两侧 identical 增量」红线）**：CAssaultMgr TU
//   `identical 109→109（不降） / ae 292→293（+1） / near 1→1 / diff 6→5（−1）` ⇒
//   **净 identical ≥ 0 且总非 identical 下降**，合规保留。字段侧为 enum（非返回类型）
//   ⇒ 定义侧 store 无归一化插入（`(ENUM)7` 直存），符合「两侧同核」要求。
void CAssaultPlace::CloseAssault()
{
    // b28d-solo: ORIG 0x082e9ade/0x082e9ae5 头部双初始化 `movl $0,-0x28;
    // movl $0,-0x24` + 两循环各再 `movl $0,-0x28` ⇒ P191 双初始化形：
    // i 在函数头声明并初始化，两处 for 复用（`for (i=0;…)`）。
    int i = 0;
    int lastVill = 0;
    // [SweepCAssaultMgr 真语义修正] ORIG 082e9af7 `test %eax,%eax; jle +0x396`
    // 的落点 = 082e9e6c（PacketGuard::C1）⇒ m_field100<=0 只跳过「for 循环 +
    // _SendUserState」，**仍执行** 图标广播块 / party 定时器块 / _DelUser 循环 /
    // FreeAssaultPlace。旧形 `if (m_field100 <= 0) return;` 会连归还池一起跳过
    // （漏 FreeAssaultPlace ⇒ 泄漏）。Ghidra C 同证 `if (0 < *(int*)(this+0x100)){…}`。
    if (0 < m_field100)
    {
    for (i = 0; i < 8; ++i)
    {
        if (m_assaulters[i].Empty())
            continue;
        CUser* slotUser = m_assaulters[i].GetUser();
        // [SweepCAssaultMgr 真语义修正] ORIG 082e9b36 `cmpl $0,-0x20(%ebp);
        // je 082e9c57`（目标 = SetAssaultPlace 段 0x82e9c57，**非**循环增量）
        // ⇒ slotUser==0 只跳过本处理块，仍执行 SetAssaultPlace/IsPowerWarWinner/
        // GetTeam 比较/ghostTime/_DelUser（Ghidra C 同证 `if (local_24 != 0) {…}`
        // 后继续）。旧形 `if (slotUser == 0) continue;` 的 je 落循环增量
        //（我方 0x3602 nop）——clean 口径把分支目标归一为 <T> 故不显形，属
        // jcc 落点差异（真语义反向）。
        if (slotUser != 0)
        {
        if (GlobalData::s_power_manager->IsPowerWarEventOn()
            && slotUser->getCurCharacVill() == 7)
        {
            if (!m_assaulters[i].IsPowerWarWinner())
                ForcedMoveAtPowerWarPlace(slotUser);
            _SendCloseAssault(slotUser,
                              (int)(slotUser->getPowerWarHP() & 0xffff));
        }
        else
        {
            if (slotUser->getCurCharacR() != 0)
            {
                // b28d-solo: ORIG 082e9bd6 正极性 `test %al; je <非ghost>`
                // ⇒ 源码为 `if (IsCurCharacGhost())`（ghost 分支直落）。
                if (slotUser->IsCurCharacGhost())
                {
                    // b28d-solo: ORIG 082e9bf9 `movzbl %al,%edx` 无 stamina 槽
                    //（P0 内联右值；具名 uchar 局部恒多 store+load 两条）。
                    slotUser->setCurCharacStamina(
                        (unsigned char)_SendCloseAssault(slotUser, 100));
                }
                else
                {
                    slotUser->setCurCharacStamina(
                        (unsigned char)_SendCloseAssault(
                            slotUser, m_assaulters[i].GetHp()));
                }
            }
        }
        lastVill = (int)slotUser->getCurCharacVill();
        }

        // b28d-solo-CAssaultMgr: ORIG 0x082e9c63 在 SetAssaultPlace 前**重取**
        // `m_assaulters[i].GetUser()`（P167 MISSING ×1；ORIG 调用点直读展开，
        // 非缓存 slotUser）。
        m_assaulters[i].GetUser()->SetAssaultPlace(0);
        if (m_assaulters[i].IsPowerWarWinner())
        {
            if (slotUser->getPowerWarHP() < 0x1e)
                slotUser->setPowerWarHP(0x1e);
        }

        // [SweepCAssaultMgr] 本比较段残差 = N7/N18a 寄存器角色互换：ORIG
        // 082e9cc2 `call GetTeam; mov 0x8(%ebp),%edx; mov 0x124(%edx),%edx;
        // cmp %edx,%eax` vs 4.4 `mov %eax,%edx; mov 0x8(%ebp),%eax;
        // mov 0x124(%eax),%eax; cmp %eax,%edx`。P226 语句表达式形实测反劣
        // （372→376：多 sete/test/je 物化）已回滚；W6 EvadeAssaultPlace 同残差
        // 亦封 N7（P321-T5）。
        if (m_assaulters[i].GetTeam() == m_winTeam
            && !GlobalData::s_power_manager->IsPowerWarEventOn()
            && slotUser->getCurCharacVill() != 7)
        {
            // b28d-solo: ORIG 082e9d07 无 now 槽（`call getCurSec` 结果直压实参）
            // ⇒ P0 内联右值。
            slotUser->SetCurCharacChaosKillTime(
                GlobalData::s_systemTime_.getCurSec());
        }

        int ghostTime = 0;
        if (GlobalData::s_power_manager->IsPowerWarEventOn()
            || slotUser->getCurCharacVill() == 7)
        {
            float penalty =
                GlobalData::s_power_manager->GetPowerWarResponPenalty(slotUser);
            ghostTime =
                GlobalData::s_power_manager->GetPowerWarGhostTime(slotUser,
                                                                 penalty);
        }
        else
        {
            // b28d-solo: ORIG 082e9d90 段把 chaos→%esi、level→%ebx 跨调用保活
            // 且无槽往返（P0/P52）⇒ 两实参内联，sps 亦内联为成员访问（+0x68）。
            ghostTime = G_CDataManager()->m_serverParameter.getAssaultGhostTime(
                slotUser->get_charac_level(),
                slotUser->GetCurCharacChaosPoint());
        }

        // b28d-solo: ORIG 082e9dcb `test %al,%al; je` 直落循环尾（bool 正极性，
        // 短路两守卫同落点）⇒ 去掉 `!= 0` 以免 4.4 生成块尾 jmp 中继。
        if (slotUser->IsCurCharacGhost() && ghostTime != 0)
        {
            int now = GlobalData::s_systemTime_.getCurSec();
            slotUser->SetCurCharacChaosDieTime(now);
            slotUser->SetCurCharacChaosResponTime(now + ghostTime);
            slotUser->m_characterView.enableSaveCharacView();
            CAssaultMgr::NotifyGhostTime(slotUser, ghostTime, ghostTime);
        }
    }
    _SendUserState();
    }

    // b28d-solo-CAssaultMgr: ORIG 082e9ec0 在 _SendPacketToArea 后**立即**析构
    // -0x34（jmp 跳过 landing pad 直落 D1）⇒ 原码是独立块，非函数级作用域
    //（旧形析构落函数尾，多 `jmp+nop` 岛 ×2 + landing 位置偏移）。
    {
        PacketGuard packet;
        _MakeViewAssaultIconPlayer(&packet, false);
        _SendPacketToArea(packet);
    }

    if (GlobalData::s_power_manager->IsPowerWarEventOn() && lastVill == 7)
    {
        // b28d-solo-CAssaultMgr: ORIG 0x082e9f01/0x082e9f4e 两处 `call
        // PartyInfo::Get` 后 `mov %eax,-0x10/-0xc; cmpl $0,-...`（具名 party
        // 局部），非 m_field00 直读（P119 恢复外联 Get 调用点）。
        CParty* party0 = (CParty*)m_parties[0].Get();
        if (party0 != 0)
        {
            TimerCompleteLoadAfterAssaultTimeout::registNextTimer(
                0x3c, party0->GetPartyIndex(),
                party0->gen_timer_key((TIMER_MESSAGE)0x2a));
        }
        CParty* party1 = (CParty*)m_parties[1].Get();
        if (party1 != 0)
        {
            TimerCompleteLoadAfterAssaultTimeout::registNextTimer(
                0x3c, party1->GetPartyIndex(),
                party1->gen_timer_key((TIMER_MESSAGE)0x2a));
        }
    }

    for (i = 0; i < 8; ++i)
    {
        if (m_assaulters[i].Empty())
            continue;
        _DelUser(i, true);
    }
    GetInstanceAssaultMgr()->FreeAssaultPlace(m_field104, this);
}