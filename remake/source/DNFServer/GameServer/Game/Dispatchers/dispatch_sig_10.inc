// ORIG 0x0820748a（报告 Dispatcher_ChangeHp/dispatch_sig.md）：
// state<=1 → LineFunc；get_int hp；CAssaultMgr::OnChangeHp(user,hp)!=1 → LineFunc。
int Dispatcher_ChangeHp::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() <= 1)
    {
        return LineFunc(0xa206,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    int hp = 0;
    if (!packet.get_int(hp))
    {
        return LineFunc(0xa20a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // P22：OnChangeHp 已按调用点 xor $1 证据改 bool 返回；`!f()` 直折 xor/test。
    if (!pvp_assault::GetInstanceAssaultMgr()->OnChangeHp(user, hp))
    {
        return LineFunc(0xa20d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}