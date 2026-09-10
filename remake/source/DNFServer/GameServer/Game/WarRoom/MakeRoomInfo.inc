void WarRoom::MakeRoomInfo(char* buf)
{
    // [b28d-solo-WarRoom] DIFF(67) | ORIG 0x086bdd58 | ①已修：m_pDungeon
    //   分支块序翻正（ORIG `test;je <if体>` = 非 0 直落，旧形反）；②残余为
    //   N82附 receiver-first vs arg-first（probe_rcv.cpp 十形全败）⇒ 封顶。
    //   本轮 fresh .o 复测：ndiff=45（orig 94/our 93），P267 双 cast
    //   `(unsigned char)(unsigned int)` 旋钮零变化 ⇒ 判 N7/P75。
    // [b28d-solo-WarRoom] 翻正：同 MakeStateInfo —— 局部 char* + 调用点 cast
    //   （探针 /tmp/wr_probe5.cpp t9）复现 ORIG「值先物化 %edx」形。
    char* p = buf;
    ((InterfacePacketBuf*)p)->put_short(m_index);
    ((InterfacePacketBuf*)p)->put_byte((unsigned char)m_field8);
    ((InterfacePacketBuf*)p)->put_byte((unsigned char)m_levelBand);
    ((InterfacePacketBuf*)p)->put_byte((unsigned char)m_roomSeq);
    ((InterfacePacketBuf*)p)->put_byte(m_state);
    // b28d-solo-WarRoom [P222/P191]：ORIG 086be2xx `test;je <if体>` = 非 0 直落
    // （正极性）⇒ 反写 if/else 以复现块序。
    if (m_pDungeon != 0)
        ((InterfacePacketBuf*)p)->put_short(m_pDungeon->m_warRoomData.m_10);
    else
        ((InterfacePacketBuf*)p)->put_short(0);
    for (int i = 0; i <= 5; ++i)
    {
        ((InterfacePacketBuf*)p)->put_byte(m_slotState[i]);
        // [b28d-solo-WarRoom P222] ORIG 086be2xx `test;je <0xffff 块>`：非 0 直落
        //   ⇒ 写 != 0 的正向 if 才复现（旧 == 0 形产 jne + 反序两臂）。
        if (m_slots[i] != 0)
            ((InterfacePacketBuf*)p)->put_short((m_slotsUser[i])->get_unique_id());
        else
            ((InterfacePacketBuf*)p)->put_short(0xffff);
    }
