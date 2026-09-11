// ORIG 0x0860a9d4 _ZN26CSpecialItemRoutingManager12StartRoutingEP5CUserR10Inven_Itemj
// get_state∉{5,0xc}→1；GetParty()==0→1；find_item(item idx)==0→1；
// IsRoutingState()→0x12；否则置路由状态/物品、insertTimer(0x23s)、
// put_header(0,0x119)+put_int(item idx)+finalize(true) 后 send_to_party，返回 0。
int CSpecialItemRoutingManager::StartRouting(CUser* user, Inven_Item& item,
                                             unsigned int uniqueId)
{
    // [round3-L30 修] ORIG 0x0860a9d4：`call get_state; cmp $0x5,%eax; je; cmp $0xc,%eax;
    //   je`（**一次** call 结果留 %eax 复用、无栈槽）⇒ 按 P261/P279「无状态槽 + cmp $K,%eax
    //   链 ⇒ 源形是 switch(call())」改写；旧形 `int state = ...; if (state != 5 && state != 0xc)`
    //   引入 ORIG 无的具名槽且第二次比较退化为槽直比。
    switch (user->get_state())
    {
    case 5:
    case 0xc:
        break;
    default:
        return 1;
    }

    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
        return 1;

    // ORIG 0x0860aa1d-0x0860aa36：`mov 0x10(%ebp),%eax; mov 0x2(%eax),%eax; mov %eax,%ebx;
    //   call G_CDataManager; mov %ebx,0x4(%esp); mov %eax,(%esp); call find_item;
    //   mov %eax,-0xc(%ebp); cmpl $0x0,-0xc(%ebp); jne` ⇒ 源码为链式
    //   `G_CDataManager()->find_item(item.m_addInfo)`（实参先求值、this 后求值），
    //   结果落具名 CItem* 槽 -0xc。
    CItem* pItem = G_CDataManager()->find_item(item.m_addInfo);
    if (pItem == 0)
        return 1;

    if (party->IsRoutingState())
        return 0x12;

    party->SetStartRouting();
    party->SetRoutingItem(item, uniqueId);
    insertTimer(party);

    PacketGuard packet;
    packet.put_header(0, 0x119);
    packet.put_int(item.m_addInfo);
    packet.finalize(true);
    party->send_to_party(packet);
    return 0;
}