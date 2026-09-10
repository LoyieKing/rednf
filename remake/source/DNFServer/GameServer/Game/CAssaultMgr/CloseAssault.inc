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
