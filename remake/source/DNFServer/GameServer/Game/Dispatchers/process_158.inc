// ORIG 0x081de1f8（报告 Dispatcher_Cancel_Quick_Party/process.md）：
// check_error 同型；cancel_quick_party(party, true, user) != 1 → param+4 = 1。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081de1f8 orig/ours=50/49 ndiff-blocks=12
//   帧差 sub $24 vs $38 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %ebx | sub    $0x24,%esp | mov    %eax,-0x14(%ebp)
int Dispatcher_Cancel_Quick_Party::process(CUser* user, MSG_BASE& msg,
                                           ParamBase& param)
{
    int err = check_error(user, msg, param);
    if (err > 0)
    {
        return err;
    }
    if (err < 0)
    {
        return -1;
    }
    // ORIG +0x42：msg 引用槽 -0x10（死引用，先声明占更深）、param 引用槽 -0xc
    MSG_BASE& mref = msg;
    (void)mref;
    stQuickPartyParam& pref = static_cast<stQuickPartyParam&>(param);  // 裸偏移→具名字段（同偏移同宽度）
    // [FL1 2026-09-10] P183 去缓存 + P290：ORIG 0x081de1b4 段 `mov %ebx,0x4(%esp)`
    //   （party 留 callee-saved %ebx 跨 receiver call，**无槽**），gm/mgr 亦无槽
    //   （链式内联）。旧形三个具名指针局部使 4.4 落 -0x18/-0x10 槽并逐点回读
    //   （帧 0x38 vs ORIG push %ebx + 0x24）⇒ 改链式内联。
    // [G4] callee 返回 bool（QuickParty.h:168 bool）⇒ 裸 `!call()` 直产 ORIG
    //   `xor $0x1,%eax; test %al,%al; je`（旧 X2 栈化形多 3 条）。
    if (!G_CGameManager()->GetQuickPartySystemManager()
             ->cancel_quick_party((CParty*)user->GetParty(), true, user))
    {
        pref.m_4 = 1;
    }
    return 0;
}