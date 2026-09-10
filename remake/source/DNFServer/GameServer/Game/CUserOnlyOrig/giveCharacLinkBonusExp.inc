void CUser::giveCharacLinkBonusExp(unsigned int bonusExp)
{
    PacketGuard packet;

    // [b28d-solo-CUserOnlyOrig P28] ORIG sfp 占深槽 -0x2c、sp 占浅槽 -0x28（gain_exp_sp
    // 实参按右到左先算 sfp）⇒ 4.4「先声明占浅槽」，故 sp 先声明、sfp 后声明。
    int sp = 0;                                        // local_30[1]（spRef）
    int sfp = 0;                                       // local_30[0]（sfpRef）
    // [b28d-solo-CUserOnlyOrig P0 试形] ORIG 无 skillTreeIdx/skillR 槽：`call
    // GetCurCharacSkillTreeIndex; mov %eax,%ebx; call getCurCharacSkillR; mov %ebx,0x4(%esp)`
    // ⇒ 原码是内联表达式（实参先求值、receiver 后求值），具名局部恒占槽并令全帧深 0xc。
    const int spBefore = ((SkillSlot*)getCurCharacSkillR())->get_remain_sp_at_index(
        (ENUM_SKILL_TREE_KIND)GetCurCharacSkillTreeIndex());
    calcHelpAbuseRatio(bonusExp, 0);                   // SUB41(param_1,0) → bool false
    // [b28d-solo-CUserOnlyOrig P28] ORIG spAfter 槽 -0x14 深于 gained(-0xd)/linkSlot(-0xc)
    // ⇒ 声明序在 gained 之前（赋值仍在 gain_exp_sp 之后）。
    int spAfter;
    const char gained = gain_exp_sp(
        (int)bonusExp, sp, sfp, (CUser::eExpAddReason)3, 0, true);

    spAfter = ((SkillSlot*)getCurCharacSkillR())->get_remain_sp_at_index(   // P48 二次独立调用
        (ENUM_SKILL_TREE_KIND)GetCurCharacSkillTreeIndex());

    if (gained != '\0')
    {
        packet.clear();                                // InterfacePacketBuf::clear
        packet.put_header(0, 2);
        packet.put_byte(0);
        packet.put_short(1);
        make_basic_info((char*)&packet, '\0');
        packet.finalize(true);
        G_GameWorld()->send_all(packet, this);         // send_all 带 excepted user
    }

    // [b28d-solo-CUserOnlyOrig P214③/P312 试形] ORIG 内联 rep stos（无 memset call）；
    // 显式 memset 在 4.4 恒 call，改 `={0}` 触发编译器 rep stos 展开。
    char linkName[0x1e] = {0};                         // local_4e
    const int linkSlot = (int)getLinkCharacSlotIndex();
    if (linkSlot >= 0 &&
        (int)m_characList.size() > linkSlot)
    {
        strcpy(linkName, (const char*)(reinterpret_cast<char*>(&m_characList[linkSlot]) + 4));
    }

    packet.clear();
    packet.put_header(0, 0x123);
    packet.put_int((int)strlen(linkName));
    packet.put_str(linkName, (int)strlen(linkName));
    packet.put_int(0);
    packet.put_int((int)bonusExp);
    // [b28d-solo-CUserOnlyOrig P200/P77 试形] ORIG `mov -0x14,%edx; mov %edx,%ecx;
    // sub %eax,%ecx` 为 32 位减法（无 cwtl/16 位 sub）⇒ 原码直接传 int 表达式
    // （put_short(int) 内部截断），显式 `(short)` cast 反产 `sub %ax,%cx; cwtl`。
    packet.put_short(spAfter - spBefore);
    packet.put_short(sfp);
    packet.finalize(true);
    Send(packet);

    SendNotiPacket((eSendTarget)1, (ENUM_NOTIPACKET)2, 1);
    send_skill_info();
