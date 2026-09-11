// ============================================================================
// SendDurability —— ORIG T 0x08190416 _ZN...14SendDurabilityEP5CUsercsci
// ============================================================================
void AvatarRechargeServer::SendDurability(CUser* user, char param2, short param3,
// [P8S4 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
                                          char param4, int param5)
{
    // [b28d-L5a 2026-09-11 落地] 旧「P223 负例 N 封顶（49 vs 45）」判词**作废**。
    // ORIG 0x08190494 岛 A(`nop;jmp exit`)+0x08190497 岛 B(`nop`) 仅前两级早返有岛、
    // 第 3/4 级 `jne exit` 直落 epilogue ⇒ 唯一复现形 = **混合形**：
    //   前两级「负向 goto 共享尾标签」(`if(user==0) goto end; if(state!=5) goto end;`)
    //   + 第 3/4 级「正向嵌套」(`if(param2==1){ if(param4==3){ 主流程 } }`，失败直落函数尾)。
    // 逐条同构（`je 岛A` / `jne 岛B` / 两条 `jne exit` / 主流程尾 `jmp exit`），
    // 45 = 45，TU 读数 ae +1 / diff −1；P167 callsite 3=3 CLEAR。
    if (user == 0)
        goto end;
    if (user->get_state() != 5)
        goto end;
    if (param2 == 1)
    {
        if (param4 == 3)
        {
            // 目标：默认单人(1)，在队伍中改队伍(2)。
            CUser::eSendTarget target = (CUser::eSendTarget)1;
            if (user->GetParty() != 0)
                target = (CUser::eSendTarget)2;
            UpdateAvatarEndurance(target, user, (short)param5);
        }
    }
end:
    ;
}