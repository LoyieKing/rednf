void DisPatcher_RecoverStamina::send(CUser* user, ParamBase& param)
{
    stOnRecoverStaminaParam& out = static_cast<stOnRecoverStaminaParam&>(param);
    PacketGuard guard;
    guard.put_header(1, 9);
    // ORIG 0x081c...：je 目标为 put 1 块、fall-through 为 put 0 块（O0 按
    // 源码分支布局）→ 首分支写 !=0 的 Error 路径。
    if (out.m_errNo != 0)
    {
        guard.put_byte(0);
        guard.put_byte(out.m_errNo);
    }
    else
    {
        guard.put_byte(1);
        guard.put_int(user->getCurCharacInvenR()->get_money());
    }
    guard.finalize(true);
    user->Send(guard);
