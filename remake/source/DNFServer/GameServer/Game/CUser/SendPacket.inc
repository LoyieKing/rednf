bool CUser::SendPacket(eSendTarget target, PacketGuard& packet)  // b28d-lane5：CUser.h 已落 bool（Main），定义同步
{
    // b28d-PoolCUser 复测（2026-09-09，P251 尝试）：ORIG 0x0867b8fe 为 4.1 决策树
    //   「mov 0xc,%eax 单装载 + 寄存器复用 cmp$1 je / cmp$1 jg / test je / cmp$2 /
    //   cmp$3」；4.4 写 switch 恒产跳表/合并体（our 68→22 指令，ndiff 54→58 反劣）
    //   ⇒ 回退 if 链形，维持跨版本 switch lowering 封顶（P45/P46 族）。
    if (target == (eSendTarget)1)
        Send(packet);
    else if (target < (eSendTarget)2)
    {
        if (target == (eSendTarget)0)
            G_GameWorld()->send_all(packet, this);
    }
    else if (target == (eSendTarget)2)
    {
        CParty* pParty = (CParty*)GetParty();
        if (pParty == 0)
            Send(packet);
        else
            pParty->send_to_party(packet);
    }
    else if (target == (eSendTarget)3)
    {
        WarRoom* pRoom = (WarRoom*)GetWarRoom();
        if (pRoom == 0)
            Send(packet);
        else
            pRoom->SendToRoom(packet);
    }
    return 1;
