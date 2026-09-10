int Dispatcher_CallMemberList::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081cbda2：check_error 结果写 param+0x04；
    // err>0 → 0；err<0 → LineFunc(0x225b)；err==0 → 向 monitor 请求成员列表。
    stCallMemberListParam& out = static_cast<stCallMemberListParam&>(param);
    out.m_4 = check_error(user, msg);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        LineFunc(0x225b,
                 __PRETTY_FUNCTION__,
                 out.m_4, (unsigned int)user->get_acc_id());
        return 0;
    }

    // ORIG 0x0822ba76 ctor；pkt+0x0a = GetUID(0x080c8c96)；
    // pkt+0x0e = getCurCharacNo(0x080cbc4e)；SendPacket(pkt, 0x12)。
    Packet_Monitor_Call_Member_List pkt;
    // [G1Dproc 2026-09-10] ORIG 0x081cbe3c/0x081cbe4f：
    //   `call GetUID; mov %eax,-0x14(%ebp)`、`call getCurCharacNo; mov %eax,-0x10(%ebp)`
    //   —— -0x14/-0x10 即 pkt（-0x1e）的 +0x0a/+0x0e 槽 ⇒ **disp 直写具名字段**
    //   （共享头 Packet_Monitor_Call_Member_List.h: m_idByChannel/m_charNo）；
    //   裸 char* 取址会生 lea+间接（P0/P82）。
    pkt.m_idByChannel = user->GetUID();
    pkt.m_charNo = user->getCurCharacNo();

    // ORIG 0x081cbe60 起：`lea -0x1e(%ebp),%ebx`(&pkt) → GetServerGroup → GetServerProxy
    //   → SendPacket 直链，**无** group/proxy 具名槽（P0）。
    GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendPacket(reinterpret_cast<char*>(&pkt), 0x12);
    return 0;
