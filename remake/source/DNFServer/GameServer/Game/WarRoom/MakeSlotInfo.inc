void WarRoom::MakeSlotInfo(char* buf, int seat)
{
    // [b28d-solo-WarRoom] DIFF(41) | ORIG 0x086bdea0 | N82附：同 MakeStateInfo 的
    //   receiver-first vs arg-first（ORIG 每调用点先 `mov -0x14(%ebp),%eax`）；
    //   probe_rcv.cpp 十形全败 ⇒ 封顶。语义/调用序与 ORIG 一致。
    // ORIG 0x86be1xx：buf 物化 -0x14 局部槽，后续全部从槽读（p 指针局部）。
    // [b28d-solo-WarRoom] 翻正：同 MakeStateInfo —— 局部声明 char*（与形参同型，
    //   保留 ORIG -0x14 槽 store），调用点 cast 到 InterfacePacketBuf* 复现
    //   ORIG「值先物化 %edx / receiver 后装」形（探针 /tmp/wr_probe5.cpp t9）。
    char* p = buf;
    // ORIG 0x086bde0x 首调用 receiver-first（`mov -0x14(%ebp),%eax` 先于常量压栈）；
    // 内联 cast 形（P277 反向）实测 ndiff 50 不变 ⇒ 保持 p-> 形。
    ((InterfacePacketBuf*)p)->put_header(0, 0x59);
    ((InterfacePacketBuf*)p)->put_short(m_index);
    if (seat == 0xffff)
    {
        int count = GetWaiterCount();
        ((InterfacePacketBuf*)p)->put_byte(count);
        if (count == 0)
        {
            LogManager::logFormat(1, "WarRoom.cpp", __PRETTY_FUNCTION__,
                                  0x793, "Send Member 0 PvpRoom %d State(%d)", m_index,
                                  m_state);
        }
        for (int i = 0; i <= 5; ++i)
        {
            if (m_slots[i] != 0)
            {
                ((InterfacePacketBuf*)p)->put_byte(i);
                ((InterfacePacketBuf*)p)->put_byte(m_slotState[i]);
                ((InterfacePacketBuf*)p)->put_short((m_slotsUser[i])->get_unique_id());
            }
        }
    }
    else
    {
        ((InterfacePacketBuf*)p)->put_byte(1);
        ((InterfacePacketBuf*)p)->put_byte(seat);
        ((InterfacePacketBuf*)p)->put_byte(m_slotState[seat]);
        // [b28d-solo-WarRoom P222/P191] ORIG 086be0xx `test;je <0xffff 块>`：
        //   非 0 直落（正极性）⇒ 写成 != 0 的正向 if 才复现块序。
        if (m_slots[seat] != 0)
            ((InterfacePacketBuf*)p)->put_short((m_slotsUser[seat])->get_unique_id());
        else
            ((InterfacePacketBuf*)p)->put_short(0xffff);
    }
    ((InterfacePacketBuf*)p)->finalize(true);
