// [L9-GrpE 2026-09-11 探针翻正·P305] ORIG 0x0867b902..0x0867b927 的
//   `mov 0xc(%ebp),%eax; cmp $1,%eax; je <1体>; cmp $1,%eax; jg <2/3>; test %eax,%eax;
//    je <0体>; jmp <默认>; cmp $2,%eax; je <2体>; cmp $3,%eax; je <3体>; jmp <默认>`
//   是 4.4 O0 对 `switch((int)x)`（case 0/1/2/3）的**决策树** lowering（非跳表）；
//   case 体物理序 = 书写序 0,1,2,3。
//   同口径探针 /tmp/L9/E/p_sp2.cpp（4.4.6 + -march=i486 -mtune=generic）：
//   FormB2=`switch((int)target)` 与 FormB3=`switch(int)` 均逐条复现上述链；
//   FormB 裸 `switch(target)`（窄枚举 eSendTarget 只定义 {0,1}）被 -fstrict-enums
//   把 case 2/3 静默删除 ⇒ **必须 `switch((int)target)`**（同 SendNotiPacket P305 修法）。
//   旧负结论「switch 恒产跳表 66→24」不再成立（该读数与本次探针不符，已作废）。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0867b8fe | _ZN5CUser10SendPacketENS_11eSendTargetER11PacketGuard
bool CUser::SendPacket(eSendTarget target, PacketGuard& packet)  // b28d-lane5：CUser.h 已落 bool（Main），定义同步
{
    switch ((int)target)
    {
    case 0:
        G_GameWorld()->send_all(packet, this);
        break;
    case 1:
        Send(packet);
        break;
    case 2:
    {
        CParty* pParty = (CParty*)GetParty();
        // 2026-09-11 N8CUser P214①/P222 正条件：ORIG 0x0867b9xx 决策树 case2 段
        //   `cmpl $0x0,-0x10(%ebp); je <this 分支>` ⇒ fall-through 是 pParty!=0 分支
        //   （receiver=槽 %ebp-0x10），我方旧形 `if (pParty==0) Send(...) else ...`
        //   产 `jne` + fall-through=this 分支 ⇒ 条件与块序双反。交换两分支体。
        if (pParty != 0)
            pParty->send_to_party(packet);
        else
            Send(packet);
    }
    break;
    case 3:
    {
        WarRoom* pRoom = (WarRoom*)GetWarRoom();
        // 同上（ORIG case3 段同形：je 跳 this 分支、pRoom!=0 块 fall-through）。
        if (pRoom != 0)
            pRoom->SendToRoom(packet);
        else
            Send(packet);
    }
    break;
    }
    return 1;
}