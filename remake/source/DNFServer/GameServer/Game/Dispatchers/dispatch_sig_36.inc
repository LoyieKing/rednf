// ORIG 0x08217ad0 T _ZN23Dispatcher_RequestMatch12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/Dispatcher_RequestMatch/dispatch_sig.md 汇编+反编译 C 全文转写）
// b28d-solo-Dispatchers verdict=AE(routed) [S206 翻正] ORIG 0x08217ad0
//   [N7→P63] 根因=两分支各自 `return 0` 各发一条 mov $0,%eax；改 if/else 单出口
//   （then call 后 jmp 共享尾 mov $0,%eax、else 直落）⇒ 归零。
int Dispatcher_RequestMatch::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() <= 2)
    {
        return 0;
    }
    PvP_Room* room = static_cast<PvP_Room*>(user->GetPVPRoom());
    if (room == 0)
    {
        return 0;
    }
    if (room->m_matching == 0)
    {
        return 0;
    }
    unsigned char flag = 0;
    if (!packet.get_byte(flag))
    {
        return LineFunc(0xc9c4,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (flag != 0)
    {
        // ORIG vtable+0x34 槽直调
        (*(int (**)(void*, void*, void*))(*(char**)room->m_matching + 0x34))(
            (void*)room->m_matching, user, room);
    }
    else
    {
        // ORIG vtable+0x30 槽直调
        (*(int (**)(void*, void*, void*))(*(char**)room->m_matching + 0x30))(
            (void*)room->m_matching, user, room);
    }
    // [b28d-solo-S206] P63 共享出口：ORIG 0x08217b9x 两分支 call 后共享尾部
    //   `mov $0x0,%eax`（then 分支 jmp 过去、else 直落）；两处独立 return 0
    //   会各发一条 mov $0,%eax（then 分支 +1 条，实测归零）。
    return 0;
}