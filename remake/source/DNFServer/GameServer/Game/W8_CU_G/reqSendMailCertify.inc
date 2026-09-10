bool CUser::reqSendMailCertify()   // [b28d-Main·P292] unsigned int→bool
{
    if (GlobalData::s_capchaDataManager.isEmpty())
    {
        return 0;
    }

    MSG_UDP_CAPCHA_CERTIFY* data = GlobalData::s_capchaDataManager.peekData();
    // b28d-W8b 复测（均无效，保持原形）：ORIG 0x0868a557 `mov 0x8(%ebp),%edx;
    // add $0x8e0ec,%edx` 两步取址 vs 4.4 `lea 0x8e0ec(%eax),%edx` 单条
    //（N107 负例方向）；`&m_capchaCertify[0]` / `reinterpret_cast<char*>(&数组)`
    // 两形实测均仍产 lea（154 不变）⇒ 封 N。
    memcpy(m_capchaCertify, data, 10);

    PacketGuard packet;
    packet.put_header(0, 0x7d);
    packet.put_byte(5);
    packet.put_int(data->m_size);
    // b28d-W8 真 bug（p167 重载选错）：ORIG 调 `put_binary(char const*, int)`
    //（_ZN18InterfacePacketBuf10put_binaryEPKci）；我方 `reinterpret_cast<char*>`
    // 选中非 const 重载 `put_binary(char*, int)`（EPci）⇒ 显式 const 版本。
    packet.put_binary(reinterpret_cast<const char*>(data->m_image), data->m_size);
    packet.put_byte(0);

    unsigned short veriA = 0;
    unsigned char veriB = 0;
    unsigned short veriC = 0;
    GenerateCleanPadVeriData(veriA, veriB, veriC);
    packet.put_short(static_cast<int>(veriA));
    packet.put_byte(static_cast<int>(veriB));
    packet.put_short(static_cast<int>(veriC));

    // ORIG 0x086530xx：`mov 0x8e0fc(%eax),%eax; movzbl %al,%eax` = int 字段
    // **uchar→int 双段截断**形（probe p18.z1：`(int)(unsigned char)` 直落产
    // movzbl %al；`& 0xff` 形产 and——两 lowering 指令名不同）。
    packet.put_byte(static_cast<int>(static_cast<unsigned char>(m_humanCertifyErrorCnt)));
    packet.put_byte(1);
    packet.finalize(true);
    Send(packet);

    setHumanCertified(false);
    m_field8e3ed = false;

    if (static_cast<unsigned int>(m_humanCertifyTimerKey) > 0x7ffffffeu)
    {
        m_humanCertifyTimerKey = 0;
    }
    ++m_humanCertifyTimerKey;

    G_TimerQueue()->InsertTimer(static_cast<TimerEntry::OBJ_TYPE>(0),
                                static_cast<int>(GetUID()),
                                static_cast<TIMER_MESSAGE>(9), 0x3c,
                                m_humanCertifyTimerKey, 0xffffffffu);
    return 1;
