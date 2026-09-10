bool CGameManager::CheckOutQuickParty(CParty* party, bool flag)
{
    // ORIG 0x082983fc：then 分支 xor$1,%eax 直返（探针 f1 形同构）；
    // party==0 沿 eax 残值（flag spill）返回且无 movzbl（4.1 spill 策略，残差 N）。
    // b28d-solo-CGameManager 判词：N 封顶（ours 25 / orig 25，残差 1 行）——ORIG 入口
    // `mov 0x10(%ebp),%eax; mov %al,-0xc` 后 eax 恒持 flag，party==0 直落 epilogue
    // 复用 eax；4.4 恒补 `movzbl -0xc(%ebp),%eax`。ORIG `xor$1;test;jmp;nop` 的 test
    // 为 4.1 bool 归一 dead test（P225），4.4 不发。本波实测 `!call()` 与
    // `(bool)(call()^1)` 两形产物逐条相同（4.4.7），故保留 `!` 形。
    // b28d-laneCGM: P223 空 then 占位岛形——ORIG 0x0829840d `cmpl $0,0xc(%ebp); je
    //   +0x46(nop 岛)`；体尾 0x08298440 `jmp +0x47` 跨过 nop 落 epilogue。裸
    //   if/return 形被 4.4 jump-thread（无 jmp+nop，ndiff 2）。
    if (party == 0)
        goto L_quick_party_flag;
    return !G_CGameManager()->GetQuickPartySystemManager()->cancel_quick_party(party, flag, 0);
L_quick_party_flag:;
    return flag;
