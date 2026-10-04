// [b28d-lane2] verdict=DIFF ndiff=11（ORIG 0x084d347e）：块序重排(N104/P223)+混合残差；gap0。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=6（ORIG 0x84d347e）：混合残差（寄存器分配/槽序：N7/N18a/N82/P112）。
int Inter_DBCheckDoubleGuildName::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // [G2A-PC 本轮负结论 N，ndiff=6] 差异为同一单参调用块
    //   `lea <S>,%eax; mov %eax,(%esp); call <T>`（PacketGuard 构造/析构类）在两侧的
    //   整体位移：ORIG 在 [55:58]、本方在 [65:68]，两条块净指令数相同 ⇒ 块序/布局抖动。
    //   源码语句序与报告 084d34e8（ctor 紧跟 setGuildCreateFlag(2)）一致，无直证可改，留 N。
    if (user->get_state() <= 2)
        return 0;
    // ORIG：包基址槽在守卫之后物化。
    Inter_DBCheckDoubleGuildNamePktView* pPkt = reinterpret_cast<Inter_DBCheckDoubleGuildNamePktView*>(data);

    // ORIG 0x084d3546：err 支尾无 `mov $0x0,%eax`（直接 jmp 汇合尾 0x084d3581）
    // ⇒ err 支不带 return；正常支 dtor 内联块尾（0x084d355f lea/mov/call D1Ev，
    // 0x084d3564 jmp 汇合）⇒ PacketGuard 直接声明于 else 块（无额外 {}，
    // 额外作用域会使 gcc 把 dtor hoist 到汇合点共享尾块，产生行序差）。
    // [Q5-InterDisp 2026-09-11 负结论·已回滚] 试「if/else 两支各写 `return 0;`」想让 dtor
    //   留在 else 支尾：实测反劣 —— gcc 改用 %ebx 汇聚返回常量（3 处 `mov $0x0,%ebx`
    //   + 尾 `mov %ebx,%eax`），nd 6→10、gap -1→-2。ORIG 0x084d349b/0x084d34d0/0x084d3564
    //   三处出口都是 `mov $0x0,%eax`（0x084d349b 直接 jmp 0x084d3586 跳过尾块；后两处
    //   jmp 0x084d3581 共享单条 `mov $0x0,%eax`），全程无 %ebx ⇒ 原码是**单一尾部 return**。
    //   dtor 块物理位置（else 支内联 vs 汇合尾块）属 4.4 块布局，源码不可控 ⇒ 判 N。
    if (pPkt->m_result == '\x02')
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x9f, 0x6b);
    }
    else
    {
        user->setGuildCreateFlag(2);
        PacketGuard packet;
        packet.put_header(1, 0x9f);
        packet.put_byte(1);
        packet.finalize(true);
        user->Send(packet);
        user->setGuildCreateFlag(GUILD_ADDRESS_DOUBLE_CHECK);
    }
    return 0;
}