// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081cb874 orig/ours=77/77 ndiff-blocks=12
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x14(%ebp),%eax | mov    %eax,-0x10(%ebp) | mov    %eax,%edx
int Dispatcher_RequestMemberEnter::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081cb874：param+4 = check_error(user,msg)；>0 返 0；<0 LineFunc(0x2192)；
    // ==0 构造 Packet_Monitor_Request_Member_Enter，UID(+6)/characNo(+0xa)，
    // memcpy(pkt+0xf, msg+0x14, *(size_t*)(msg+0x10))，Monitor 代理 SendTcpPacket(pkt, 0x31)。
    stRequestMemberEnterParam& out = static_cast<stRequestMemberEnterParam&>(param);
    out.m_4 = check_error(user, msg);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x2192,
                        __PRETTY_FUNCTION__,
                        out.m_4, user->get_acc_id());
    }
    // ORIG 0x081cb8f2: `mov 0x10(%ebp),%eax; mov %eax,-0xc(%ebp)` —— msg 具名引用槽；
    //   0x081cb91f 起长度读 `mov 0x10(%eax),%eax` 常量偏移。
    MSG_BASE& m = msg;
    struct __attribute__((packed)) ReqMemberEnterMsgView
    {
        char m_pad[0x10];
        unsigned int m_nameLen;   // +0x10：ORIG `mov 0x10(%eax),%eax` disp 直读
        char m_name[0x1e];        // +0x14：ORIG `add $0x14,%edx`
    };
    Packet_Monitor_Request_Member_Enter pkt;   // ORIG W ctor 0x0822b9ce（局部帧 -0x40 起）
    // ORIG 实测帧内偏移（pkt 基 -0x40）：0x081cb90e `mov %eax,-0x36(%ebp)` = pkt+0xa ← GetUID；
    //   0x081cb91c `mov %eax,-0x32(%ebp)` = pkt+0xe ← getCurCharacNo；
    //   0x081cb933 `lea -0x40(%ebp),%eax; add $0x12,%eax` = memcpy/strncpy 目标 pkt+0x12。
    //   （原 +6/+0xa/+0xe 偏移系早期推断，与 ORIG 差 4。）
    // [g02 2026-09-11 · Main 已批准 · P367 第 4 例] `Packet_Monitor_Request_Member_Enter`
    //   自身带具名成员（m_uid@+0x0a / m_characNo@+0x0e / m_name@+0x12）⇒ 同类型直接成员访问
    //   复现 ORIG 的单级 ebp 直存（`mov %eax,-0x36(%ebp)` / `mov %eax,-0x32(%ebp)`）；
    //   旧局部 packed 视图（含「引用+具名」「异型化」两轮试形）恒产 `lea -0x40(%ebp),%ebx;
    //   mov %eax,0xa(%ebx)` 两步 —— 视图/reinterpret 恒物化基址，只能改头。
    //   类型：ORIG 两处 store 均无转换 ⇒ 与 RHS 同型（GetUID→unsigned / getCurCharacNo→int）。
    pkt.m_uid = user->GetUID();                          // +0x0a
    pkt.m_characNo = user->getCurCharacNo();             // +0x0e
    std::strncpy(pkt.m_name,
                 reinterpret_cast<const ReqMemberEnterMsgView*>(&m)->m_name,
                 reinterpret_cast<const ReqMemberEnterMsgView*>(&m)->m_nameLen);
    // ORIG 0x081cb941：&pkt 先物化 %ebx，receiver 链内联（无 proxy 具名槽）
    GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendTcpPacket(reinterpret_cast<char*>(&pkt), 0x30);
    return 0;
}