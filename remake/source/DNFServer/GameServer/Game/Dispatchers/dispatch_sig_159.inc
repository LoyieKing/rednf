// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0821d8d0 orig/ours=56/64 ndiff-blocks=13
//   帧差 sub $20 vs $34 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | sub    $0x20,%esp | jne    0821d8fe <+0x2e>
int Dispatcher_RequestPCRoomPlayerCount::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // [G1Csig] 同 dispatch_sig_152.inc 判据（ORIG 两侧全序列对照，同一 ORIG 模板）：
    //   ① 无 `cmpl $0x0,0xc(%ebp)` ⇒ 删 user==0 发明防御；② 无 accountId 槽 ⇒ 内联；
    //   ③ trace 第 3 参数直传 get_state()；④ LineFunc 为语句 + 尾 `return 0`（P325）。
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        // 同 dispatch_sig_152.inc：ORIG ctor 在实参求值之后（P227 全表达式临时链）。
        cMyTrace(__PRETTY_FUNCTION__, 0xd186, 5)(
            "PCROOM : Dispatcher_RequestPCRoomPlayerCount => User State : %d, m_id : %s",
            user->get_state(), NumberToString(user->get_acc_id(), 0));
        return LineFunc(0xd187,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;

}