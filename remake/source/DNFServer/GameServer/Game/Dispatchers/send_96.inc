void Dispatcher_SeriaRoom_Ani_Deco_Event::send(CUser* user, ParamBase& param)
{
    // b28c-L20: N93 — ORIG(4.1) 块内 info 占浅槽 -0x10、guard 深槽 -0x1c（声明序
    //   res→info→guard）；4.4 相反（guard -0x18 / info -0x1c）。info 提前声明会引入
    //   默认构造 call（our=86 劣化），具名/提前/直接初始化三形穷举不可翻，纯槽号漂移。
    stSeriaRoomDecoEvent& res = static_cast<stSeriaRoomDecoEvent&>(param);   // ORIG -0xc
    PacketGuard guard;
    // ORIG 无 clear()（ctor 后直接 put_header），忠实保留
    guard.put_header(1, 0x201);
    if (res.m_4 == 0)
    {
        guard.put_byte(1);
        // param+8 word 零扩展入栈（movzwl/movzwl）
        guard.put_short(static_cast<int>(
            static_cast<unsigned short>(res.m_8)));
        // -0xe word（m_field2）零扩展后 cwtl 符号扩展为 int 实参
        // b28d-D0 试形：匿名 sret 临时（全表达式链）复现 ORIG -0x10 sret 槽（具名 info 落 -0x1c）。
        guard.put_short(static_cast<int>(static_cast<short>(
            user->getSeriaRoomAniDecoInfo().m_field2)));
    }
    else
    {
        guard.put_byte(0);
        guard.put_byte(res.m_4);
    }
    guard.finalize(true);
    user->Send(guard);
