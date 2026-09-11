// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0821d7fa orig/ours=56/64 ndiff-blocks=13
//   帧差 sub $20 vs $34 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | sub    $0x20,%esp | jne    0821d828 <+0x2e>
int Dispatcher_RequestPCRoomPlayerList::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // [G1Csig] ORIG 0x0822xxxx 直证（两侧全序列对照）：
    //   ① 无 `cmpl $0x0,0xc(%ebp)` —— user==0 判空是我方发明防御（ORIG 直取 get_state）⇒ 删；
    //   ② 无 accountId 槽（ORIG `call get_acc_id; mov %eax,(%esp)` 直传 NumberToString
    //      第 1 参数、`mov $0x0,0x4(%esp)`）⇒ 去具名局部改内联；
    //   ③ trace 第 3 参数 = `user->get_state()` 直传（ORIG `call get_state; mov %eax,%esi`），
    //      非三元；④ ORIG 尾 `call LineFunc; jmp → mov $0x0,%eax`（P325 共享出口）
    //      ⇒ LineFunc 为语句，尾 `return 0`（我方原写 return LineFunc ⇒ 多 -0x9 gap）。
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        // ORIG 0x0821d850 段：NumberToString(%ebx) → get_state(%esi) 先求值保活，
        //   cMyTrace ctor 在其后才 call ⇒ 全表达式临时链，非两段式具名 trace。
        cMyTrace(__PRETTY_FUNCTION__, 0xd14e, 5)(
            "PCROOM : Dispatcher_RequestPCRoomPlayerList => User State : %d, m_id : %s",
            user->get_state(), NumberToString(user->get_acc_id(), 0));
        // [L7DispG4 2026-09-11] 语义轴修正（ORIG 直证 0x0821d8bc `call LineFunc` 后
        //   0x0821d8c1 `jmp 0x0821d8c8` 跳过 `mov $0x0,%eax`，且不重设 eax）
        //   ⇒ ORIG 语义 = `return LineFunc(...)`（LineFunc 返回值即本函数返回值）；
        //   前判「P325 共享出口 / 语句调用」与 ORIG 相悖（那形会产 `call; mov $0,%eax`）。
        return LineFunc(0xd14f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}