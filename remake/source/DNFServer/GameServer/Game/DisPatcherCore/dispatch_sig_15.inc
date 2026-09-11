// ORIG 0x081eddf6 T _ZN15DisPatcher_Fish12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_Fish/dispatch_sig.md 全文转写）：
//   状态非 4（钓鱼中）直接返 0；为 4 时调 GameWorld::fishing，恒返 0。
int DisPatcher_Fish::dispatch_sig(CUser* user, PacketBuf& /*packet*/)

{
    if (user->get_state() != 4)
    {
        return 0;
    }
    G_GameWorld()->fishing(user);
    return 0;
}
