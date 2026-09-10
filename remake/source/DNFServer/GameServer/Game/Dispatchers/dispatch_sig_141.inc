int Dispatcher_DeathTowerStageCommand::dispatch_sig(CUser* user, PacketBuf& packet)
{
    unsigned short local_22[4];          // -0x1e：4 个玩家用时（ushort）
    unsigned char local_19;              // -0x15：命令字节
    WongWork::CDeathTower* local_18;     // -0x14
    CParty* local_14;                    // -0x10
    int local_c;                         // -0xc：循环索引
    char cVar3;
    bool bVar2;
    unsigned int uVar9;

    // [round5 A01] P0：ORIG 0x08208ab1 `cmp $0xa,%eax` 直比 call 结果（无槽）
    //   ⇒ 删具名 int iVar4（4.1 物化形 setne/test/je 由内联表达式直接复现）。
    if (user->get_state() != 10)
    {
        return 0;
    }
    cVar3 = packet.get_byte(local_19);
    if ((char)(cVar3 ^ 1) != 0)
    {
        return LineFunc(0xa3f2, __PRETTY_FUNCTION__, 0, 0);
    }
    local_18 = user->getDeathTower();
    if (local_18 != 0)
    {
        cVar3 = (char)local_18->handleStageCommand(user, local_19);
        if (cVar3 != 1)
        {
            return LineFunc(0xa3f9, __PRETTY_FUNCTION__, 0, 0);
        }
        local_14 = (CParty*)user->GetParty();
        if (local_14 != 0 && local_19 == 2)
        {
            WongWork::CDeathTower::CStage* pCVar6 = local_18->getCStage();
            pCVar6->onFinishStage();
            for (local_c = 0; local_c <= 3; ++local_c)   // setle：<=3 继续
            {
                cVar3 = packet.get_short(local_22[local_c]);
                if ((char)(cVar3 ^ 1) != 0)
                {
                    return LineFunc(0xa405, __PRETTY_FUNCTION__, 0, 0);
                }
                cVar3 = (char)local_14->checkValidUser(local_c);
                if (cVar3 != 0)
                {
                    if (local_22[local_c] == 0)
                    {
                        cVar3 = (char)local_14->IsSinglePlay();
                        bVar2 = (cVar3 != 0);
                    }
                    else
                    {
                        bVar2 = false;
                    }
                    if (bVar2)
                    {
                        CUser* uVar5 = local_14->get_user(local_c);
                        CUser* pCVar7 = local_14->get_user(local_c);
                        WongWork::CHackAnalyzer* pCVar8 = pCVar7->getHackAnalyzer();
                        pCVar8->addServerHackCnt(uVar5, (WongWork::ENUM_HACKTYPE)0xd5, 1, 0, 0);
                    }
                    CUser* pCVar7b = local_14->get_user(local_c);
                    WongWork::CDeathTower* iVar4b = pCVar7b->getDeathTower();
                    reinterpret_cast<DTChkView*>(iVar4b)->m_killChecker.init();   // +0xb58
                    WongWork::CDeathTower::CStage* pCVar6b = local_18->getCStage();
                    uVar9 = (unsigned int)pCVar6b->getStageClearTime();
                    unsigned short uVar1 = local_22[local_c];
                    CUser* iVar4c = local_14->get_user(local_c);
                    iVar4c->m_historyLog.MapClear(uVar1, (int)(uVar9 / 1000), 0);
                }
            }
        }
    }
    return 0;
