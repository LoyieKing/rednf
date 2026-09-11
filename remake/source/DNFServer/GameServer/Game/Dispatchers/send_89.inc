// ==== Dispatcher_StackableActionUse::send (ORIG 0x0825e4ec) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0825e4ec orig/ours=114/116 ndiff-blocks=26
//   帧差 sub $30 vs $40 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x30,%esp | mov    %eax,-0xc(%ebp) | mov    -0xc(%ebp),%eax
void Dispatcher_StackableActionUse::send(CUser* user, ParamBase& param)
{
    // [G17 2026-09-11 试形 G1] ORIG 每处访问为**成员 disp 寻址**
    //   `mov 0x4(%eax),%eax` / `movzbl 0xa(%eax),%eax` / `movzwl 0x8(%eax),%eax`；
    //   裸偏移指针算术（`*(int*)(pbase+4)`）4.4 恒发 `add $0x4,%eax; mov (%eax),%eax`
    //   （+1 条/处，共 5 处 = gap 6）。改**类型化视图 struct**做成员访问
    //   （布局与 ORIG 实证一致，尺寸 0xb 不变；TU 内匿名 struct，不发射符号）。
    struct StdViewStackableActionUse
    {
        char m_c0[4];        // +0x00
        int m_err;           // +0x04
        short m_echoSlot;    // +0x08
        char m_echoType;     // +0x0a
    };
    StdViewStackableActionUse* pbase =
        reinterpret_cast<StdViewStackableActionUse*>(&param);  // ORIG -0xc 具名指针局部
    // [G1Frest 2026-09-10] ORIG 0x0825e4fa/0x0825e541/0x0825e559/0x0825e5fd/0x0825e614：
    //   每处使用都经 pbase 重新装载（pbase+4 / pbase+0xa / pbase+8），
    //   无 err/echoSlot/echoType 具名局部（旧码 -0x0c/-0x10/-0x14 三槽为发明，P0/P48）。
    if (pbase->m_err != 0)
    {
        PacketGuard packet;
        packet.put_header(1, 0x204);                 // class=1 cmd=0x204
        packet.put_byte(0);
        // [Q4DispatchBig 2026-09-11] 范式同 :26741（ORIG `movzbl` ⇒ (unsigned char)
        //   截断；旧译 `& 0xff` 产 `and $0xff,%eax` 掩码形）。紧邻 :27816 同段先例。
        packet.put_byte(static_cast<int>(
            static_cast<unsigned char>(pbase->m_err)));
        // ORIG `movzbl 0xa(%eax),%eax; movsbl %al,%eax` 两段 = 读 uchar 后按 char 提升
        packet.put_byte((int)(char)pbase->m_echoType);
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        PacketGuard packet;
        packet.put_header(1, 0x204);
        packet.put_byte(1);
        // ORIG `movzwl 0x8(%eax),%eax; cwtl` 两段
        packet.put_short((int)pbase->m_echoSlot);
        packet.put_byte((int)(char)pbase->m_echoType);
        packet.finalize(true);
        user->Send(packet);
    }
}