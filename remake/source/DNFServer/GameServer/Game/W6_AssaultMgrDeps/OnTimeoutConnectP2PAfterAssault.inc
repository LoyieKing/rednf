// ===========================================================================
// OnTimeoutConnectP2PAfterAssault —— ORIG T 0x082ef9d8 (0x79)
// [b28d-solo-W6_AssaultMgrDeps] AE（P214① 正极性短路 &&：4.4 直产 ORIG `je;je;mov $1,%eax;jmp;
// mov $0,%eax;test;je` 物化岛，40=40；旧「bool ABI 跨版本」判词作废）；ORIG 0x082ef9d8。
// ===========================================================================
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
    catch (ENUM_ERROR)                      // [R18exc 2026-09-13] ORIG landing pad
                                            // `cmp $0x1,%edx; je` + _Unwind_Resume 分支；
                                            // LSDA 类型表 = _ZTI10ENUM_ERROR（非 catch-all）。
    {
        // ORIG 仅 begin_catch → 存值到槽 → end_catch，不消费。
    }
    return 1;   // ORIG 所有路径 eax=1
}