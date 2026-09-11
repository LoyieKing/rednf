// ORIG 0x081cba34 T _ZN27Dispatcher_MemberEnterReply7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/Dispatcher_MemberEnterReply/process.md 汇编+反编译 C 全文转写）
// param 为模板实参，ORIG 裸访问 +0x04 错误码槽（m_4）。
// local_27 = Packet_Monitor_Member_Enter_Reply（ORIG 0x13 字节，shared/packet 权威布局）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081cba34 orig/ours=71/69 ndiff-blocks=14
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0x10(%ebp) | mov    %eax,%edx | mov    -0x10(%ebp),%eax
int Dispatcher_MemberEnterReply::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stMemberEnterReplyParam& out = static_cast<stMemberEnterReplyParam&>(param);
    out.m_4 = check_error(user, msg);
    if (out.m_4 >= 1)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x21ef,
                        __PRETTY_FUNCTION__,
                        out.m_4, user->get_acc_id());
    }
    // ORIG `mov 0x10(%ebp),%eax; mov %eax,-0xc(%ebp)` 紧邻 reply ctor 的 `lea -0x23` 之前
    // （-0x10=param 引用先、-0xc=msg 引用后；提前声明会多 -0x14 槽 ⇒ 帧 0x38 vs 0x34）。
    MSG_MEMBER_ENTER_REPLY& mref = reinterpret_cast<MSG_MEMBER_ENTER_REPLY&>(msg);
    Packet_Monitor_Member_Enter_Reply reply;   // local_27（栈对象 0x13）
    reply.m_idByChannel = user->GetUID();      // local_1d（+0xa）
    reply.m_charNo = (unsigned int)user->getCurCharacNo();  // local_19（+0xe）
    reply.m_code = mref.m_reply;    // local_15（+0x12）
    // ORIG `lea -0x23(%ebp),%ebx` 先于 GetServerGroup（P52 单条链式、禁具名 proxy 槽）
    GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendTcpPacket(reinterpret_cast<char*>(&reply), 0x13);
    return 0;
}