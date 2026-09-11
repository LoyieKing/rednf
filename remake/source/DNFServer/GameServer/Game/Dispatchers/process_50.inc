// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081cbc2e orig/ours=77/77 ndiff-blocks=12
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x14(%ebp),%eax | mov    %eax,-0x10(%ebp) | mov    %eax,%edx
int Dispatcher_MemberSecede::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081cbc2e：param+4 = check_error(user,msg)；>0 返 0；<0 LineFunc(0x2228)；
    // ==0 构造 Packet_Monitor_Member_Secede，UID(+7)/characNo(+0xa)，
    // memcpy(pkt+0xf, msg+0x14, *(size_t*)(msg+0x10))，Monitor 代理 SendTcpPacket(pkt, 0x31)。
    stMemberSecedeParam& out = static_cast<stMemberSecedeParam&>(param);
    out.m_4 = check_error(user, msg);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x2228,
                        __PRETTY_FUNCTION__,
                        out.m_4, user->get_acc_id());
    }
    // ORIG `mov 0x10(%ebp),%eax; mov %eax,-0xc(%ebp)`（msg 具名引用槽 -0xc，声明先于 pkt）
    MSG_BASE& m = msg;
    // TU 局部线格式视图（不改权威头；布局=ORIG 反汇编实测）
    struct SecedePktView
    {
        char m_head[0xa];
        int m_uid;              // +0x0a（ORIG -0x37 = pkt-0x41+0xa）
        int m_characNo;         // +0x0e（ORIG -0x33 = pkt-0x41+0xe）
        char m_pad12;           // +0x12
        char m_name[0x1e];      // +0x13（memcpy dst）
    } __attribute__((packed));
    struct SecedeMsgView
    {
        char m_head[0x10];
        unsigned int m_len;     // +0x10（ORIG `mov 0x10(%eax),%eax` disp 折叠）
        char m_name[0x1e];      // +0x14
    } __attribute__((packed));
    Packet_Monitor_Member_Secede pkt;   // ORIG W ctor 0x0822ba34（局部帧 -0x41 起）
    // ORIG 帧偏移：-0x37=pkt+0xa(UID@0x09a)、-0x33=pkt+0xe(characNo@0x0a8)、
    // 名区 memcpy dst=pkt+0x13(0x0bf)、len=*(msg+0x10)、src=msg+0x14。
    // [语义修复] 旧版写 +7/+0xa/+0xf 与 ORIG 不符（UID/characNo/名区整体错位）。
    // [Q4DispatchBig 2026-09-11 负结论] 试「视图取址→引用形」
    //   （`reinterpret_cast<SecedePktView&>(pkt).m_uid = ...`，两行同改）实测
    //   仍产 `lea -0x41(%ebp),%ebx; mov %eax,0xa(%ebx)`、our 79 不变 ⇒ 4.4 对
    //   「异类型 cast 视图」恒物化基址；ORIG 的 ebp 相对单级直存
    //   （`mov %eax,-0x37(%ebp)`）只能来自**同类型直接成员访问**
    //   （即 Packet_Monitor_Member_Secede 自身带 +0x0a int 字段）⇒ 属共享头话题。
    reinterpret_cast<SecedePktView*>(&pkt)->m_uid = user->GetUID();
    reinterpret_cast<SecedePktView*>(&pkt)->m_characNo = user->getCurCharacNo();
    std::memcpy(reinterpret_cast<char*>(&pkt) + 0x13,
                reinterpret_cast<const char*>(&m) + 0x14,
                reinterpret_cast<SecedeMsgView*>(&m)->m_len);
    // ORIG 0x081cbcd9 `lea -0x41(%ebp),%ebx` 先于 GetServerGroup（P52 右→左实参物化、
    // 跨 receiver call 保活）⇒ 单条链式表达式，禁具名 proxy 局部。
    GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendTcpPacket(reinterpret_cast<char*>(&pkt), 0x31);
    return 0;
}