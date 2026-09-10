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
    unsigned int* const pNo =
        reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(&request) + 0x0e);
    *pNo = 0;                                    // ORIG 预清 -0x3e
    packet.get_int(*pNo);                        // ORIG 未检查返回值
    CUser* target = G_GameWorld()->find_from_world(
        (unsigned short)*pNo);
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
        strncpy(reinterpret_cast<char*>(&request) + 0x16,
                target->getCurCharacName(), nameLen);
        PacketGuard response;
        response.put_header(1, 0x14b);
        response.put_int(*pNo);
        response.put_int((int)nameLen);
        response.put_str(reinterpret_cast<char*>(&request) + 0x16, (int)nameLen);
        response.finalize(true);
        user->Send(response);
    }
    return 0;
