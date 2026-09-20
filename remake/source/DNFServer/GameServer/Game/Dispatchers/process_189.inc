// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081d5492 orig/ours=59/59 ndiff-blocks=3
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movzwl -0x1d(%ebp),%eax | movzwl %ax,%esi | mov    &_ZN10GlobalData17s_guild_proxy_mgrE,%edx
int Dispatcher_GuildAttendanceInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_BASE* local_10;

    (void)param;
    // [b24ab] ORIG 入口 `mov 0x10(%ebp),-0xc`：缓存 msg（2nd 参，0x10）到 local_10，
    // 后续 local_10[0xd] 读 char（Ghidra 误标 param_3，实证为 msg）
    local_10 = &msg;
    if (user->get_state() <= 2)
        return 0;
    // [b24ab] P48：ORIG 直测寄存器（sete）不落槽 ⇒ guildkey 非具名局部，
    // 守卫与字段写各重取（get_charac_guildkey 两次实调用）。
    if (user->get_charac_guildkey() == 0)
        return 0;
    // ORIG ctor Packet_Guild_Attendance_Info(local_23) @0x0822c384（W，唯一导出符号）。
    Packet_Guild_Attendance_Info local_23;
    // [b24ab] ORIG 直写 +0xa/+0xe（-0x15/-0x11）：包字段用真实成员
    // m_fieldA/m_fieldB（此前裸偏移 +4/+8 偏移错误）。
    local_23.m_fieldA = (unsigned int)user->get_charac_guildkey();
    // [b24ab] P134：getCurCharacNo 直写 m_fieldB，无中间槽（ORIG 直 store -0x11）
    local_23.m_fieldB = (unsigned int)user->getCurCharacNo();
    // b28c-33：ORIG 无独立 char 局部（该 -0xd 槽即对象 +0x12=m_fieldC），且 msg+0xd
    // 为单条 disp 读（`movzbl 0xd(%eax),%eax`）⇒ 具名成员 + 具名视图，去掉 add 链。
    local_23.m_fieldC = static_cast<unsigned char>(
        reinterpret_cast<MSG_GUILD_ATTENDANCE*>(local_10)->m_field0d);
    // b28d-solo-Dispatchers: P200/P52 —— ORIG 0x081d550b `movzwl -0x1d(%ebp),%eax;
    //   movzwl %ax,%esi` 后跨 GetServerGroup/GetServerProxy 保活，源码为**短整表达式
    //   直传**（无具名局部、无 register 绑定）：len 与 &local_23 由链式表达式实参物化。
    GlobalData::s_guild_proxy_mgr
        ->GetServerProxy((ENUM_SERVER_GROUP)user->GetServerGroup())
        ->SendTcpPacket(reinterpret_cast<char*>(&local_23), local_23.packetSize);
    return 0;
}