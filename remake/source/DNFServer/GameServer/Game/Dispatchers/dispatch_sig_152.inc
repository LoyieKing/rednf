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
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0xd14e, 5);
        trace("PCROOM : Dispatcher_RequestPCRoomPlayerList => User State : %d, m_id : %s",
              user->get_state(), NumberToString(user->get_acc_id(), 0));
        LineFunc(0xd14f,
                 __PRETTY_FUNCTION__,
                 0, 0);
    }
    return 0;
