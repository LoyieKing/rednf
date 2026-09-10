void CAssaultPlace::_MakeUserState(PacketGuard* p, CUser* user)
{
    // b28d-solo-CAssaultMgr: N 封顶（N7 寄存器角色互换；清单 DIFF 11 复验）
    // ORIG 0x82e8d60+0x30：`call GetUserState; mov %eax,%edx; mov p,%eax;
    // mov %edx,0x4(%esp); mov %eax,(%esp)`（值先中转 %edx、this 后载 %eax）；
    // 4.4 恒 `mov p,%edx; mov %eax,0x4(%esp); mov %edx,(%esp)`（this 先载、
    // 值留 %eax）——助记符序列全等，仅 %eax/%edx 角色互换（34 vs 33）。
    // 本轮补测 /tmp/probe_mus.cpp + probe_mus2.cpp 17 形（uint/long/enum/
    // 短整 cast、语句表达式、*&、receiver cast、具名 receiver、前置掩码上下文）
    // 4.4 全产 this-first；callee 返回类型旋钮不可用（CUser.h 冻结头）。
    // ORIG 同 TU SendUserState 同表达式无 hop（ORIG 自身分裂两形）。
    p->put_header(0, 3);
    p->put_byte(1);
    p->put_short(user->get_unique_id() & 0xffff);
    // 同判词（N7）：单条 %eax↔%edx 角色互换，语义等价；绑定形 P0 禁用。
    p->put_byte(user->GetUserState());
    p->finalize(true);
