// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081ce070 orig/ours=66/69 ndiff-blocks=13
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x14(%ebp),%eax | mov    %eax,-0xc(%ebp) | mov    %eax,%edx
int Dispatcher_RequestBlackList::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081ce070：check_error 结果写 param+0x04；
    // err>0 → 0；err<0 → LineFunc(0x2717)；err==0 → 向 monitor 请求黑名单。
    stRequestBlackListParam& out = static_cast<stRequestBlackListParam&>(param);
    out.m_4 = check_error(user, msg);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x2717,
                        __PRETTY_FUNCTION__,
                        out.m_4, (unsigned int)user->get_acc_id());
    }

    // ORIG 0x0822bbb2 ctor；pkt+0x0a = get_acc_id(0x080da36e)；
    // pkt+0x0e = GetUID(0x080c8c96)；SendPacket(pkt, 0x12)。
    Packet_Request_BlackList pkt;
    // [G1Dproc 2026-09-11] ORIG 0x081ce1xx（pkt@-0x1e）：`call get_acc_id; mov %eax,-0x14(%ebp)`
    //   = pkt+0x0a、`call GetUID; mov %eax,-0x10(%ebp)` = pkt+0x0e ⇒ **disp 直写具名字段**
    //   （shared 头 Packet_Request_BlackList.h: m_dbid@+0xa / m_idByChannel@+0xe）；
    //   裸 char* 取址会生 lea+间接（P0/P82）。
    // [父泳道修正] 头内真实布局（Packet_Request_BlackList.h）：+0x0a 起为 `char m_data[8]`
    // （无 m_dbid/m_idByChannel 成员）⇒ 经 m_data 双字视图写同一偏移（disp 直写不变）。
    // ORIG `mov %eax,-0x14(%ebp)` / `mov %eax,-0x10(%ebp)`（pkt@-0x1e 的 disp 直写槽）
    // ⇒ 用 TU 局部视图具名写同一偏移（`reinterpret_cast<unsigned int*>(pkt.m_data)` 产
    //   lea+间接 store 五条）。
    struct BlackListPktView
    {
        char m_head[0xa];
        unsigned int m_dbid;         // +0x0a
        unsigned int m_idByChannel;  // +0x0e
    } __attribute__((packed));
    reinterpret_cast<BlackListPktView*>(&pkt)->m_dbid = user->get_acc_id();        // +0x0a
    reinterpret_cast<BlackListPktView*>(&pkt)->m_idByChannel = user->GetUID();     // +0x0e

    // ORIG 0x081ce1xx：`lea -0x1e(%ebp),%ebx`(&pkt)、GetServerGroup→GetServerProxy→SendPacket
    //   直链，**无** group/proxy 具名槽（P0）。
    GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendPacket(reinterpret_cast<char*>(&pkt), 0x12);
    return 0;
}