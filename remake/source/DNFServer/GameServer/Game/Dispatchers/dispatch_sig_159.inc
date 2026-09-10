int Dispatcher_RequestPCRoomPlayerCount::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // [G1Csig] 同 dispatch_sig_152.inc 判据（ORIG 两侧全序列对照，同一 ORIG 模板）：
    //   ① 无 `cmpl $0x0,0xc(%ebp)` ⇒ 删 user==0 发明防御；② 无 accountId 槽 ⇒ 内联；
    //   ③ trace 第 3 参数直传 get_state()；④ LineFunc 为语句 + 尾 `return 0`（P325）。
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0xd186, 5);
        trace("PCROOM : Dispatcher_RequestPCRoomPlayerCount => User State : %d, m_id : %s",
              user->get_state(), NumberToString(user->get_acc_id(), 0));
        LineFunc(0xd187,
                 __PRETTY_FUNCTION__,
                 0, 0);
    }
    return 0;
