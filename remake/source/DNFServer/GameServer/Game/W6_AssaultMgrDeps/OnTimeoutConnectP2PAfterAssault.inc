int CAssaultMgr::OnTimeoutConnectP2PAfterAssault(unsigned int partyIndex, int timerKey)
{
    (void)timerKey;                                     // ORIG 第 2 参未消费
    try
    {
        CParty* party = G_CGameManager()->GetParty(static_cast<int>(partyIndex));
        // [b28d-solo] P214① 正极性短路：ORIG 短路 && 后 bool 物化
        // `mov $1,%eax/jmp; mov $0,%eax; test %al,%al; je`（0x082efa0b..0x082efa19）。
        if (party != 0 && party->TimeoutCompleteAfterAssault())
        {
            party->ClearBadP2PUser();
        }
    }
    catch (int)                             // ORIG landing pad `cmp $0x1,%edx; je`+
                                            // _Unwind_Resume 分支 = 定类型 catch(int)，
                                            // 非 catch(...)（其无 selector 匹配）。
    {
        // ORIG 仅 begin_catch → 存值到槽 → end_catch，不消费。
    }
    return 1;   // ORIG 所有路径 eax=1
