// b28d-solo-Dispatchers verdict=DIFF(routed) [N89] ORIG 0x0820bbde orig/ours=234/228 ndiff-blocks=39
//   ORIG 段内 std::/__gnu_cxx out-of-line 内联实例 call 9 个，4.4 libstdc++ 全内联不可复现
//   首差: sub    $0x190,%esp | movb   $0x1,-0x1a(%ebp) | movb   $0x1,-0x19(%ebp)
int Dispatcher_New_Gmdebug_Command::dispatch_sig(CUser* user, PacketBuf& packet)
{
    char local_1a = 1;                   // -0x1a（后被 string 复用，语义上为临时）
    char local_19 = 1;                   // -0x19：GM 通过标志
    char* local_1c;                      // -0x18：get_packet(0) 结果
    int local_18;                        // -0x14：*(int*)(pkt+0xd)
    int local_14;                        // -0x10：vector size（填充前）
    game_master::CCommand* local_c;      // -0xc：GetCommand 结果
    // [Q4DispatchBig 2026-09-11 负结论] 试删未用 `char cVar1;`：四数与本件
    //   (233/231, gap +2) **均不变** ⇒ 4.4 不给未使用自动变量分配槽，删之无收益 ⇒ 还原。
    char cVar1;

    local_1c = packet.get_packet(0);
    if (user->getCurCharacR() == 0)
    {
        return LineFunc(0xa9bc, __PRETTY_FUNCTION__, 0, 0);
    }
    // [G2-114 2026-09-11 · P329/P0] ORIG 0x0820bbde：`mov <A>,%edx; mov %eax,0x4(%esp);
    //   mov %edx,(%esp); call isGM; xor $0x1,%eax; test %al,%al; je` ⇒ accId 经 %eax 直传
    //   （无 uVar4 槽）、isGM 结果寄存器直测（无 cVar1 槽）。
    if (!GlobalData::s_pGMAccounts_->isGM(user->get_acc_id()))
    {
        local_19 = 0;
    }
    // [Q4DispatchBig 2026-09-11 负结论] ORIG 该点 `movzbl -0x19(%ebp),%eax;
    //   xor $0x1,%eax; test %al,%al; je`（4 条，4.1 char 提升链）。本波实测两形均劣：
    //   ① `if (local_19 == 0)` ⇒ `cmpb $0x0; jne`（形态仍不符，gap +2 不变）；
    //   ② `if (!local_19)` ⇒ 我方 our 231→**230**（gap +2→**+3** 劣化）且槽漂移
    //   -0x1a/-0x19 → -0x1e/-0x1d。⇒ 回滚 `!= 1`（N 档：4.4 内存直比不可造 4.1 提升形）。
    if (local_19 != 1)
    {
        return 0;
    }
    cMyTrace local_34(__PRETTY_FUNCTION__, 0xa9c4, 0);
    local_34("GM Pass Complete!");
    std::vector<std::string> local_40;
    local_18 = *(int*)(local_1c + 0xd);
    *(unsigned char*)(local_1c + local_18 + 0x11) = 0;
    user->getSeparatedTextUnits(local_40, local_1c + 0xd, " ", 0);
    // [G1-Dispatchers] ORIG 调用序列为 size()(填充前记录) → push_back 体 → size()(循环尾条件)
    // 共 2 个 size 调用点（p167 ORIG-only `_ZNKSt6vectorISsSaISsEE4sizeEv` ×1）；
    // 4.1 的 while 条件落在体之后 ⇒ 前一次即此处的具名记录（local_14 原本只声明未用）。
    local_14 = (int)local_40.size();
    while (local_40.size() < 10)
    {
        local_40.push_back("");
    }
    PacketGuard local_4c;
    DisPatcher_DebugCommand local_50;
    if (user->isGMUser())
    {
        game_master::CParser local_178;
        local_178.m_user = user;
        local_178.m_extra = local_40;
        local_c = GlobalData::s_GameMasterMgr->GetCommand(local_40[0]);
        if (local_c == 0)
        {
            oldGmRoutine(user, packet, local_40);
        }
        else
        {
            local_c->SetParser(&local_178);
            GlobalData::s_GameMasterMgr->HandleRequest(local_c);
        }
    }
    return 0;
}