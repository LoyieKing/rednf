// b28d-solo-Dispatchers verdict=AE(routed) [P226/P0] ORIG 0x0812513c orig/ours=48/48
//   ORIG bool 物化于 %al（0x0812516x `test %al,%al`）无槽往返 ⇒ 条件表达式直用；
//   具名 bool 落 -0x9/帧漂移（探针 /tmp/b28d_disp/probe_bool.cpp fB 同形）。
int Dispatcher_ClearAction::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0812513c：state>2 且 curCharacR 非空才受理（否则 0x278）；
    // get_binary 0xb 字节 → ClearActionAndSendtoUser(group@0, value@4, packetClass 1)
    // ORIG 0x08125142/5156/5165：三条件全走直分支（cmpl je / cmp jle / test jne），
    // 汇合到 `mov $1,%eax`+`mov $0,%eax` 常量岛后 `test %al,%al`。栈 bool 会额外
    // 产 movb/cmpb 对 ⇒ 取反短路条件表达式直用（P226；禁 register 绑定，P0）。
    if (!(user != 0 && user->get_state() > 2 && user->getCurCharacR() != 0))
        return 0x278;
    _MSG_ACTION_POINT_ACTION_CLEAR msg;
    // ORIG 0x0812517e：msg 基址 -0x13（1 对齐槽位）。本类型含 int 成员，4.4 默认
    // 把对象摆到 4 对齐的 -0x14；先用 1B dead pad 试探推移（技巧31）。
    if (!packet.get_binary(reinterpret_cast<char*>(&msg), 0xb))
        return 0x27c;
    CUserProc::ClearActionAndSendtoUser(user, (_ActionGroupIndex)msg.m_group,
                                        msg.m_field4, (ENUM_PACKETCLASS)1);
    return 0;
}