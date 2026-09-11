int Inter_VillageAttackedUpdate::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    // [b28c-32] ORIG 0x084dfbfb/0x084dfbfd 为 `test %al; je 主体`（bool callee 直分支，
    // 无 xor 物化）⇒ 早返形；0x084dfc24 `jne false 块` = P45 单 return 短路链；pPkt 局部
    // 赋值位于两守卫之后（ORIG +0x5d 才 mov 0x10(%ebp)→-0xc）。
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        return 0;
    // ORIG 0x84dfc08/38：G_CEnvironment() 短路双调（O0 不 CSE）。==1||==2 → 不广播。
    if (G_CEnvironment()->m_serverEnvir.m_serverType == 1 ||
        G_CEnvironment()->m_serverEnvir.m_serverType == 2)
        return 0;
    Inter_VillageAttackedUpdatePktView* pPkt = reinterpret_cast<Inter_VillageAttackedUpdatePktView*>(data);
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xf7);
    packet.put_int(pPkt->m_a);
    packet.put_int(pPkt->m_e);
    packet.put_int(pPkt->m_12);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    return 0;
}