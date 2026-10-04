// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x082273ae orig/ours=140/145 ndiff-blocks=7
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: lea    -0x4c(%ebp),%eax | mov    %eax,-0x42(%ebp) | lea    -0x4c(%ebp),%eax
int Dispatcher_police_queryCharacName_useMid::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // [b23S3C] ORIG 0x082273ae：岛形前置判定保留；去 characNo/world 值槽——pkt 字段
    // 预清 0 后 get_int 直接引用（ORIG `movl $0,-0x3e; lea -0x4c+0xe; get_int`），
    // 各用点 `mov -0x3e(%ebp),%eax` 回读；m_uid 成员直写单 store（probe4 实证 4.4
    // 出 `mov %eax,-off(%ebp)` RHS-first）；world 无槽链式；proxy 链式右值（ebx=&pkt）。
    if (user->get_state() <= 2 || user->getCurCharacR() == 0
        || !user->isGMUser())
    {
        return 0;
    }
    Pakcet_FindUserName_use_Charac_no request;
    request.m_uid = user->GetUID();              // +0x0a（ORIG -0x42）
    // [L7DispG1 2026-09-11 负结论] 试删具名 `unsigned int* const pNo`（ORIG 0x082273ae 段无
    //   指针槽：预清 `movl $0,-0x3e`、get_int 实参 `lea -0x4c+0xe`、回读 `mov -0x3e(%ebp)`）
    //   ⇒ 改为表达式直算后**劣化** 145→147（每用点重算 lea/add 比槽回读更贵）⇒ 回滚保具名形。
    unsigned int characNo = 0;
    packet.get_int(characNo);
    request.m_characNo = characNo;
    CUser* target = G_GameWorld()->find_from_world(
        (unsigned short)characNo);
    if (target == 0)
    {
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(
            (ENUM_SERVER_GROUP)user->GetServerGroup())->SendTcpPacket(
            reinterpret_cast<char*>(&request), 0x34);
        return 0;
    }
    size_t nameLen = strlen(target->getCurCharacName());
    if (nameLen > 0x1d)
    {
        nameLen = 0;
    }
    if (nameLen != 0)
    {
        strncpy(request.m_name,
                target->getCurCharacName(), nameLen);
        PacketGuard response;
        response.put_header(1, 0x14b);
        response.put_int(characNo);
        response.put_int((int)nameLen);
        response.put_str(request.m_name, (int)nameLen);
        response.finalize(true);
        user->Send(response);
    }
    return 0;
}