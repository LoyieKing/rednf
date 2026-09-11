// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08208a9e orig/ours=173/186 ndiff-blocks=35
//   帧差 sub $50 vs $88 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | push   %ebx | sub    $0x50,%esp
int Dispatcher_DeathTowerStageCommand::dispatch_sig(CUser* user, PacketBuf& packet)
{
    unsigned short local_22[4];          // -0x1e：4 个玩家用时（ushort）
    unsigned char local_19;              // -0x15：命令字节
    WongWork::CDeathTower* local_18;     // -0x14
    CParty* local_14;                    // -0x10
    int local_c;                         // -0xc：循环索引
    unsigned int uVar9;

    // [round5 A01] P0：ORIG 0x08208ab1 `cmp $0xa,%eax` 直比 call 结果（无槽）
    //   ⇒ 删具名 int iVar4（4.1 物化形 setne/test/je 由内联表达式直接复现）。
    if (user->get_state() != 10)
    {
        return 0;
    }
    // [A4 P0] ORIG 0x08208ad2 `call get_byte(unsigned char&)` → 0x08208ad7
    //   `xor $0x1,%eax; test %al,%al; je 08208b07`：返回值全程在 %al，全文无
    //   `mov %al,-0x32(%ebp)` 槽写入 ⇒ 原码无 cVar3 具名局部；我方 cVar3 槽为多余。
    if (!packet.get_byte(local_19))
    {
        return LineFunc(0xa3f2, __PRETTY_FUNCTION__, 0, 0);
    }
    local_18 = user->getDeathTower();
    if (local_18 != 0)
    {
        // ORIG 0x08208b37 `call handleStageCommand` → 0x08208b3c `xor $0x1,%eax; test %al,%al;
        //   je 08208b6c`（同形，无槽）。
        if (!local_18->handleStageCommand(user, local_19))
        {
            return LineFunc(0xa3f9, __PRETTY_FUNCTION__, 0, 0);
        }
        local_14 = (CParty*)user->GetParty();
        if (local_14 != 0 && local_19 == 2)
        {
            // [g01 2026-09-11 · P-g01-b] ORIG 0x08208b6c `call getCStage` → `mov %eax,(%esp)`
            //   直传 onFinishStage（无具名指针槽）；我方具名 pCVar6 多 1 槽 + 3 条
            //   （`mov %eax,-0x20; mov -0x20,%eax; mov %eax,(%esp)`）⇒ 内联链式。
            local_18->getCStage()->onFinishStage();
            for (local_c = 0; local_c <= 3; ++local_c)   // setle：<=3 继续
            {
                // ORIG 0x08208bc3 `call get_short(unsigned short&)` → 0x08208bc8
                //   `xor $0x1,%eax; test %al,%al; je 08208bf8`（无槽）。
                if (!packet.get_short(local_22[local_c]))
                {
                    return LineFunc(0xa405, __PRETTY_FUNCTION__, 0, 0);
                }
                // ORIG 0x08208c05 `call checkValidUser` → 0x08208c0a `test %al,%al;
                //   je 08208d20`（直接用返回值，无槽）。
                if (local_14->checkValidUser(local_c) != 0)
                {
                    // [g01 2026-09-11 · P0] ORIG 0x08208c1a..08208c3c 为**寄存器物化**
                    //   （`mov $0x1,%eax`/`mov $0x0,%eax` → `test %al,%al; je`），无 bVar2 槽
                    //   ⇒ 原码是单条三元表达式作 if 条件（我方 if/else 赋值 + if 产槽）。
                    if (local_22[local_c] == 0
                            ? ((char)local_14->IsSinglePlay() != 0)
                            : false)
                    {
                        // [g01 2026-09-11 · P0] ORIG 0x08208c3e..08208c93：`call get_user;
                        //   mov %eax,%ebx`（第 1 参保活，无槽）→ 第二次 `call get_user` →
                        //   `call getHackAnalyzer`（receiver 最后求值）⇒ 三个具名中转
                        //   （uVar5/pCVar7/pCVar8）均为多余槽（我方帧 +0x10 的来源之一）。
                        local_14->get_user(local_c)->getHackAnalyzer()->addServerHackCnt(
                            local_14->get_user(local_c),
                            (WongWork::ENUM_HACKTYPE)0xd5, 1, 0, 0);
                    }
                    // [G5DsigE] ORIG 0x08208cxx 链式：get_user→getDeathTower→add $0xb58 直传
                    //   （`mov %eax,(%esp); call`），MapClear 亦 `call get_user; add $0x79700,%eax`。
                    reinterpret_cast<DTChkView*>(
                        local_14->get_user(local_c)->getDeathTower())
                        ->m_killChecker.init();                        // +0xb58
                    // [g01 2026-09-11 · P-g01-b] 同上：`getCStage` 结果直传 getStageClearTime。
                    uVar9 = (unsigned int)local_18->getCStage()->getStageClearTime();
                    // [g01 2026-09-11 · P0] ORIG 0x08208ce9 `movzwl -0x1e(%ebp,%eax,2),%eax;
                    //   movzwl %ax,%ebx` 直传第 1 参（无 uVar1 具名槽）⇒ 内联表达式。
                    local_14->get_user(local_c)
                        ->m_historyLog.MapClear(local_22[local_c],
                                                (int)(uVar9 / 1000), 0);
                }
            }
        }
    }
    return 0;
}