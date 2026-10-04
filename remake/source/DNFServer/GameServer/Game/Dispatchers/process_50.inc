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
    Packet_Monitor_Member_Secede pkt;   // ORIG W ctor 0x0822ba34（局部帧 -0x41 起）
    // ORIG 帧偏移：-0x37=pkt+0xa(UID@0x09a)、-0x33=pkt+0xe(characNo@0x0a8)、
    // 名区 memcpy dst=pkt+0x13(0x0bf)、len=*(msg+0x10)、src=msg+0x14。
    // [语义修复] 旧版写 +7/+0xa/+0xf 与 ORIG 不符（UID/characNo/名区整体错位）。
    // [g02 2026-09-11] 定案：`Packet_Monitor_Member_Secede` 自身带具名 int 成员
    //   （m_uid@+0x0a / m_characNo@+0x0e，Main 已批准加字段）⇒ 同类型直接成员访问复现
    //   ORIG 的单级 ebp 直存 `mov %eax,-0x37(%ebp)` / `mov %eax,-0x33(%ebp)`；
    //   旧形（局部 packed 视图 + reinterpret_cast）4.4 恒物化基址 `lea …,%ebx; mov %eax,0xa(%ebx)`。
    pkt.m_uid = user->GetUID();
    pkt.m_characNo = user->getCurCharacNo();
    {
        MSG_MEMBER_SECEDE& secede = static_cast<MSG_MEMBER_SECEDE&>(m);
        std::memcpy(pkt.m_name, secede.m_characName, secede.m_nameLen);
    }
    // ORIG 0x081cbcd9 `lea -0x41(%ebp),%ebx` 先于 GetServerGroup（P52 右→左实参物化、
    // 跨 receiver call 保活）⇒ 单条链式表达式，禁具名 proxy 局部。
    GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendTcpPacket(reinterpret_cast<char*>(&pkt), 0x31);
    return 0;
}