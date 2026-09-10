void CAssaultPlace::_BroadcastRunaway(CUser* user, int index)
{
    // b28d-solo-CAssaultMgr: **AE（归零）** — S174 曾判 NEAR（槽基平移 4B）。
    // 真因：ORIG 的 len/len2 不是独立局部，而是 **msg 成员** —— 其槽位
    // -0x11c/-0x128 恰为 msg+0xe/+0x2（msg this=-0x12a、sizeof 0x10e）；
    // 写 `msg.m_len`/`msg.packetSize` 后局部集与 ORIG 一致，`char pad1[1]`
    // 死局部（P312 旧代偿）随之删除，buf 自然落 -0x52a（mod4=2）复现 ORIG
    // 对齐剥离慢路。orig 102 = ours 102，p167 CLEAR。
    char buf[0x400] = {0};
    // b28d-L3：ORIG 0x82eab7a 段把 name→%esi、curName→%ebx 跨 notice 求值
    // 保活、三值均无槽往返（P0）⇒ 三具名局部内联为 sprintf 实参（变参右到左
    // 求值序：第4参 name 先、第3参 curName 次、第2参 notice 末）。
    sprintf(buf,
            G_CDataManager()->m_serverParameter.getAssaultUserRunawayNotice(),
            user->getCurCharacName(),
            GetAnotherTeamCharacName(index));

    Packet_Broadcast_Msg msg;
    msg.m_padA = 0;
    // ORIG 0x82eac41/0x82eac9e 直写 msg+0xe/+0x2 ⇒ 原码即成员赋值（见上）。
    msg.m_len = static_cast<char>(strlen(buf));
    memset(msg.m_text, 0, 0xff);
    strncpy(msg.m_text, buf, (unsigned char)msg.m_len);
    msg.packetSize = static_cast<short>((unsigned char)msg.m_len + 0xf);
    int packetSize = (unsigned short)msg.packetSize;
    GlobalData::s_monitor_proxy_mgr->GetServerProxy(
        user->GetServerGroup())
        ->SendPacket(reinterpret_cast<char*>(&msg), packetSize);
