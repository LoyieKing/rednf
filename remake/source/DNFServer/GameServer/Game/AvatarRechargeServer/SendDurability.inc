void AvatarRechargeServer::SendDurability(CUser* user, char param2, short param3,
                                          char param4, int param5)
{
    // [b28d-solo-AvatarRechargeServer] N 封顶（P223 负例，本文档已列 SendDurability）：
    // ORIG 0x08190494 岛 A(`nop;jmp exit`)+0x08190497 岛 B(`nop`) 仅**前两级**早返有岛，
    // 第 3/4 级 `jne +0x82` 直落 epilogue；4.4 岛数恒=条件数(4) ⇒ 49 vs 45。
    // 已试三形均不闭合：否定早返 +2 jmp、正向嵌套 −2 jmp、P223 尾 goto 标签串 +2 jmp；
    // 合并第 3/4 条件只得 3 岛。P167 callsite 3=3 CLEAR ⇒ 语义等价，保持可读早返形。
    if (user == 0)
        return;
    if (user->get_state() != 5)
        return;
    if (param2 != 1)
        return;
    if (param4 != 3)
        return;
    // 目标：默认单人(1)，在队伍中改队伍(2)。
    CUser::eSendTarget target = (CUser::eSendTarget)1;
    if (user->GetParty() != 0)
        target = (CUser::eSendTarget)2;
    UpdateAvatarEndurance(target, user, (short)param5);
