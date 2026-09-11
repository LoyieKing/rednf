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
    // [G4] 视图用「引用 + 具名成员」直访问（P235 完整类型成员形）⇒ 折叠为单条 disp 栈写
    struct __attribute__((packed)) MonitorMemberEnterBody
    {
        char m_hdr[0xa];
        // [G5·P248档③] ORIG 0x081cb90e/0x081cb91c = `call GetUID; mov %eax,-0x36(%ebp)` /
        //   `call getCurCharacNo; mov %eax,-0x32(%ebp)`（RHS-first，地址不缓存）；我方同型
        //   （unsigned←unsigned、int←int）产 `lea -0x40(%ebp),%ebx; call; mov %eax,0xa(%ebx)`
        //   （LHS-first 缓存地址，+2 条）⇒ 字段与 RHS 符号性异型化。
        int m_uid;                  // +0x0a（GetUID 返回 unsigned int ⇒ 异型）
        unsigned int m_characNo;    // +0x0e（getCurCharacNo 返回 int ⇒ 异型）
        char m_name[0x1e];    // +0x12
    };
    reinterpret_cast<MonitorMemberEnterBody&>(pkt).m_uid = user->GetUID();
    reinterpret_cast<MonitorMemberEnterBody&>(pkt).m_characNo = user->getCurCharacNo();
    std::strncpy(reinterpret_cast<MonitorMemberEnterBody&>(pkt).m_name,
                 reinterpret_cast<const ReqMemberEnterMsgView*>(&m)->m_name,
                 reinterpret_cast<const ReqMemberEnterMsgView*>(&m)->m_nameLen);
    // ORIG 0x081cb941：&pkt 先物化 %ebx，receiver 链内联（无 proxy 具名槽）
    GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendTcpPacket(reinterpret_cast<char*>(&pkt), 0x30);
    return 0;
}