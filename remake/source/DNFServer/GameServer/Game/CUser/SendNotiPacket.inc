// 0x0867ba5c（SendNotiPacket.md）：header(0,cmd)；cmd==2 → byte+short(1)+
// make_basic_info；cmd==0x25 → level/exp/组队/会员 exp/SP/SFP/胜点(+0x58)/
// 疲劳/seria bonus/周末(事件 0x57 活跃才发并 reset)/premium bonus(reset)。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867ba5c | _ZN5CUser14SendNotiPacketENS_11eSendTargetE15ENUM_NOTIPACKETi
bool CUser::SendNotiPacket(eSendTarget target, ENUM_NOTIPACKET cmd, int param)
{
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, cmd);
    // 2026-09-11 CuB（P261 if 链→switch + P324 case 块物理序）：ORIG 0x0867ba89
    //   决策树 = `mov 0x10(%ebp),%eax; cmp $0x2,%eax; je <2 体@+0x234>;
    //   cmp $0x25,%eax; jne <log@+0x27d>`（单装载寄存器比较链），且 0x25 体在 +0x42、
    //   2 体在 +0x234（物理序 = 书写序）⇒ 原码是 switch，**case 0x25 写在 case 2 之前**；
    //   if/else 链（旧形）四四产内存直折 cmp + 2 体直落（我方旧读数）。
    //   4.4 形验证（探针 /tmp/CuB/probe/p1.cpp g2 vs g3，c6root 4.4.7 -m32 -O0）：
    //   switch 同序 ⇒ `mov 12(%ebp),%eax; cmpl $2,%eax; je .L8; cmpl $37,%eax; jne .L15`
    //   + 0x25 体在前 / 2 体在后，与 ORIG 逐形一致。
    // [Main-K4C 2026-09-11 · P305 修正] **必须 `switch((int)cmd)`**：`ENUM_NOTIPACKET`
    //   全仓只定义 `ENUM_NOTIPACKET_0 = 0`（CUser.h:354），是窄枚举 ⇒ `-fstrict-enums`
    //   判定值域 {0}，把 `case (ENUM_NOTIPACKET)0x25/2` **静默整块删除**（编译零告警）
    //   —— 实测裸 `switch(cmd)` 使本函数 212→50 条（两个 case 体全丢，真语义损坏）。
    //   改 `switch((int)cmd)` + int 常量 case 即保留寄存器比较链形且不被删（S18 修法）。
    switch ((int)cmd)
    {
    case 0x25:
    {
        ((InterfacePacketBuf*)&packet)->put_byte(get_charac_level());
        ((InterfacePacketBuf*)&packet)->put_int(get_charac_exp());
        ((InterfacePacketBuf*)&packet)->put_int(get_charac_party_bonus_exp());
        ((InterfacePacketBuf*)&packet)->put_int(get_charac_member_bonus_exp());
        ((InterfacePacketBuf*)&packet)->put_short(((SkillSlot*)getCurCharacSkillR())->get_remain_sp_at_index(
            (ENUM_SKILL_TREE_KIND)0));
        ((InterfacePacketBuf*)&packet)->put_short(((SkillSlot*)getCurCharacSkillR())->get_remain_sp_at_index(
            (ENUM_SKILL_TREE_KIND)1));
        ((InterfacePacketBuf*)&packet)->put_short(((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(
            (ENUM_SKILL_TREE_KIND)2));
        ((InterfacePacketBuf*)&packet)->put_short(((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(
            (ENUM_SKILL_TREE_KIND)3));
        ((InterfacePacketBuf*)&packet)->put_int(getPVPResultR()->m_winPoint);
        ((InterfacePacketBuf*)&packet)->put_int(get_charac_fatigue_buf_bonus_exp());
        ((InterfacePacketBuf*)&packet)->put_int(get_charac_seria_buf_bonus_exp());
        // ORIG 0x0867bc0f..0x0867bc2a：`call GetRepeatEvent` 后直接
        // `mov (%eax),%edx; add $0x34,%edx; mov (%edx),%edx; …; call *%edx;
        // test %al,%al; je else` —— 无具名指针槽、无判空、无 bool 物化
        // （P171 照抄不写 + 发明防御删除；同 add_guild_pvp_result 的 0x21 处置）。
        if (GlobalData::s_event_manager->GetRepeatEvent(0x57)
                ->IsEventing((CUser*)0))
        {
            ((InterfacePacketBuf*)&packet)->put_int(getWeekendBonusExp());
            resetWeekendBonusExp();
        }
        else
        {
            ((InterfacePacketBuf*)&packet)->put_int(0);
        }
        ((InterfacePacketBuf*)&packet)->put_int(getPremiumBonusExp());
        resetPremiumBonusExp();
        break;
    }
    case 2:
    {
        // [L9-GrpE 2026-09-11 P0 具名槽恢复] ORIG 0x0867bc90 `mov 0x14(%ebp),%eax;
        //   mov %eax,-0xc(%ebp)` 后 put_byte（0x0867bc96 从 -0xc 读回）、put_short、
        //   make_basic_info（0x0867bcbb `mov -0xc(%ebp),%eax; movsbl %al,%edx`）三处
        //   均从具名槽 -0xc 读 ⇒ 原码在本分支把 param 拷入具名局部（Ghidra C：
        //   `local_10 = param_4;`）。重建直读 `param` 形缺该槽。
        int p = param;
        ((InterfacePacketBuf*)&packet)->put_byte(p);
        ((InterfacePacketBuf*)&packet)->put_short(1);
        make_basic_info((char*)&packet, (char)p);
        break;
    }
    default:
    {
        LogManager::logFormat(1, "user.cpp",
            __PRETTY_FUNCTION__,
            0x76d1, "User %s - NotifyID = %d", getCurCharacName(), cmd);
        return false;   // b28c-L5 SIGFIX：ORIG 串 bool；失败臂 `mov $0x0,%ebx` 汇合尾
    }
    }
    ((InterfacePacketBuf*)&packet)->finalize(true);
    SendPacket(target, packet);
    return true;        // ORIG 0x0867bd4e `call SendPacket; mov $0x1,%ebx; …mov %ebx,%eax`
}