int Inter_VillageAttackedCountdown::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    // b28c-33：ORIG 0x84dfaf8/0x84dfb03 为 bool callee 直分支 `test %al; je 主体`
    //（无 xor 物化），且短路链两臂 `je L_true`/`jne L_false` ⇒ 早返形（正向包裹恒
    // 多一条 `xor $1` 且 ==1||==2 物化方向相反）。pPkt 局部赋值在守卫之后（ORIG +0x5d）。
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        return 0;
    if (G_CEnvironment()->m_serverEnvir.m_serverType == 1 ||
        G_CEnvironment()->m_serverEnvir.m_serverType == 2)
        return 0;
    Inter_VillageAttackedCountdownPktView* pPkt = reinterpret_cast<Inter_VillageAttackedCountdownPktView*>(data);
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xf1);
    packet.put_int(pPkt->m_a);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    return 0;
}